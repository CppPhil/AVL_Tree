#ifdef DEBUG_MODE
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif /* DEBUG_MODE */

#include <cctype>
#include <cstddef>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <string>

#include "avl_tree.hpp"
#include "test_framework.hpp"

using namespace std::string_literals;

std::vector<at::TestFunctionWithIdentifier> testFunctions{};

using Tree = at::AvlTree<int, int>;

static Tree testTree()
{
  Tree t{};

  for (int i{1}; i <= 10; ++i) {
    t.insert(i, i);
  }

  return t;
}

static std::string trimmed(std::string s)
{
  while (!s.empty() && std::isspace(s.front())) {
    s.erase(s.begin());
  }

  while (!s.empty() && std::isspace(s.back())) {
    s.pop_back();
  }

  return s;
}

static std::mt19937_64 createURBG()
{
  constexpr std::size_t byteCount{
    std::mt19937_64::state_size * sizeof(std::mt19937_64::result_type)};
  std::random_device randomDevice{};
  std::array<
    std::random_device::result_type,
    (byteCount - 1) / sizeof(std::random_device::result_type) + 1>
    seedBuffer{};
  std::generate(seedBuffer.begin(), seedBuffer.end(), [&randomDevice] {
    return randomDevice();
  });
  std::seed_seq seedSequence(seedBuffer.begin(), seedBuffer.end());
  return std::mt19937_64{seedSequence};
}

AT_TEST(shouldBeAbleToDefaultConstruct)
{
  Tree t{};
  AT_ASSERT_EQ(0, t.size());
  AT_ASSERT_EQ(true, t.empty());
  AT_ASSERT_EQ(t.begin(), t.end());
  AT_ASSERT_EQ(t.cbegin(), t.cend());
  AT_ASSERT_EQ(t.rbegin(), t.rend());
  AT_ASSERT_EQ(t.crbegin(), t.crend());
}

AT_TEST(shouldBeAbleToConstructFromIterators)
{
  const std::vector<std::pair<int, int>> vector{{1, 2}, {2, 4}, {3, 6}, {4, 8}};
  Tree                                   t{vector.begin(), vector.end()};
  AT_ASSERT_EQ(vector.size(), t.size());
  AT_ASSERT_EQ(false, t.empty());

  for (const std::pair<int, int>& pair : vector) {
    const int key{pair.first};
    const int value{pair.second};

    const Tree::iterator iter{t.find(key)};
    AT_ASSERT_NE(t.end(), iter);
    AT_ASSERT_EQ(key, iter->first);
    AT_ASSERT_EQ(value, iter->second);
  }

  const std::string actualTreeString{at::toString(t)};
  const std::string expectedTreeString{trimmed(R"(
|=====1 => 2
|
|
2 => 4
|
|
|=====3 => 6
|
|
|===========4 => 8)")};

  AT_ASSERT_EQ(expectedTreeString, actualTreeString);
}

AT_TEST(shouldBeAbleToConstructFromInitializerList)
{
  Tree t{{4, 2}, {8, 4}, {16, 8}, {20, 10}, {40, 20}};

  AT_ASSERT_EQ(5, t.size());
  AT_ASSERT_EQ(false, t.empty());

  Tree::iterator it{t.find(4)};
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(4, it->first);
  AT_ASSERT_EQ(2, it->second);

  it = t.find(8);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(8, it->first);
  AT_ASSERT_EQ(4, it->second);

  it = t.find(16);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(16, it->first);
  AT_ASSERT_EQ(8, it->second);

  it = t.find(20);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(20, it->first);
  AT_ASSERT_EQ(10, it->second);

  it = t.find(40);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(40, it->first);
  AT_ASSERT_EQ(20, it->second);

  const std::string expectedTreeString{trimmed(R"(
|=====4 => 2
|
|
8 => 4
|
|
|===========16 => 8
|
|
|=====20 => 10
|
|
|===========40 => 20)")};
  AT_ASSERT_EQ(expectedTreeString, at::toString(t));
}

AT_TEST(shouldBeAbleToCopyConstruct)
{
  const Tree t{testTree()};
  const Tree copy{t};

  AT_ASSERT_EQ(10, t.size());
  AT_ASSERT_EQ(10, copy.size());
  AT_ASSERT_EQ(false, t.empty());
  AT_ASSERT_EQ(false, copy.empty());

  for (int i{1}; i <= 10; ++i) {
    const Tree::const_iterator tIt{t.find(i)};
    const Tree::const_iterator copyIt{copy.find(i)};
    AT_ASSERT_NE(t.end(), tIt);
    AT_ASSERT_NE(copy.end(), copyIt);
    AT_ASSERT_EQ(i, tIt->first);
    AT_ASSERT_EQ(i, tIt->second);
    AT_ASSERT_EQ(i, copyIt->first);
    AT_ASSERT_EQ(i, copyIt->second);
  }
}

AT_TEST(shouldBeAbleToCopyAssign)
{
  const Tree t1{testTree()};
  Tree       t2{{1, 1}, {2, 2}};

  t2 = t1;

  AT_ASSERT_EQ(10, t2.size());
  AT_ASSERT_EQ(false, t2.empty());

  for (const Tree::value_type& keyValuePair : t1) {
    const Tree::key_type&    key{keyValuePair.first};
    const Tree::mapped_type& value{keyValuePair.second};

    const Tree::iterator it{t2.find(key)};
    AT_ASSERT_NE(t1.end(), it);
    AT_ASSERT_EQ(key, it->first);
    AT_ASSERT_EQ(value, it->second);
  }
}

AT_TEST(shouldBeAbleToAssignWithInitializerList)
{
  Tree t{testTree()};
  t = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {0, 0}};

  AT_ASSERT_EQ(6, t.size());
  const std::string expected{trimmed(R"(
|===========0 => 0
|
|
|=====1 => 1
|
|
2 => 2
|
|
|===========3 => 3
|
|
|=====4 => 4
|
|
|===========5 => 5)")};
  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldBeAbleToClear)
{
  Tree t{testTree()};
  t.clear();
  AT_ASSERT_EQ(0, t.size());
  AT_ASSERT_EQ(true, t.empty());
  AT_ASSERT_EQ(t.begin(), t.end());
  AT_ASSERT_EQ(t.rbegin(), t.rend());
}

AT_TEST(shouldBeAbleToInsertNewElement)
{
  Tree t{testTree()};
  const auto [it, wasInserted] = t.insert(11, 11);
  AT_ASSERT_EQ(true, wasInserted);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(11, it->first);
  AT_ASSERT_EQ(11, it->second);
  Tree::iterator iter{t.find(11)};
  AT_ASSERT_NE(t.end(), iter);
  AT_ASSERT_EQ(11, iter->first);
  AT_ASSERT_EQ(11, iter->second);
  AT_ASSERT_EQ(11, t.size());
  AT_ASSERT_EQ(false, t.empty());

  const std::string actual{at::toString(t)};
  const std::string expected{trimmed(R"(
|===========1 => 1
|
|
|=====2 => 2
|
|
|===========3 => 3
|
|
4 => 4
|
|
|=================5 => 5
|
|
|===========6 => 6
|
|
|=================7 => 7
|
|
|=====8 => 8
|
|
|=================9 => 9
|
|
|===========10 => 10
|
|
|=================11 => 11)")};
  AT_ASSERT_EQ(expected, actual);
}

AT_TEST(shouldDoNothingWhenInsertingAnElementThatAlreadyExists)
{
  Tree t{testTree()};
  const auto [it, wasInserted] = t.insert(1, 55);
  AT_ASSERT_EQ(false, wasInserted);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
  AT_ASSERT_EQ(10, t.size());
}

AT_TEST(shouldBeAbleToInsertAnIteratorRange)
{
  Tree t{};
  t.insert(1, 1);
  const std::vector<std::pair<const int, int>> v{{2, 2}, {3, 3}, {-5, -5}};
  t.insert(v.begin(), v.end());

  const std::string expected{trimmed(R"(
|===========-5 => -5
|
|
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3)")};
  const std::string actual{at::toString(t)};
  AT_ASSERT_EQ(expected, actual);
}

AT_TEST(shouldBeAbleToInsertWithInsertOrAssign)
{
  Tree                                  t{testTree()};
  const std::pair<Tree::iterator, bool> pair{t.insert_or_assign(0, 0)};
  AT_ASSERT_EQ(true, pair.second);
  AT_ASSERT_NE(t.end(), pair.first);
  AT_ASSERT_EQ(0, pair.first->first);
  AT_ASSERT_EQ(0, pair.first->second);

  const std::string expected{trimmed(R"(
|=================0 => 0
|
|
|===========1 => 1
|
|
|=====2 => 2
|
|
|===========3 => 3
|
|
4 => 4
|
|
|=================5 => 5
|
|
|===========6 => 6
|
|
|=================7 => 7
|
|
|=====8 => 8
|
|
|===========9 => 9
|
|
|=================10 => 10)")};
  const std::string actual{at::toString(t)};

  AT_ASSERT_EQ(expected, actual);
}

AT_TEST(shouldBeAbleToAssignWithInsertOrAssign)
{
  Tree t{testTree()};
  const auto [it, wasInserted] = t.insert_or_assign(5, 500);
  AT_ASSERT_EQ(false, wasInserted);
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(5, it->first);
  AT_ASSERT_EQ(500, it->second);
}

AT_TEST(shouldBeAbleToErase)
{
  Tree           t{testTree()};
  Tree::iterator it{t.erase(9)};
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
  AT_ASSERT_EQ(9, t.size());

  const std::string expected{trimmed(R"(
|===========1 => 1
|
|
|=====2 => 2
|
|
|===========3 => 3
|
|
4 => 4
|
|
|=================5 => 5
|
|
|===========6 => 6
|
|
|=================7 => 7
|
|
|=====8 => 8
|
|
|===========10 => 10)")};
  const std::string actual{at::toString(t)};

  AT_ASSERT_EQ(expected, actual);
}

AT_TEST(shouldBeAbleToEraseLastElement)
{
  Tree           t{testTree()};
  Tree::iterator it{t.erase(10)};
  AT_ASSERT_EQ(t.end(), it);
  AT_ASSERT_EQ(9, t.size());
  AT_ASSERT_EQ(false, t.empty());

  const std::string expected{trimmed(R"(
|===========1 => 1
|
|
|=====2 => 2
|
|
|===========3 => 3
|
|
4 => 4
|
|
|=================5 => 5
|
|
|===========6 => 6
|
|
|=================7 => 7
|
|
|=====8 => 8
|
|
|===========9 => 9)")};
  const std::string actual{at::toString(t)};

  AT_ASSERT_EQ(expected, actual);
}

AT_TEST(shouldDoNothingWhenErasingNonExistantKey)
{
  Tree                 t{testTree()};
  const Tree::iterator it{t.erase(0)};

  AT_ASSERT_EQ(t.end(), it);
  AT_ASSERT_EQ(10, t.size());
  AT_ASSERT_EQ(false, t.empty());

  const std::string expected{trimmed(R"(
|===========1 => 1
|
|
|=====2 => 2
|
|
|===========3 => 3
|
|
4 => 4
|
|
|=================5 => 5
|
|
|===========6 => 6
|
|
|=================7 => 7
|
|
|=====8 => 8
|
|
|===========9 => 9
|
|
|=================10 => 10)")};

  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldBeAbleToSwap)
{
  Tree t1{testTree()};
  Tree t2{{1, 1}, {2, 2}, {3, 3}};
  swap(t1, t2);

  AT_ASSERT_EQ(10, t2.size());
  AT_ASSERT_EQ(3, t1.size());

  for (int i{1}; i <= 10; ++i) {
    const Tree::iterator it{t2.find(i)};
    AT_ASSERT_NE(t2.end(), it);
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }

  for (int i{1}; i <= 3; ++i) {
    const Tree::iterator it{t1.find(i)};
    AT_ASSERT_NE(t1.end(), it);
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToFindByKey)
{
  const Tree                 t{testTree()};
  const Tree::const_iterator it{t.find(4)};
  AT_ASSERT_NE(t.end(), it);
  AT_ASSERT_EQ(4, it->first);
  AT_ASSERT_EQ(4, it->second);
}

AT_TEST(shouldNotBeAbleToFindNonExistantKey)
{
  const Tree                 t{testTree()};
  const Tree::const_iterator it{t.find(11)};
  AT_ASSERT_EQ(t.end(), it);
}

AT_TEST(shouldBeAbleToIterateForwardUsingIterators)
{
  Tree t{testTree()};

  int i{1};

  for (Tree::iterator it{t.begin()}, end{t.end()}; it != end; ++it, ++i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToPostfixIncrementIterateForwardUsingIterators)
{
  Tree t{testTree()};

  int i{1};

  for (Tree::iterator it{t.begin()}, end{t.end()}; it != end; it++, ++i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToIterateForwardUsingConstIterators)
{
  const Tree t{testTree()};

  int i{1};

  for (Tree::const_iterator it{t.begin()}, end{t.end()}; it != end; ++it, ++i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToPostfixIncrementIterateForwardUsingConstIterators)
{
  const Tree t{testTree()};

  int i{1};

  for (Tree::const_iterator it{t.begin()}, end{t.end()}; it != end; it++, ++i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToIterateBackwardsUsingIterators)
{
  Tree t{testTree()};

  int i{10};

  Tree::iterator it{t.end()};

  for (Tree::iterator begin{t.begin()}; it != begin; --it) {
    if (it != t.end()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      --i;
    }
  }

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
}

AT_TEST(shouldBeAbleToPostfixIterateBackwardsUsingIterators)
{
  Tree t{testTree()};

  int i{10};

  Tree::iterator it{t.end()};

  for (Tree::iterator begin{t.begin()}; it != begin; it--) {
    if (it != t.end()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      --i;
    }
  }

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
}

AT_TEST(shouldBeAbleToIterateBackwardsUsingConstIterators)
{
  const Tree t{testTree()};

  int i{10};

  Tree::const_iterator it{t.end()};

  for (Tree::const_iterator begin{t.begin()}; it != begin; --it) {
    if (it != t.end()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      --i;
    }
  }

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
}

AT_TEST(shouldBeAbleToPostfixIterateBackwardsUsingConstIterators)
{
  const Tree t{testTree()};

  int i{10};

  Tree::const_iterator it{t.end()};

  for (Tree::const_iterator begin{t.begin()}; it != begin; it--) {
    if (it != t.end()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      --i;
    }
  }

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
}

AT_TEST(shouldBeAbleToIterateUsingReverseIterators)
{
  Tree t{testTree()};

  int i{10};

  for (Tree::reverse_iterator it{t.rbegin()}, end{t.rend()}; it != end;
       ++it, --i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToPostfixIterateUsingReverseIterators)
{
  Tree t{testTree()};

  int i{10};

  for (Tree::reverse_iterator it{t.rbegin()}, end{t.rend()}; it != end;
       it++, --i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToIterateUsingConstReverseIterators)
{
  const Tree t{testTree()};

  int i{10};

  for (Tree::const_reverse_iterator it{t.rbegin()}, end{t.rend()}; it != end;
       ++it, --i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToPostfixIterateUsingConstReverseIterators)
{
  const Tree t{testTree()};

  int i{10};

  for (Tree::const_reverse_iterator it{t.rbegin()}, end{t.rend()}; it != end;
       it++, --i) {
    AT_ASSERT_EQ(i, it->first);
    AT_ASSERT_EQ(i, it->second);
  }
}

AT_TEST(shouldBeAbleToIterateBackwardsUsingReverseIterators)
{
  Tree t{testTree()};

  int i{1};

  Tree::reverse_iterator it{t.rend()};

  for (Tree::reverse_iterator begin{t.rbegin()}; it != begin; --it) {
    if (it != t.rend()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      ++i;
    }
  }

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
}

AT_TEST(shouldBeAbleToPostfixIterateBackwardsUsingReverseIterators)
{
  Tree t{testTree()};

  int i{1};

  Tree::reverse_iterator it{t.rend()};

  for (Tree::reverse_iterator begin{t.rbegin()}; it != begin; it--) {
    if (it != t.rend()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      ++i;
    }
  }

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
}

AT_TEST(shouldBeAbleToIterateBackwardsUsingConstReverseIterators)
{
  const Tree t{testTree()};

  int i{1};

  Tree::const_reverse_iterator it{t.rend()};

  for (Tree::const_reverse_iterator begin{t.rbegin()}; it != begin; --it) {
    if (it != t.rend()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      ++i;
    }
  }

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
}

AT_TEST(shouldBeAbleToPostfixIterateBackwardsUsingConstReverseIterators)
{
  const Tree t{testTree()};

  int i{1};

  Tree::const_reverse_iterator it{t.rend()};

  for (Tree::const_reverse_iterator begin{t.rbegin()}; it != begin; it--) {
    if (it != t.rend()) {
      AT_ASSERT_EQ(i, it->first);
      AT_ASSERT_EQ(i, it->second);
      ++i;
    }
  }

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
}

AT_TEST(shouldBeAbleToCompareIterators)
{
  Tree t{{1, 1}};

  AT_ASSERT_NE(t.end(), t.begin());
  AT_ASSERT_NE(t.cend(), t.cbegin());
  AT_ASSERT_NE(t.rend(), t.rbegin());
  AT_ASSERT_NE(t.crend(), t.crbegin());

  AT_ASSERT_EQ(t.end(), std::next(t.begin()));
  AT_ASSERT_EQ(t.cend(), std::next(t.cbegin()));
  AT_ASSERT_EQ(t.rend(), std::next(t.rbegin()));
  AT_ASSERT_EQ(t.crend(), std::next(t.crbegin()));

  AT_ASSERT_EQ(t.begin(), std::prev(t.end()));
  AT_ASSERT_EQ(t.cbegin(), std::prev(t.cend()));
  AT_ASSERT_EQ(t.rbegin(), std::prev(t.rend()));
  AT_ASSERT_EQ(t.crbegin(), std::prev(t.crend()));
}

AT_TEST(shouldBeAbleToPrefixIncrement)
{
  Tree t{testTree()};

  Tree::iterator it{t.begin()};
  std::advance(it, 3);
  Tree::iterator res{++it};

  AT_ASSERT_EQ(res, it);
  AT_ASSERT_EQ(5, it->first);
  AT_ASSERT_EQ(5, it->second);
}

AT_TEST(shouldBeAbleToPrefixIncrementBeginIterator)
{
  Tree t{testTree()};

  Tree::iterator it{t.begin()};
  Tree::iterator res{++it};

  AT_ASSERT_EQ(res, it);
  AT_ASSERT_EQ(2, it->first);
  AT_ASSERT_EQ(2, it->second);
}

AT_TEST(shouldThrowWhenPrefixIncrementingEndIterator)
{
  Tree t{testTree()};

  try {
    Tree::iterator it{t.end()};
    Tree::iterator res{++it};
    AT_ASSERT_EQ(false, true);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPrefixIncrementWithOneElement)
{
  Tree           t{{1, 2}};
  Tree::iterator it1{t.begin()}, it2{t.end()};
  Tree::iterator res1{++it1};

  try {
    Tree::iterator res2{++it2};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }

  AT_ASSERT_EQ(it1, res1);
  AT_ASSERT_EQ(t.end(), res1);
}

AT_TEST(shouldBeAbleToPrefixIncrementWithTwoElements)
{
  Tree           t{{1, 1}, {2, 2}};
  Tree::iterator it1{t.begin()};

  AT_ASSERT_EQ(1, it1->first);
  AT_ASSERT_EQ(1, it1->second);

  Tree::iterator it2{++it1};
  AT_ASSERT_EQ(it2, it1);
  AT_ASSERT_EQ(2, it2->first);
  AT_ASSERT_EQ(2, it2->second);

  Tree::iterator it3{++it1};
  AT_ASSERT_EQ(it3, it1);
  AT_ASSERT_EQ(t.end(), it3);

  try {
    Tree::iterator it4{++it1};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPostfixIncrement)
{
  Tree           t{testTree()};
  Tree::iterator it{t.begin()};

  std::advance(it, 3);

  Tree::iterator it2{it++};

  AT_ASSERT_EQ(4, it2->first);
  AT_ASSERT_EQ(4, it2->second);

  AT_ASSERT_EQ(5, it->first);
  AT_ASSERT_EQ(5, it->second);
}

AT_TEST(shouldBeAbleToPostfixIncrementBeginIterator)
{
  Tree           t{testTree()};
  Tree::iterator it{t.begin()};
  Tree::iterator it2{it++};

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);

  AT_ASSERT_EQ(2, it->first);
  AT_ASSERT_EQ(2, it->second);
}

AT_TEST(shouldThrowWhenPostfixIncrementingEndIterator)
{
  Tree t{testTree()};

  try {
    Tree::iterator it1{t.end()};
    Tree::iterator it2{it1++};
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPostfixIncrementWithASingleElement)
{
  Tree           t{{1, 2}};
  Tree::iterator it1{t.begin()};
  Tree::iterator it2{t.end()};

  Tree::iterator it3{it1++};

  try {
    Tree::iterator it4{it2++};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }

  AT_ASSERT_EQ(t.end(), it1);

  AT_ASSERT_EQ(1, it3->first);
  AT_ASSERT_EQ(2, it3->second);
}

AT_TEST(shouldBeAbleToPostfixIncrementIteratorsWithTwoElements)
{
  Tree t{{1, 2}, {2, 4}};

  Tree::iterator it1{t.begin()};
  Tree::iterator it2{std::next(t.begin())};
  Tree::iterator it3{t.end()};

  Tree::iterator it4{it1++};
  Tree::iterator it5{it2++};

  try {
    Tree::iterator it6{it3++};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }

  AT_ASSERT_EQ(2, it1->first);
  AT_ASSERT_EQ(4, it1->second);

  AT_ASSERT_EQ(t.end(), it2);

  AT_ASSERT_EQ(1, it4->first);
  AT_ASSERT_EQ(2, it4->second);

  AT_ASSERT_EQ(2, it5->first);
  AT_ASSERT_EQ(4, it5->second);
}

AT_TEST(shouldeBeAbleToPrefixDecrementIterators)
{
  Tree           t{testTree()};
  Tree::iterator it{t.end()};
  std::advance(it, -3);
  Tree::iterator it2{--it};

  AT_ASSERT_EQ(7, it->first);
  AT_ASSERT_EQ(7, it->second);
  AT_ASSERT_EQ(7, it2->first);
  AT_ASSERT_EQ(7, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementBeginIterator)
{
  Tree           t{testTree()};
  Tree::iterator it{t.begin()};
  Tree::iterator it2{--it};

  AT_ASSERT_EQ(t.end(), it);
  AT_ASSERT_EQ(t.end(), it2);
}

AT_TEST(shouldBeAbleToPrefixDecrementEndIterator)
{
  Tree           t{testTree()};
  Tree::iterator it{t.end()};
  ;
  Tree::iterator it2{--it};

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
  AT_ASSERT_EQ(10, it2->first);
  AT_ASSERT_EQ(10, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementWithOneElement)
{
  Tree           t{{1, 1}};
  Tree::iterator it1{t.begin()};
  Tree::iterator it2{t.end()};
  Tree::iterator it3{--it1};
  Tree::iterator it4{--it2};

  AT_ASSERT_EQ(t.end(), it1);

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);

  AT_ASSERT_EQ(t.end(), it3);

  AT_ASSERT_EQ(1, it4->first);
  AT_ASSERT_EQ(1, it4->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementWithTwoElements)
{
  Tree           t{{1, 1}, {2, 2}};
  Tree::iterator it1{t.begin()};
  Tree::iterator it2{std::next(t.begin())};
  Tree::iterator it3{t.end()};
  Tree::iterator it4{--it1};
  Tree::iterator it5{--it2};
  Tree::iterator it6{--it3};

  AT_ASSERT_EQ(t.end(), it1);

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);

  AT_ASSERT_EQ(2, it3->first);
  AT_ASSERT_EQ(2, it3->second);

  AT_ASSERT_EQ(t.end(), it4);

  AT_ASSERT_EQ(1, it5->first);
  AT_ASSERT_EQ(1, it5->second);

  AT_ASSERT_EQ(2, it6->first);
  AT_ASSERT_EQ(2, it6->second);
}

AT_TEST(shouldBeAbleToPostfixDecrement)
{
  Tree           t{testTree()};
  Tree::iterator it{t.begin()};
  std::advance(it, 7);
  Tree::iterator it2{it--};

  AT_ASSERT_EQ(7, it->first);
  AT_ASSERT_EQ(7, it->second);

  AT_ASSERT_EQ(8, it2->first);
  AT_ASSERT_EQ(8, it2->second);
}

AT_TEST(shouldReturnEndIteratorWhenPostfixDecrementingTheBeginIterator)
{
  Tree           t{testTree()};
  Tree::iterator it{t.begin()};
  Tree::iterator it2{it--};

  AT_ASSERT_EQ(t.end(), it);

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);
}

AT_TEST(shouldBeAbleToPostfixDecrementTheEndIterator)
{
  Tree           t{testTree()};
  Tree::iterator it1{t.end()};
  Tree::iterator it2{it1--};

  AT_ASSERT_EQ(10, it1->first);
  AT_ASSERT_EQ(10, it1->second);

  AT_ASSERT_EQ(t.end(), it2);
}

AT_TEST(shouldBeAbleToPostfixDecrementWithOneElement)
{
  Tree           t{{1, 1}};
  Tree::iterator it1{t.begin()};
  Tree::iterator it2{t.end()};
  Tree::iterator it3{it1--};
  Tree::iterator it4{it2--};

  AT_ASSERT_EQ(t.end(), it1);

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);

  AT_ASSERT_EQ(1, it3->first);
  AT_ASSERT_EQ(1, it3->second);

  AT_ASSERT_EQ(t.end(), it4);
}

AT_TEST(shouldBeAbleToPostfixDecrementWithTwoElements)
{
  Tree           t{{1, 1}, {2, 2}};
  Tree::iterator it1{t.begin()};
  Tree::iterator it2{std::next(t.begin())};
  Tree::iterator it3{t.end()};
  Tree::iterator it4{it1--};
  Tree::iterator it5{it2--};
  Tree::iterator it6{it3--};

  AT_ASSERT_EQ(t.end(), it1);

  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);

  AT_ASSERT_EQ(2, it3->first);
  AT_ASSERT_EQ(2, it3->second);

  AT_ASSERT_EQ(1, it4->first);
  AT_ASSERT_EQ(1, it4->second);

  AT_ASSERT_EQ(2, it5->first);
  AT_ASSERT_EQ(2, it5->second);

  AT_ASSERT_EQ(t.end(), it6);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseBeginIteratorForOneElement)
{
  Tree                   t{{1, 1}};
  Tree::reverse_iterator it{t.rbegin()};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  Tree::reverse_iterator it2{++it};
  Tree::reverse_iterator end{t.rend()};

  AT_ASSERT_EQ(end, it);
  AT_ASSERT_EQ(end, it2);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseBeginIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rbegin()};

  AT_ASSERT_EQ(2, it->first);
  AT_ASSERT_EQ(2, it->second);

  Tree::reverse_iterator it2{++it};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseBeginIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);

  Tree::reverse_iterator it2{++it};

  AT_ASSERT_EQ(9, it->first);
  AT_ASSERT_EQ(9, it->second);
  AT_ASSERT_EQ(9, it2->first);
  AT_ASSERT_EQ(9, it2->second);
}

AT_TEST(shouldThrowWhenPrefixDecrementingReverseBeginIteratorWithOneElement)
{
  Tree                   t{{1, 2}};
  Tree::reverse_iterator it{t.rbegin()};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(2, it->second);

  try {
    Tree::reverse_iterator it2{--it};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldThrowWhenPrefixDecrementingReverseBeginIteratorWithTwoElements)
{
  Tree                   t{{1, 2}, {2, 4}};
  Tree::reverse_iterator rit{t.rbegin()};

  AT_ASSERT_EQ(2, rit->first);
  AT_ASSERT_EQ(4, rit->second);

  try {
    Tree::reverse_iterator other{--rit};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(
  shouldThrowWhenPrefixDecrementingReverseBeginIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator rit{t.rbegin()};

  AT_ASSERT_EQ(10, rit->first);
  AT_ASSERT_EQ(10, rit->second);

  try {
    Tree::reverse_iterator other{--rit};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseEndIteratorWithOneElement)
{
  Tree                   t{{1, 2}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{++it};
  AT_ASSERT_EQ(t.rbegin(), it);
  AT_ASSERT_EQ(t.rbegin(), it2);
  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(2, it->second);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(2, it2->second);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseEndIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{++it};
  AT_ASSERT_EQ(t.rbegin(), it);
  AT_ASSERT_EQ(t.rbegin(), it2);
  AT_ASSERT_EQ(2, it->first);
  AT_ASSERT_EQ(2, it->second);
  AT_ASSERT_EQ(2, it2->first);
  AT_ASSERT_EQ(2, it2->second);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseEndIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{++it};
  AT_ASSERT_EQ(t.rbegin(), it);
  AT_ASSERT_EQ(t.rbegin(), it2);
  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);
  AT_ASSERT_EQ(10, it2->first);
  AT_ASSERT_EQ(10, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementReverseEndIteratorWithOneElement)
{
  Tree                   t{{1, 1}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{--it};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementReverseEndIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{--it};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementReverseEndIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{--it};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(1, it2->second);
}

AT_TEST(shouldBeAbleToPrefixIncrementReverseIteratorInTheMiddle)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};
  std::advance(it, 3);
  Tree::reverse_iterator it2{++it};

  AT_ASSERT_EQ(6, it->first);
  AT_ASSERT_EQ(6, it->second);
  AT_ASSERT_EQ(6, it2->first);
  AT_ASSERT_EQ(6, it2->second);
}

AT_TEST(shouldBeAbleToPrefixDecrementReverseIteratorInTheMiddle)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};
  std::advance(it, 5);
  Tree::reverse_iterator it2{--it};

  AT_ASSERT_EQ(6, it->first);
  AT_ASSERT_EQ(6, it->second);
  AT_ASSERT_EQ(6, it2->first);
  AT_ASSERT_EQ(6, it2->second);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseBeginIteratorWithOneElement)
{
  Tree                   t{{1, 2}};
  Tree::reverse_iterator it{t.rbegin()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(t.rend(), it);
  AT_ASSERT_EQ(1, it2->first);
  AT_ASSERT_EQ(2, it2->second);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseBeginIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rbegin()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  AT_ASSERT_EQ(2, it2->first);
  AT_ASSERT_EQ(2, it2->second);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseBeginIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(9, it->first);
  AT_ASSERT_EQ(9, it->second);

  AT_ASSERT_EQ(10, it2->first);
  AT_ASSERT_EQ(10, it2->second);
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseBeginIteratorWithOneElement)
{
  Tree                   t{{1, 1}};
  Tree::reverse_iterator it{t.rbegin()};

  try {
    Tree::reverse_iterator it2{it--};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseBeginIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rbegin()};

  try {
    Tree::reverse_iterator it2{it--};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseBeginIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};

  try {
    Tree::reverse_iterator it2{it--};
    AT_ASSERT_EQ(true, false);
  }
  catch (const std::runtime_error& ex) {
    AT_ASSERT_EQ(
      "AvlTree::iterator: prefix increment called on end iterator!"s,
      ex.what());
  }
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseEndIteratorWithOneElement)
{
  Tree                   t{{1, 1}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseEndIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(2, it->first);
  AT_ASSERT_EQ(2, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseEndIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(10, it->first);
  AT_ASSERT_EQ(10, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseEndIteratorWithOneElement)
{
  Tree                   t{{1, 1}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it--};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseEndIteratorWithTwoElements)
{
  Tree                   t{{1, 1}, {2, 2}};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it--};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseEndIteratorWithMultipleElements)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rend()};
  Tree::reverse_iterator it2{it--};

  AT_ASSERT_EQ(1, it->first);
  AT_ASSERT_EQ(1, it->second);

  AT_ASSERT_EQ(t.rend(), it2);
}

AT_TEST(shouldBeAbleToPostfixIncrementReverseIteratorInTheMiddle)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};
  std::advance(it, 4);
  Tree::reverse_iterator it2{it++};

  AT_ASSERT_EQ(5, it->first);
  AT_ASSERT_EQ(5, it->second);

  AT_ASSERT_EQ(6, it2->first);
  AT_ASSERT_EQ(6, it2->second);
}

AT_TEST(shouldBeAbleToPostfixDecrementReverseIteratorInTheMiddle)
{
  Tree                   t{testTree()};
  Tree::reverse_iterator it{t.rbegin()};
  std::advance(it, 7);
  Tree::reverse_iterator it2{it--};

  AT_ASSERT_EQ(4, it->first);
  AT_ASSERT_EQ(4, it->second);

  AT_ASSERT_EQ(3, it2->first);
  AT_ASSERT_EQ(3, it2->second);
}

AT_TEST(shouldBeAbleToPrintTree)
{
  Tree t{{1, 1}, {2, 2}, {3, 3}, {4, 4}};

  const std::string expected{trimmed(R"(
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3
|
|
|===========4 => 4)")};

  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldBeAbleToPrintEmptyTree)
{
  Tree t{};
  AT_ASSERT_EQ("Empty AvlTree", at::toString(t));
}

AT_TEST(shouldPerformRotationsCorrectly)
{
  at::AvlTree<char, char> t{};
  t.insert('M', 'M');
  t.insert('N', 'N');
  t.insert('O', 'O');
  // left rotate
  const std::string expectedLeftRotate{trimmed(R"(
|=====M => M
|
|
N => N
|
|
|=====O => O
  )")};
  AT_ASSERT_EQ(expectedLeftRotate, at::toString(t));

  t.insert('L', 'L');
  t.insert('K', 'K');
  // right rotate
  const std::string expectedRightRotate{trimmed(R"(
|===========K => K
|
|
|=====L => L
|
|
|===========M => M
|
|
N => N
|
|
|=====O => O
  )")};
  AT_ASSERT_EQ(expectedRightRotate, at::toString(t));

  t.insert('Q', 'Q');
  t.insert('P', 'P');
  // right left rotate
  const std::string expectedRightLeftRotate{trimmed(R"(
|===========K => K
|
|
|=====L => L
|
|
|===========M => M
|
|
N => N
|
|
|===========O => O
|
|
|=====P => P
|
|
|===========Q => Q
  )")};
  AT_ASSERT_EQ(expectedRightLeftRotate, at::toString(t));

  t.insert('H', 'H');
  t.insert('I', 'I');
  // left right
  const std::string expectedLeftRightRotate{trimmed(R"(
|=================H => H
|
|
|===========I => I
|
|
|=================K => K
|
|
|=====L => L
|
|
|===========M => M
|
|
N => N
|
|
|===========O => O
|
|
|=====P => P
|
|
|===========Q => Q
  )")};
  AT_ASSERT_EQ(expectedLeftRightRotate, at::toString(t));

  t.insert('A', 'A');
  // right
  const std::string expectedRightRotateEnd{trimmed(R"(
|=================A => A
|
|
|===========H => H
|
|
|=====I => I
|
|
|=================K => K
|
|
|===========L => L
|
|
|=================M => M
|
|
N => N
|
|
|===========O => O
|
|
|=====P => P
|
|
|===========Q => Q
  )")};
  AT_ASSERT_EQ(expectedRightRotateEnd, at::toString(t));
}

AT_TEST(shouldLeftRotate)
{
  Tree t{};
  t.insert(1, 1);
  t.insert(2, 2);
  t.insert(3, 3);

  const std::string expected{trimmed(R"(
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3)")};
  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldRightRotate)
{
  Tree t{};
  t.insert(3, 3);
  t.insert(2, 2);
  t.insert(1, 1);

  const std::string expected{trimmed(R"(
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3)")};
  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldLeftRightRotate)
{
  Tree t{};
  t.insert(3, 3);
  t.insert(1, 1);
  t.insert(2, 2);

  const std::string expected{trimmed(R"(
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3)")};
  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldRightLeftRotate)
{
  Tree t{};
  t.insert(1, 1);
  t.insert(3, 3);
  t.insert(2, 2);

  const std::string expected{trimmed(R"(
|=====1 => 1
|
|
2 => 2
|
|
|=====3 => 3)")};
  AT_ASSERT_EQ(expected, at::toString(t));
}

AT_TEST(shouldBeAbleToSustainRandomizedTest)
{
  std::mt19937_64                    urbg{createURBG()};
  Tree                               t{testTree()};
  std::uniform_int_distribution<int> dist{0, 4};
  std::uniform_int_distribution<int> valueDist{0, 10'000};

  const int toAdd{valueDist(urbg)};

  for (int i{0}; i < toAdd; ++i) {
    t.insert(valueDist(urbg), valueDist(urbg));
  }

  for (int round{0}; round < 1'000'000; ++round) {
    const int operation{dist(urbg)};

    switch (operation) {
    case 0:
      t.clear();
      break;
    case 1: {
      const int toInsert{valueDist(urbg)};
      t.insert(toInsert, toInsert);
      break;
    }
    case 2: {
      const int toInsert{valueDist(urbg)};
      t.insert_or_assign(toInsert, toInsert);
      break;
    }
    case 3: {
      const int v{valueDist(urbg)};
      t.erase(v);
      break;
    }
    case 4: {
      const int v{valueDist(urbg)};
      t.find(v);
      break;
    }
    }
  }
}

AT_TEST(shouldBeAbleToSustainRandomizedTestWithNoInitialValues)
{
  std::mt19937_64                    urbg{createURBG()};
  Tree                               t{};
  std::uniform_int_distribution<int> dist{0, 4};
  std::uniform_int_distribution<int> valueDist{0, 10'000};

  for (int round{0}; round < 1'000'000; ++round) {
    const int operation{dist(urbg)};

    switch (operation) {
    case 0:
      t.clear();
      break;
    case 1: {
      const int toInsert{valueDist(urbg)};
      t.insert(toInsert, toInsert);
      break;
    }
    case 2: {
      const int toInsert{valueDist(urbg)};
      t.insert_or_assign(toInsert, toInsert);
      break;
    }
    case 3: {
      const int v{valueDist(urbg)};
      t.erase(v);
      break;
    }
    case 4: {
      const int v{valueDist(urbg)};
      t.find(v);
      break;
    }
    }
  }
}

AT_TEST(shouldSustainRandomizedIteratorTest)
{
  std::mt19937_64 urbg{createURBG()};

  for (int i{0}; i < 1000; ++i) {
    std::uniform_int_distribution<int> dist{0, 1000};
    const int                          valuesToGenerate{dist(urbg)};

    Tree t{};

    for (int i{0}; i < valuesToGenerate; ++i) {
      const int value{dist(urbg)};
      t.insert(value, value);
    }

    for (int j{0}; j < 1000; ++j) {
      std::uniform_int_distribution<std::ptrdiff_t> offsetDist{
        0, static_cast<std::ptrdiff_t>(t.size())};
      Tree::iterator         it{t.begin()};
      Tree::reverse_iterator rit{t.rbegin()};
      const std::ptrdiff_t   offset{offsetDist(urbg)};
      std::advance(it, offset);
      std::advance(rit, offset);

      if (it != t.end()) {
        volatile std::pair<const int, int> p{*it};
        (void)p;
      }

      if (rit != t.rend()) {
        volatile std::pair<const int, int> p{*rit};
        (void)p;
      }
    }

    for (int j{0}; j < 1000; ++j) {
      std::uniform_int_distribution<std::ptrdiff_t> offsetDist{
        0, static_cast<std::ptrdiff_t>(t.size())};
      Tree::iterator         it{t.end()};
      Tree::reverse_iterator rit{t.rend()};
      const std::ptrdiff_t   offset{offsetDist(urbg)};
      std::advance(it, -offset);
      std::advance(rit, -offset);

      if (it != t.end()) {
        volatile std::pair<const int, int> p{*it};
        (void)p;
      }

      if (rit != t.rend()) {
        volatile std::pair<const int, int> p{*rit};
        (void)p;
      }
    }
  }
}

namespace at {
namespace {
[[nodiscard]] int runAllTests()
{
  std::string_view currentFunctionName{"NO FUNCTION"};
  std::cout << std::unitbuf;

  try {
    std::size_t testCase{1};

    for (const auto& [function, identifier] : testFunctions) {
      currentFunctionName = identifier;
      function();
      std::cout << "Test case " << testCase << " \"" << identifier
                << "\": SUCCESS.\n";
      ++testCase;
    }

    std::cout << ">>>> All tests ran successfully.\n";

    return EXIT_SUCCESS;
  }
  catch (const TestFailureException& exception) {
    std::cerr << '\n'
              << exception.what()
              << "\n\n\n>>>>>>>>>>> TEST FAILURE <<<<<<<<<\n\n";
    return EXIT_FAILURE;
  }
  catch (const std::runtime_error& runtimeError) {
    std::cerr << "Caught std::runtime_error: " << runtimeError.what()
              << "\nfunction: " << currentFunctionName << '\n'
              << "\n\n\n>>>>>>>>>>> TEST FAILURE <<<<<<<<<\n\n";
    return EXIT_FAILURE;
  }
  catch (const std::exception& exception) {
    std::cerr << "Caught std::exception: " << exception.what()
              << "\nfunction: " << currentFunctionName << '\n'
              << "\n\n\n>>>>>>>>>>> TEST FAILURE <<<<<<<<<\n\n";
    return EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "Unknown error caught!"
              << "\nfunction: " << currentFunctionName << '\n'
              << "\n\n\n>>>>>>>>>>> TEST FAILURE <<<<<<<<<\n\n";
    return EXIT_FAILURE;
  }
}
} // anonymous namespace
} // namespace at

int main()
{
#ifdef DEBUG_MODE
  _CrtMemState s1{}, s2{}, s3{};
  _CrtMemCheckpoint(&s1);
  std::cerr << "DEBUG_MODE\n";
#endif /* DEBUG_MODE */

  const int testsExitCode{at::runAllTests()};

#ifdef DEBUG_MODE
  _CrtMemCheckpoint(&s2);

  if (_CrtMemDifference(&s3, &s1, &s2)) {
    std::cerr << "\n\n>>>>>>>> Memory leaks detected <<<<<<<\n";
    _CrtMemDumpStatistics(&s3);
    return testsExitCode | EXIT_FAILURE;
  }
  else {
    std::cout << "No memory leaks detected.\n";
  }
#endif /* DEBUG_MODE */

  return testsExitCode;
}
