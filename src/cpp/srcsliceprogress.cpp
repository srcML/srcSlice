#include "srcsliceprogress.hpp"

std::string terminalOutput;

std::string format_time(double seconds) {
    int minutes = static_cast<int>(seconds / 60);
    double sec_rem = seconds - (minutes * 60);

    std::ostringstream out;
    out << minutes << "m" << std::fixed << std::setprecision(3) << sec_rem << "s";
    return out.str();
}

//#############################################################################
//#############################################################################

ProgressBar::ProgressBar(size_t c): count(c), i(0) {
    oldCoutBuf = std::cout.rdbuf();
    oldCerrBuf = std::cerr.rdbuf();

    actionFinished = false;

    std::cout << "[*] Attempting to Process " << count << " units. . ." << std::endl;

    // start the display thread that shows how far the progress of an action
    // is near real-time
    displayThread = std::thread(&ProgressBar::Update, this);
}

ProgressBar::~ProgressBar() {
    // ensure thread clean up occurs on obj dtor
    actionFinished = true;
    if (displayThread.joinable()) displayThread.join();
    
    std::cout.rdbuf(oldCoutBuf);
    std::cerr.rdbuf(oldCerrBuf);
}

// stdout buffers are set to normal destinations when this executes
void ProgressBar::Finish() {
    if (!actionFinished) {
        actionFinished = true;
        if (displayThread.joinable()) displayThread.join();
    }
    
    std::cout.rdbuf(oldCoutBuf);
    std::cerr.rdbuf(oldCerrBuf);

    printf("\r\033[2K");
    std::cout << "[+] Finished Processing!" << std::endl;
};

void ProgressBar::Increment() {
    if (i < count) {
        ++i;
    } else {
        actionFinished = true;
    }
}

// continuously ran and ensures progress bar only displays once
// and is always at the bottom of the screen pushing other stdout
// prints above it
void ProgressBar::Update() {
    while (!actionFinished) {
        usleep(2000);
        Log();
    }
}

// Custom printf logic where cout and cerr appear above the status bar
void ProgressBar::Log() {
    if (!terminalOutput.empty()) {
        printf("\r\033[2K");      // Clear the current line
        printf("%s", terminalOutput.c_str());
        terminalOutput.clear();
    }

    // ANSI Escapes
    printf("\033[s");             // Save current position
    printf("\r\033[2K");          // Clear the current line

    // calculate percentage of completion
    double rawPercentage = count > 0 ? static_cast<double>(i) / count * 100.0 : 100.0;
    double percentage = std::round(rawPercentage * 10.0) / 10.0;  // Round to 1 decimal place

    // characters for the progress bar
    int barSize = 25;
    int barFill = (int)((double)barSize * (percentage/100.0));

    // draw the progress bar
    printf("[");
    for (size_t j = 0; j < barSize; ++j) {
        printf("%s", (j < barFill) ? "░" : "█");
    }
    printf("] %.1f%%\r", percentage);

    fflush(stdout);
}

//#############################################################################
//#############################################################################

IdleBar::IdleBar() {
    // track default buffers for out streams
    oldCoutBuf = std::cout.rdbuf();
    oldCerrBuf = std::cerr.rdbuf();

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
    
    std::cout.rdbuf(oldCoutBuf);
    std::cerr.rdbuf(oldCerrBuf);
}

// stdout buffers are set to normal destinations when this executes
void IdleBar::Finish() {
    if (!actionFinished) {
        actionFinished = true;
        endTime = std::chrono::steady_clock::now();
        if (displayThread.joinable()) displayThread.join();
    }
    
    std::cout.rdbuf(oldCoutBuf);
    std::cerr.rdbuf(oldCerrBuf);
    
    // Show elapsed time
    endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    printf("\r\033[2K");
    std::cout << "final time elapsed    " << format_time(elapsed) << std::endl;
};

// continuously ran and ensures progress bar only displays once
// and is always at the bottom of the screen pushing other stdout
// prints above it
void IdleBar::Update() {
    while (!actionFinished) {
        usleep(2000);
        Log();
    }
}

// Custom printf logic where cout and cerr appear above the status bar
void IdleBar::Log() {
    if (!terminalOutput.empty()) {
        printf("\r\033[2K");      // Clear the current line
        printf("%s", terminalOutput.c_str());
        terminalOutput.clear();
    }

    // ANSI Escapes
    printf("\033[s");             // Save current position
    printf("\r\033[2K");          // Clear the current line

    // Show elapsed time
    endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    printf("time elapsed\t%s\r", format_time(elapsed).c_str());

    fflush(stdout);
}

//#############################################################################
//#############################################################################

CallbackBuf::CallbackBuf(Callback cb) : callback(cb) {}

std::streambuf::int_type CallbackBuf::overflow(int_type ch) {
    if (ch != traits_type::eof()) {
        lineBuffer += static_cast<char>(ch);
        if (ch == '\n') {
            flushLine();
        }
    }
    return ch;
}

std::streamsize CallbackBuf::xsputn(const char* s, std::streamsize n) {
    for (std::streamsize i = 0; i < n; ++i) {
        overflow(static_cast<unsigned char>(s[i]));
    }
    return n;
}

int CallbackBuf::sync() {
    if (!lineBuffer.empty()) {
        flushLine();  // Flush even if it’s not a full line
    }
    return 0;
}

void CallbackBuf::flushLine() {
    callback(lineBuffer);
    lineBuffer.clear();
}

std::string coutLineBuffer;
std::string cerrLineBuffer;

// Sample callback functions for cout and cerr
void coutHandler(const std::string& s) {
    // std::cout << s;
    for (char c : s) {
        coutLineBuffer += c;
        if (c == '\n') {
            terminalOutput += coutLineBuffer;
            coutLineBuffer.clear();
        }
    }
}

void cerrHandler(const std::string& s) {
    // std::cerr << s;
    for (char c : s) {
        cerrLineBuffer += c;
        if (c == '\n') {
            terminalOutput += cerrLineBuffer;
            cerrLineBuffer.clear();
        }
    }
}