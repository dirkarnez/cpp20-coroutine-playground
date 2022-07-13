cpp20-coroutine-playground
==========================
### Reference
- [C++20 Coroutines — Complete* Guide | by Šimon Tóth | ITNEXT](https://itnext.io/c-20-coroutines-complete-guide-7c3fc08db89d)
- [如何編寫 C++ 20 協程(Coroutines)_多顆糖 - MdEditor](https://www.gushiciku.cn/pl/g0CZ/zh-tw)
- [c++20 coroutine 实现的 generator 可以被优化成常数 - V2EX](https://www.v2ex.com/t/865556)
  - ```cpp
    #include <experimental/coroutine>
    #include <numeric>

    namespace stdx = std::experimental;

    template <typename T> struct generator {
      struct promise_type {
        T current_value;
        stdx::suspend_always yield_value(T value) {
          this->current_value = value;
          return {};
        }
        stdx::suspend_always initial_suspend() { return {}; }
        stdx::suspend_always final_suspend() { return {}; }
        generator get_return_object() { return generator{this}; };
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
      };

      struct iterator {
        stdx::coroutine_handle<promise_type> _Coro;
        bool _Done;

        iterator(stdx::coroutine_handle<promise_type> Coro, bool Done)
            : _Coro(Coro), _Done(Done) {}

        iterator &operator++() {
          _Coro.resume();
          _Done = _Coro.done();
          return *this;
        }

        bool operator==(iterator const &_Right) const {
          return _Done == _Right._Done;
        }

        bool operator!=(iterator const &_Right) const { return !(*this == _Right); }
        T const &operator*() const { return _Coro.promise().current_value; }
        T const *operator->() const { return &(operator*()); }
      };

      iterator begin() {
        p.resume();
        return {p, p.done()};
      }

      iterator end() { return {p, true}; }

      generator(generator const&) = delete;
      generator(generator &&rhs) : p(rhs.p) { rhs.p = nullptr; }

      ~generator() {
        if (p)
          p.destroy();
      }

    private:
      explicit generator(promise_type *p)
          : p(stdx::coroutine_handle<promise_type>::from_promise(*p)) {}

      stdx::coroutine_handle<promise_type> p;
    };

    template <typename T>
    generator<T> seq() {
      for (T i = {};; ++i)
        co_yield i;
    }

    template <typename T>
    generator<T> take_until(generator<T>& g, T limit) {
      for (auto&& v: g)
        if (v < limit) co_yield v;
        else break;
    }

    template <typename T>
    generator<T> multiply(generator<T>& g, T factor) {
      for (auto&& v: g)
        co_yield v * factor;
    }

    template <typename T>
    generator<T> add(generator<T>& g, T adder) {
      for (auto&& v: g)
        co_yield v + adder;
    }

    int main() {
      auto s = seq<int>();
      auto t = take_until(s, 10);
      auto m = multiply(t, 2);
      auto a = add(m, 110);
      return std::accumulate(a.begin(), a.end(), 0);
    }
    ```
