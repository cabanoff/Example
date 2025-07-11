// Singleton.h
#pragma once

template <typename T>
class Singleton {
public:
    static T& instance() {
        static T inst;
        return inst;
    }

protected:
    Singleton() {}
    ~Singleton() {}

private:
    // Prevent copying
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
};