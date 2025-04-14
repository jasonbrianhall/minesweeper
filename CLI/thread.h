// thread.h - Minimal thread implementation for MS-DOS/DJGPP
#ifndef DOS_THREAD_H
#define DOS_THREAD_H

#include <dos.h>
#include <functional>

// Define a namespace to avoid conflicts with std::chrono
namespace dos_compat {
    // This is our replacement for std::thread
    class thread {
    private:
        bool is_active;
        std::function<void()> thread_func; // Store the function to call
        
    public:
        thread() : is_active(false) {}
        
        // Constructor for regular functions
        thread(void (*func)()) : is_active(true), thread_func(func) {}
        
        // Special constructor for member functions
        template<class T>
        thread(void (T::*func)(), T* obj) : is_active(true) {
            // Store the member function and object to call it on
            thread_func = [func, obj]() { (obj->*func)(); };
        }
        
        bool joinable() const {
            return is_active;
        }
        
        void join() {
            is_active = false;
        }
        
        // Execute one "tick" of the thread function
        void tick() {
            if (is_active && thread_func) {
                thread_func();
            }
        }
        
        ~thread() {
            if (joinable()) {
                join();
            }
        }
    };
    
    // This is our replacement for std::this_thread
    namespace this_thread {
        inline void sleep_for(int milliseconds) {
            delay(milliseconds);
        }
    }
}

// Aliases for compatibility
namespace std {
    using dos_compat::thread;
    
    namespace this_thread {
        using dos_compat::this_thread::sleep_for;
    }
}

#endif /* DOS_THREAD_H */
