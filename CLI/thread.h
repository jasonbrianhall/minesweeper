// thread.h - Minimal thread implementation for MS-DOS/DJGPP
#ifndef DOS_THREAD_H
#define DOS_THREAD_H

#include <dos.h>

// Define a namespace to avoid conflicts with std::chrono
namespace dos_compat {
    // This is our replacement for std::thread
    class thread {
    private:
        bool is_active;
        
    public:
        thread() : is_active(false) {}
        
        // Special constructor for member functions
        template<class T>
        thread(void (T::*func)(), T* obj) : is_active(true) {
            // In MS-DOS, we don't actually create a thread
            // Just mark as active for compatibility
        }
        
        bool joinable() const {
            return is_active;
        }
        
        void join() {
            is_active = false;
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
