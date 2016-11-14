#include <limits.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace std {
  struct true_type {
    static constexpr bool value = true;
  };
  constexpr bool true_type::value;

  struct false_type {
    static constexpr bool value = false;
  };
  constexpr bool false_type::value;

  template<class T> struct is_integral : public false_type {};
  template<> struct is_integral<char> : public true_type {};
  template<> struct is_integral<short> : public true_type {};
  template<> struct is_integral<int> : public true_type {};
  template<> struct is_integral<long> : public true_type {};
  template<> struct is_integral<unsigned char> : public true_type {};
  template<> struct is_integral<unsigned short> : public true_type {};
  template<> struct is_integral<unsigned int> : public true_type {};
  template<> struct is_integral<unsigned long> : public true_type {};

  template<class T> struct numeric_limits {
    static T min() { return T(); }
    static T max() { return T(); }
  };
  template<> struct numeric_limits<char> {
    static char min() { return CHAR_MIN; }
    static char max() { return CHAR_MAX; }
  };
  template<> struct numeric_limits<int> {
    static int min() { return INT_MIN; }
    static int max() { return INT_MAX; }
  };
  template<> struct numeric_limits<short int> {
    static short int min() { return SHRT_MIN; }
    static short int max() { return SHRT_MAX; }
  };
  template<> struct numeric_limits<long> {
    static long min() { return LONG_MIN; }
    static long max() { return LONG_MAX; }
  };
  template<> struct numeric_limits<unsigned char> {
    static unsigned char min() { return 0; }
    static unsigned char max() { return UCHAR_MAX; }
  };
  template<> struct numeric_limits<unsigned int> {
    static unsigned int min() { return 0; }
    static unsigned int max() { return UINT_MAX; }
  };
  template<> struct numeric_limits<unsigned short int> {
    static unsigned short int min() { return 0; }
    static unsigned short int max() { return USHRT_MAX; }
  };
  template<> struct numeric_limits<unsigned long> {
    static unsigned long min() { return 0; }
    static unsigned long max() { return ULONG_MAX; }
  };

  // function - functor wrapper

  // Typed invoker class

  template <class FT, class RT, class... AT> class Invoker {
  public:
    static RT invoke(void *x, AT... args) {
      FT& functor = *(FT *)x;
      return functor(args...);
    }
  };
  
  // Wrapper class
  template <class T> class function;
  template <class RT, class... AT> class function<RT (AT...)> {
  private:
    void *functor;
    RT (*invoker)(void *, AT...);
  public:
    template <typename FT>
      function(const FT& f) {
      functor = new FT(f);
      invoker = Invoker<FT,RT,AT...>::invoke;
    }
    RT operator ()(AT... args) {
      return (*invoker)(functor, args...);
    }
  };
}
