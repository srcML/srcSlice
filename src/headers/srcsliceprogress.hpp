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
#include <cmath>

// Type Alias
using TimePoint = std::chrono::steady_clock::time_point;

std::string format_time(double seconds);

class CallbackBuf : public std::streambuf {
public:
    using Callback = void(*)(const std::string&);
    CallbackBuf(Callback cb);
protected:
    int_type overflow(int_type ch) override;
    int sync() override;
    std::streamsize xsputn(const char* s, std::streamsize n) override;
private:
    std::string lineBuffer;
    Callback callback;

    void flushLine();
};

// Sample callback functions for cout and cerr
void coutHandler(const std::string& s);
void cerrHandler(const std::string& s);

class ProgressBar {
public:
    ProgressBar(size_t c);
    ~ProgressBar();

    void Increment();
    void Finish();
private:
    void Update();
    void Log();

    size_t count;
    size_t i;
    bool actionFinished;
    std::thread displayThread;
    std::streambuf* oldCoutBuf;
    std::streambuf* oldCerrBuf;
};

class IdleBar {
public:
    IdleBar();
    ~IdleBar();
    void Finish();
private:
    void Update();
    void Log();

    TimePoint startTime;
    TimePoint endTime;
    bool actionFinished;
    std::thread displayThread;
    std::streambuf* oldCoutBuf;
    std::streambuf* oldCerrBuf;
};

#endif