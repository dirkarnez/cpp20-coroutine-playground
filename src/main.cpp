#include <coroutine>
#include <iostream>

using namespace std;

template<class T>
struct Task {
    struct promise_type {
        promise_type() { 
            cout << "@ promise_type created\n"; 
        }
        
        ~promise_type() { 
            cout << "@ promise_type died\n"; 
        }
        
        auto get_return_object() {
            cout << "@ get_return_object called\n";
            return Task<T>{Handle::from_promise(*this)};
        }
           
        std::suspend_always initial_suspend() { 
            cout << "@ initial_suspend is called\n";
            return {}; 
        }
        
        std::suspend_never final_suspend() noexcept { 
            cout << "@ final_suspend is called\n";
            return {}; 
        }
        
        auto return_value(T v) { 
            cout << "@ return_value is called\n";
            value = v;
            return std::suspend_never{}; 
            // dont suspend it (return std::experimental::suspend_always{};)
        }
        
        void unhandled_exception() { 
            cout << "@ unhandled_exception is called\n";
            std::exit(1);
        }
        
        T value;
    };
        
    using Handle = std::coroutine_handle<promise_type>;
    
    // ~Task() { 
    //     cout << "@ Task gone\n"; 
    //     if (coro_) {
    //         coro_.destroy();
    //     }
    // }
    
    // void resume() { 
    //     cout << "@ Task resume\n"; 
    //     coro_.resume(); 
    // }

    explicit Task<T>(Handle coro) : coro_(coro) {}
    
    T get()
    {
        cout << "@ get is called\n";
        if (!(this->coro_.done()))
        {
            coro_.resume();  //resume
            return coro_.promise().value;
        }
        
        return 0;
    }
    
private:
    Handle coro_;
};

Task<int> myCoroutine() {
    co_return 1;// make it a coroutine
}

int main() {
    Task<int> c = myCoroutine();
    cout << "created a corutine, try to get a value\n";
    int an = c.get();
    cout << "value is " << an << endl;

    cin.get();
}
