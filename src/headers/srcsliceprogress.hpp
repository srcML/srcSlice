#ifndef SRCSLICEPROGRESS
#define SRCSLICEPROGRESS
#include <iostream>
#include <cstdio>
#include <string>
#include <streambuf>
#include <sstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <iomanip>
#include <mutex>

// Type Alias
using TimePoint = std::chrono::steady_clock::time_point;

std::string format_time(double seconds) {
    int minutes = static_cast<int>(seconds / 60);
    double sec_rem = seconds - (minutes * 60);

    std::ostringstream out;
    out << minutes << "m" << std::fixed << std::setprecision(3) << sec_rem << "s";
    return out.str();
}

/*
class ProgressBar {
public:
    ProgressBar(size_t c): actionFinished(false), count(c), i(0) {
        startTime = std::chrono::steady_clock::now();
        // start the display thread that shows how far the progress of an action
        // is near real-time
        displayThread = std::thread(&ProgressBar::Display, this);
    }

    ~ProgressBar() {
        // ensure thread clean up occurs on obj dtor
        actionFinished = true;
        if (displayThread.joinable()) displayThread.join();
    }

    void Update() {
        if (i < count) {
            ++i;
        } else {
            if (!actionFinished) {
                actionFinished = true;
                endTime = std::chrono::steady_clock::now();
                if (displayThread.joinable()) displayThread.join();
                printf("\033[2K");
            }

            // show time elapsed
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            std::cout << "time elapsed\t" << format_time(elapsed) << "\n";
        }
    };
private:
    // continuously ran and ensures progress bar only displays once
    // and is always at the bottom of the screen pushing other stdout
    // prints above it
    void Display() {
        while (!actionFinished) {
            usleep(200000); // 200ms
            
            // Initialize char for printing loading bar
            char a = 177, b = 219;

            // ANSI Escapes
            printf("\033[s");          // Save current position
            printf("\033[999B");       // Move far down (bottom of terminal)
            printf("\r");              // Return to beginning of line
            printf("\033[2K");         // Clear line

            // Draw loading bar
            for (size_t j = 0; j < count; ++j) {
                putchar(j < i ? b : a);
            }

            fflush(stdout);            // Make sure it's flushed
            printf("\033[u");
        }
    }

    size_t count;
    size_t i;
    TimePoint startTime;
    TimePoint endTime;
    bool actionFinished;
    std::thread displayThread;
};
*/

std::string terminalOutput;
class IdleBar {
public:
    IdleBar(): actionFinished(false) {
        startTime = std::chrono::steady_clock::now();
        // start the display thread that shows how far the progress of an action
        // is near real-time
        displayThread = std::thread(&IdleBar::Update, this);
    }

    ~IdleBar() {
        // ensure thread clean up occurs on obj dtor
        actionFinished = true;
        if (displayThread.joinable()) displayThread.join();
    }

    void Finish() {
        if (!actionFinished) {
            actionFinished = true;
            endTime = std::chrono::steady_clock::now();
            if (displayThread.joinable()) displayThread.join();
        }
        
        printf("\033[s");
        printf("\033[999B");
        printf("\r\033[2K");
        printf("");
        fflush(stdout);
        printf("\033[u");
    };

    void Status() {
        // Show elapsed time
        endTime = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        std::cout << "final time elapsed\t" << format_time(elapsed) << std::endl;
    }
private:
    // continuously ran and ensures progress bar only displays once
    // and is always at the bottom of the screen pushing other stdout
    // prints above it
    void Update() {
        while (!actionFinished) {
            usleep(2000);
            Log();
        }
    }

    // Custom printf logic where cout and cerr appear above the status bar
    void Log() {
        // ANSI Escapes
        printf("\033[s");          // Save current position
        printf("\033[999B");       // Move far down (bottom of terminal)
        printf("\r\033[2K");       // Return to start and clear line

        // Show elapsed time
        endTime = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        if (terminalOutput.size() > 0) printf("%s\n", terminalOutput.c_str());
        printf("time elapsed\t%s\r", format_time(elapsed).c_str());

        fflush(stdout);
        printf("\033[u");          // Restore original position
    }

    TimePoint startTime;
    TimePoint endTime;
    bool actionFinished;
    std::thread displayThread;
};

// Custom streambuf that calls a callback on output
class CallbackBuf : public std::streambuf {
public:
    using Callback = void(*)(const std::string&);

    CallbackBuf(Callback cb) : callback(cb) {}

protected:
    // Buffer size
    static const int bufferSize = 256;
    char buffer[bufferSize];

    // Called when buffer is full or flush requested
    int_type overflow(int_type ch) override {
        if (ch != traits_type::eof()) {
            buffer[0] = ch;
            writeBuffer(1);
        }
        return ch;
    }

    int sync() override {
        writeBuffer(pbase() - pptr());
        setp(buffer, buffer + bufferSize - 1);
        return 0;
    }

    // Called when characters are inserted into the buffer
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        // Directly call callback for simplicity
        callback(std::string(s, n));
        return n;
    }

private:
    Callback callback;

    void writeBuffer(std::streamsize n) {
        if (n > 0) {
            callback(std::string(buffer, n));
        }
    }
};

// Sample callback functions for cout and cerr
void coutHandler(const std::string& s) {
    // std::cout << s;
    terminalOutput = s;
}

void cerrHandler(const std::string& s) {
    // std::cerr << s;
    terminalOutput = s;
}

#endif