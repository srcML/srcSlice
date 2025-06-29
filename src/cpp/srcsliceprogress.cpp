#include "srcsliceprogress.hpp"

std::string format_time(double seconds) {
    int minutes = static_cast<int>(seconds / 60);
    double sec_rem = seconds - (minutes * 60);

    std::ostringstream out;
    out << minutes << "m" << std::fixed << std::setprecision(3) << sec_rem << "s";
    return out.str();
}

IdleBar::IdleBar() {
    actionFinished = false;
    startTime = std::chrono::steady_clock::now();
    // start the display thread that shows how far the progress of an action
    // is near real-time
    displayThread = std::thread(&IdleBar::Update, this);
}

IdleBar::~IdleBar() {
    // ensure thread clean up occurs on obj dtor
    actionFinished = true;
    if (displayThread.joinable()) displayThread.join();
}

void IdleBar::Finish() {
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

void IdleBar::Status() {
    // Show elapsed time
    endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    std::cout << "final time elapsed\t" << format_time(elapsed) << std::endl;
}

// continuously ran and ensures progress bar only displays once
// and is always at the bottom of the screen pushing other stdout
// prints above it
void IdleBar::Update() {
    while (!actionFinished) {
        usleep(2000);
        Log();
    }
}

std::string terminalOutput;
// Custom printf logic where cout and cerr appear above the status bar
void IdleBar::Log() {
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

CallbackBuf::CallbackBuf(Callback cb) : callback(cb) {}

std::streambuf::int_type CallbackBuf::overflow(int_type ch) {
    if (ch != traits_type::eof()) {
        buffer[0] = ch;
        writeBuffer(1);
    }
    return ch;
}

int CallbackBuf::sync() {
    writeBuffer(pbase() - pptr());
    setp(buffer, buffer + bufferSize - 1);
    return 0;
}

std::streamsize CallbackBuf::xsputn(const char* s, std::streamsize n) {
    callback(std::string(s, n));
    return n;
}

void CallbackBuf::writeBuffer(std::streamsize n) {
    if (n > 0) {
        callback(std::string(buffer, n));
    }
}

// Sample callback functions for cout and cerr
void coutHandler(const std::string& s) {
    // std::cout << s;
    terminalOutput = s;
}

void cerrHandler(const std::string& s) {
    // std::cerr << s;
    terminalOutput = s;
}