#include "test_framework.hpp"

namespace at {
TestFailureException::TestFailureException(
  std::int64_t     lineNumber,
  std::string_view functionName,
  std::string_view failingExpression,
  std::string_view expectedResult,
  std::string_view actualResult)
  : std::runtime_error{formatMessage(
    lineNumber,
    functionName,
    failingExpression,
    expectedResult,
    actualResult)}
{
}

std::string TestFailureException::formatMessage(
  std::int64_t     lineNumber,
  std::string_view functionName,
  std::string_view failingExpression,
  std::string_view expectedResult,
  std::string_view actualResult)
{
  std::ostringstream outputStringStream{};
  outputStringStream.imbue(std::locale::classic());

  outputStringStream << "TestFailureException:\n\"" << failingExpression
                     << "\" failed!\n"
                        "Line    : "
                     << lineNumber
                     << "\n"
                        "Function: "
                     << functionName
                     << "\n"
                        "Expected:\n"
                     << expectedResult
                     << "\n"
                        "Actual  : \n"
                     << actualResult << "\n\n";

  return outputStringStream.str();
}
} // namespace at
