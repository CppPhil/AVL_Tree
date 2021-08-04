#pragma once
#include <cstdint>

#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "avl_tree.hpp"

#ifdef _MSC_VER
#define AT_FUNCTION __FUNCSIG__
#else
#define AT_FUNCTION __PRETTY_FUNCTION__
#endif

namespace at {
class TestFailureException : public std::runtime_error {
public:
  TestFailureException(
    std::int64_t     lineNumber,
    std::string_view functionName,
    std::string_view failingExpression,
    std::string_view expectedResult,
    std::string_view actualResult);

private:
  static std::string formatMessage(
    std::int64_t     lineNumber,
    std::string_view functionName,
    std::string_view failingExpression,
    std::string_view expectedResult,
    std::string_view actualResult);
};

template<typename Any>
std::string toString(const Any& any)
{
  std::ostringstream outputStringStream{};

  outputStringStream.imbue(std::locale::classic());
  outputStringStream << std::boolalpha;

  using rev_it       = ::at::AvlTree<int, int>::reverse_iterator;
  using const_rev_it = ::at::AvlTree<int, int>::const_reverse_iterator;

  if constexpr (std::is_same_v<Any, rev_it>) {
    outputStringStream << "AvlTree::reverse_iterator{" << any.base() << '}';
  }
  else if constexpr (std::is_same_v<Any, const_rev_it>) {
    outputStringStream << "AvlTree::const_reverse_iterator{" << any.base()
                       << '}';
  }
  else {
    outputStringStream << any;
  }

  return outputStringStream.str();
}

#define AT_THROW(expectedResult, actualResult, op)                   \
  throw ::at::TestFailureException                                   \
  {                                                                  \
    __LINE__, AT_FUNCTION, #expectedResult " " op " " #actualResult, \
      ::at::toString(expectedResult), ::at::toString(actualResult)   \
  }

#define AT_ASSERT_EQ(expectedResult, actualResult)  \
  do {                                              \
    if (!((expectedResult) == (actualResult))) {    \
      AT_THROW(expectedResult, actualResult, "=="); \
    }                                               \
  } while (false)

#define AT_ASSERT_NE(expectedResult, actualResult)  \
  do {                                              \
    if (!((expectedResult) != (actualResult))) {    \
      AT_THROW(expectedResult, actualResult, "!="); \
    }                                               \
  } while (false)

using TestFunction = void (*)();

struct TestFunctionWithIdentifier {
  TestFunction     function;
  std::string_view identifier;
};

#define AT_TEST(testIdentifier)                                              \
  void testIdentifier();                                                     \
  class testIdentifier##Class {                                              \
  public:                                                                    \
    testIdentifier##Class()                                                  \
    {                                                                        \
      testFunctions.push_back(                                               \
        ::at::TestFunctionWithIdentifier{&testIdentifier, #testIdentifier}); \
    }                                                                        \
  };                                                                         \
  testIdentifier##Class testIdentifier##instance{};                          \
  void                  testIdentifier()
} // namespace at
