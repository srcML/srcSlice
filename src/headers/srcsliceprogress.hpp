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

std::string format_time(double seconds);

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

class IdleBar {
public:
    IdleBar();
    ~IdleBar();
    void Finish();
    void Status();
private:
    void Update();
    void Log();

    TimePoint startTime;
    TimePoint endTime;
    bool actionFinished;
    std::thread displayThread;
};

class CallbackBuf : public std::streambuf {
public:
    using Callback = void(*)(const std::string&);
    CallbackBuf(Callback cb);
protected:
    int_type overflow(int_type ch) override;
    int sync() override;
    std::streamsize xsputn(const char* s, std::streamsize n) override;
private:
    static const int bufferSize = 256;
    char buffer[bufferSize];
    Callback callback;
    void writeBuffer(std::streamsize n);
};

// Sample callback functions for cout and cerr
void coutHandler(const std::string& s);
void cerrHandler(const std::string& s);

#endif