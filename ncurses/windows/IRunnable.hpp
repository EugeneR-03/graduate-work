#pragma once

struct IRunnable {
    virtual void run() = 0;
    virtual void stop() = 0;
    virtual bool is_running() = 0;
};