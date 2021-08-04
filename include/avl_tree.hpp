#ifndef INCG_AVL_TREE_HPP
#define INCG_AVL_TREE_HPP
#ifdef DEBUG_MODE
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#define DBG_NEW new (_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DBG_NEW new
#endif

#include <cmath>
#include <cstdint>

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <locale>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace at {
template<typename Key, typename T, typename Compare = std::less<Key>>
class AvlTree {
public:
  using this_type       = AvlTree;
  using key_type        = Key;
  using mapped_type     = T;
  using value_type      = std::pair<const key_type, mapped_type>;
  using size_type       = std::size_t;
  using ssize_type      = std::make_signed_t<size_type>;
  using difference_type = std::ptrdiff_t;
  using key_compare     = Compare;
  using reference       = value_type&;
  using const_reference = const value_type&;
  using pointer         = value_type*;
  using const_pointer   = const value_type*;

  class const_iterator;

private:
  struct Node {
    Node(const key_type& key, const mapped_type& value)
      : keyValuePair{key, value}
      , parent{nullptr}
      , left{nullptr}
      , right{nullptr}
      , height{1}
    {
    }

    const key_type& key() const
    {
      return keyValuePair.first;
    }

    mapped_type& value()
    {
      return keyValuePair.second;
    }

    const mapped_type& value() const
    {
      return const_cast<Node*>(this)->value();
    }

    value_type keyValuePair;
    Node*      parent;
    Node*      left;
    Node*      right;
    ssize_type height;
  };

public:
  friend std::ostream& operator<<(std::ostream& os, const const_iterator& it);

  class iterator {
  public:
    using difference_type   = typename AvlTree::difference_type;
    using value_type        = std::remove_cv_t<typename AvlTree::value_type>;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept  = std::bidirectional_iterator_tag; // C++20

    friend class AvlTree;

    friend bool operator==(const iterator& lhs, const iterator& rhs)
    {
      return lhs.m_node == rhs.m_node;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs)
    {
      return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const iterator& it)
    {
      return os << "AvlTree::iterator{" << it.m_node << '}';
    }

    iterator(Node* root, Node* node) : m_node{node}, m_root{root}
    {
    }

    reference operator*() const
    {
      return m_node->keyValuePair;
    }

    pointer operator->() const
    {
      return &m_node->keyValuePair;
    }

    iterator& operator++() // prefix increment
    {
      if (m_node == nullptr) {
        throw std::runtime_error{
          "AvlTree::iterator: prefix increment called on end iterator!"};
      }

      Node* previous{m_node};

      increment(m_node);

      if (m_node == previous) {
        m_node = nullptr;
      }

      return *this;
    }

    iterator operator++(int) // postfix increment
    {
      iterator it{*this};
      ++(*this);
      return it;
    }

    iterator& operator--() // prefix decrement
    {
      // Decrement end.
      if (m_node == nullptr) {
        Node* node{m_root};

        while (node->right != nullptr) {
          node = node->right;
        }

        m_node = node;
        return *this;
      }

      Node* previous{m_node};

      decrement(m_node);

      if (m_node == previous) {
        m_node = nullptr;
      }

      return *this;
    }

    iterator operator--(int) // postfix decrement
    {
      iterator it{*this};
      --(*this);
      return it;
    }

  private:
    static void increment(Node*& node)
    {
      if (node->right != nullptr) {
        node = node->right;

        while (node->left != nullptr) {
          node = node->left;
        }
      }
      else {
        Node* parent{node->parent};

        while (parent != nullptr && node == parent->right) {
          node   = parent;
          parent = node->parent;
        }

        node = parent;
      }
    }

    static void decrement(Node*& node)
    {
      if (node->left != nullptr) {
        node = node->left;

        while (node->right != nullptr) {
          node = node->right;
        }
      }
      else {
        Node* parent{node->parent};

        while (parent != nullptr && node == parent->left) {
          node   = parent;
          parent = node->parent;
        }

        node = parent;
      }
    }

    Node* m_node;
    Node* m_root;
  };

  class const_iterator {
  public:
    using difference_type   = typename AvlTree::difference_type;
    using value_type        = std::remove_cv_t<typename AvlTree::value_type>;
    using pointer           = const value_type*;
    using reference         = const value_type&;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept  = std::bidirectional_iterator_tag; // C++20

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.m_it == rhs.m_it;
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
    {
      return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const const_iterator& it)
    {
      return os << "AvlTree::const_iterator{"
                << reinterpret_cast<Node* const&>(it.m_it) << '}';
    }

    /* IMPLICIT */ const_iterator(iterator it) : m_it{it}
    {
    }

    reference operator*() const
    {
      return *m_it;
    }

    pointer operator->() const
    {
      return m_it.operator->();
    }

    const_iterator& operator++() // prefix increment
    {
      ++m_it;
      return *this;
    }

    const_iterator operator++(int) // postfix increment
    {
      const_iterator it{*this};
      ++(*this);
      return it;
    }

    const_iterator& operator--() // prefix decrement
    {
      --m_it;
      return *this;
    }

    const_iterator operator--(int) // postfix decrement
    {
      const_iterator it{*this};
      --(*this);
      return it;
    }

  private:
    iterator m_it;
  };

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

#define AT_CMPKEY(key1, key2) key_compare{}((key1), (key2))

  class value_compare {
  public:
    bool operator()(const_reference lhs, const_reference rhs) const
    {
      return AT_CMPKEY(lhs.first, rhs.first);
    }
  };

  friend std::ostream& operator<<(std::ostream& os, const this_type& tree)
  {
    if (tree.empty()) {
      return os << "Empty AvlTree";
    }

    std::ostringstream outputStringStream{};
    outputStringStream.imbue(std::locale::classic());
    printTree(tree.m_root, 0, outputStringStream);
    std::string string{outputStringStream.str()};

    for (int i{0}; i < 5; ++i) {
      string.pop_back();
    }

    os << string;
    return os;
  }

  AvlTree() : m_root{nullptr}, m_nodeCount{0}
  {
  }

  template<typename InputIterator>
  AvlTree(InputIterator first, InputIterator last) : AvlTree{}
  {
    while (first != last) {
      insert(*first);
      ++first;
    }
  }

  AvlTree(std::initializer_list<value_type> initList)
    : AvlTree{initList.begin(), initList.end()}
  {
  }

  AvlTree(const this_type& other) : AvlTree{}
  {
    copy(other);
  }

  this_type& operator=(const this_type& other)
  {
    if (this == &other) {
      return *this;
    }

    clear();
    copy(other);
    return *this;
  }

  this_type& operator=(std::initializer_list<value_type> initList)
  {
    clear();

    for (const value_type& keyValuePair : initList) {
      insert(keyValuePair);
    }

    return *this;
  }

  ~AvlTree()
  {
    destroyTree(m_root);
  }

  size_type size() const
  {
    return m_nodeCount;
  }

  [[nodiscard]] bool empty() const
  {
    return size() == 0;
  }

  iterator begin()
  {
    if (empty()) {
      return end();
    }

    Node** smallest{leftmostNode(&m_root)};
    return iterator{m_root, *smallest};
  }

  const_iterator begin() const
  {
    return const_iterator{const_cast<this_type*>(this)->begin()};
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  iterator end()
  {
    return iterator{m_root, nullptr};
  }

  const_iterator end() const
  {
    return const_iterator{const_cast<this_type*>(this)->end()};
  }

  const_iterator cend() const
  {
    return end();
  }

  reverse_iterator rbegin()
  {
    return reverse_iterator{end()};
  }

  const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator{const_cast<this_type*>(this)->rbegin()};
  }

  const_reverse_iterator crbegin() const
  {
    return rbegin();
  }

  reverse_iterator rend()
  {
    return reverse_iterator{begin()};
  }

  const_reverse_iterator rend() const
  {
    return const_cast<this_type*>(this)->rend();
  }

  const_reverse_iterator crend() const
  {
    return rend();
  }

  void clear()
  {
    destroyTree(m_root);
    m_root      = nullptr;
    m_nodeCount = 0;
  }

  std::pair<iterator, bool> insert(
    const key_type&    key,
    const mapped_type& value)
  {
    bool           didInsert{false};
    Node*          nodeInserted{nullptr};
    constexpr bool dontReplace{false};
    m_root
      = insertImpl(key, value, m_root, &nodeInserted, &didInsert, dontReplace);
    m_root->parent = nullptr;

    if (didInsert) {
      ++m_nodeCount;
    }

    return {iterator{m_root, nodeInserted}, didInsert};
  }

  std::pair<iterator, bool> insert(const_reference keyValuePair)
  {
    return insert(keyValuePair.first, keyValuePair.second);
  }

  template<typename InputIterator>
  void insert(InputIterator first, InputIterator last)
  {
    while (first != last) {
      insert(*first);
      ++first;
    }
  }

  void insert(std::initializer_list<value_type> initList)
  {
    insert(initList.begin(), initList.end());
  }

  std::pair<iterator, bool> insert_or_assign(
    const key_type&    key,
    const mapped_type& value)
  {
    bool           didInsert{false};
    Node*          nodeInserted{nullptr};
    constexpr bool doReplace{true};
    m_root
      = insertImpl(key, value, m_root, &nodeInserted, &didInsert, doReplace);
    m_root->parent = nullptr;

    if (didInsert) {
      ++m_nodeCount;
    }

    return {iterator{m_root, nodeInserted}, didInsert};
  }

  std::pair<iterator, bool> insert_or_assign(const value_type& keyValuePair)
  {
    return insert_or_assign(keyValuePair.first, keyValuePair.second);
  }

  iterator erase(const key_type& key)
  {
    if (empty()) {
      return end();
    }

    iterator next{end()};
    m_root = eraseImpl(key, m_root, &next);

    if (m_root != nullptr) {
      m_root->parent = nullptr;
    }

    next.m_root = m_root;
    return next;
  }

  void swap(this_type& other) noexcept
  {
    std::swap(m_root, other.m_root);
    std::swap(m_nodeCount, other.m_nodeCount);
  }

  iterator find(const key_type& key)
  {
    Node* node{m_root};

    while (node != nullptr) {
      if (AT_CMPKEY(key, node->key())) { // If key < node.key -> go left.
        node = node->left;
      }
      else if (AT_CMPKEY(node->key(), key)) { // If key > node.key -> go right.
        node = node->right;
      }
      else { // Found it.
        return iterator{m_root, node};
      }
    }

    return end(); // Didn't find it.
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator{const_cast<this_type*>(this)->find(key)};
  }

private:
  static void printTree(Node* node, int depth, std::ostream& os)
  {
    if (node == nullptr) {
      return;
    }

    printTree(node->left, depth + 6, os);

    for (int i{0}; i < depth; ++i) {
      if (i == 0) {
        os << '|';
      }
      else {
        os << '=';
      }
    }

    os << node->key() << " => " << node->value();
    os << "\n|\n|\n";

    printTree(node->right, depth + 6, os);
  }

  void copy(const this_type& other)
  {
    for (const value_type& keyValuePair : other) {
      insert(keyValuePair);
    }
  }

  void destroyTree(Node* node)
  {
    if (node == nullptr) {
      return;
    }

    destroyTree(node->right);
    destroyTree(node->left);

    delete node;
  }

  static ssize_type heightOf(Node* node)
  {
    if (node == nullptr) {
      return 0;
    }

    return node->height;
  }

  ssize_type calculateBalanceFactor(Node* node)
  {
    if (node == nullptr) {
      return 0;
    }

    const ssize_type rightHeight{heightOf(node->right)};
    const ssize_type leftHeight{heightOf(node->left)};

    return leftHeight - rightHeight;
  }

  Node* rotateRight(Node* node)
  {
    Node* left{node->left};
    Node* leftRight{left->right};
    left->right = node;

    if (left->right != nullptr) {
      left->right->parent = left;
    }

    node->left = leftRight;

    if (node->left != nullptr) {
      node->left->parent = node;
    }

    node->height = std::max(heightOf(node->right), heightOf(node->left)) + 1;
    left->height = std::max(heightOf(left->right), heightOf(left->left)) + 1;

    return left;
  }

  Node* rotateLeft(Node* node)
  {
    Node* right{node->right};
    Node* rightLeft{right->left};

    right->left = node;

    if (right->left != nullptr) {
      right->left->parent = right;
    }

    node->right = rightLeft;

    if (node->right != nullptr) {
      node->right->parent = node;
    }

    node->height  = std::max(heightOf(node->right), heightOf(node->left)) + 1;
    right->height = std::max(heightOf(right->right), heightOf(right->left)) + 1;

    return right;
  }

  Node** leftmostNode(Node** node)
  {
    Node** result{node};

    while ((*result)->left != nullptr) {
      result = &(*result)->left;
    }

    return result;
  }

  Node* detachLeftmostNode(Node*& node)
  {
    Node** leftmost{leftmostNode(&node)};
    return detachNode(*leftmost);
  }

  Node* detachNode(Node*& node)
  {
    if (node->left == nullptr && node->right == nullptr) {
      // No children
      Node* detached{node};
      node = nullptr;
      return detached;
    }

    if (node->left != nullptr && node->right != nullptr) {
      // Two children
      Node* replacement{detachLeftmostNode(node->right)};

      replacement->left   = node->left;
      replacement->right  = node->right;
      replacement->parent = node->parent;

      Node* detached{node};
      node = replacement;
      return detached;
    }

    // One child
    Node* child{node->left == nullptr ? node->right : node->left};
    Node* detached{node};
    child->parent = node->parent;
    node          = child;
    return detached;
  }

  Node* balance(Node* node)
  {
    const ssize_type balanceFactor{calculateBalanceFactor(node)};

    // Right Right => rotate left
    // left height (0) - right height (2) == -2
    // right child has a left height of 0 and a right height of 1 -> -1 (because
    // 0 - 1 == -1)
    if (balanceFactor == -2 && calculateBalanceFactor(node->right) == -1) {
      return rotateLeft(node);
    }

    // Left Left => rotate right
    // left height (2) - right height (0) == 2
    // left child has a left height of 1 and a right height of 0 -> 1 (because 1
    // - 0 == 1)
    if (balanceFactor == 2 && calculateBalanceFactor(node->left) == 1) {
      return rotateRight(node);
    }

    // Right Left => rotate right left
    // left height (0) - right height (2) == -2
    // right child has a left height of 1 and a right height of 0 -> 1 (because
    // 1 - 0 == 1)
    if (balanceFactor == -2 && calculateBalanceFactor(node->right) == 1) {
      node->right         = rotateRight(node->right);
      node->right->parent = node;

      return rotateLeft(node);
    }

    // Left Right => rotate left right
    // left height (2) - right height (0) == 2
    // left child has a left height of 0 and right height of 1 -> -1 (because 0
    // - 1 == -1)
    if (balanceFactor == 2 && calculateBalanceFactor(node->left) == -1) {
      node->left         = rotateLeft(node->left);
      node->left->parent = node;

      return rotateRight(node);
    }

    return node;
  }

  Node* eraseImpl(const key_type& key, Node*& node, iterator* next)
  {
    if (node == nullptr) {
      return nullptr;
    }

    if (AT_CMPKEY(node->key(), key)) { // If key > node.key -> go right
      node->right = eraseImpl(key, node->right, next);

      if (node->right != nullptr) {
        node->right->parent = node;
      }
    }
    else if (AT_CMPKEY(key, node->key())) { // If key < node.key -> go left
      node->left = eraseImpl(key, node->left, next);

      if (node->left != nullptr) {
        node->left->parent = node;
      }
    }
    else { // Found it.
      *next = iterator{m_root, node};
      ++(*next);

      Node* nodeToDelete{detachNode(node)};
      delete nodeToDelete;

      --m_nodeCount;
    }

    if (node == nullptr) {
      return nullptr;
    }

    node->height = std::max(heightOf(node->right), heightOf(node->left)) + 1;

    return balance(node);
  }

  Node* insertImpl(
    const key_type&    key,
    const mapped_type& value,
    Node*              node,
    Node**             insertedOrPreventedInsertion,
    bool*              didInsert,
    bool               shouldReplace)
  {
    if (node == nullptr) { // Leaf node found -> replace it.
      Node* nodeCreated{nullptr};

      try {
        nodeCreated                   = DBG_NEW Node{key, value};
        *insertedOrPreventedInsertion = nodeCreated;
        *didInsert                    = true;
        return nodeCreated;
      }
      catch (...) {
        delete nodeCreated;
        throw;
      }
    }

    if (AT_CMPKEY(node->key(), key)) { // If key > node.key -> go right
      node->right = insertImpl(
        key,
        value,
        node->right,
        insertedOrPreventedInsertion,
        didInsert,
        shouldReplace);
      node->right->parent = node;
    }
    else if (AT_CMPKEY(key, node->key())) { // If key < node.key -> go left
      node->left = insertImpl(
        key,
        value,
        node->left,
        insertedOrPreventedInsertion,
        didInsert,
        shouldReplace);
      node->left->parent = node;
    }
    else { // It's already there.
      *insertedOrPreventedInsertion = node;
      *didInsert                    = false;

      if (shouldReplace) {
        node->keyValuePair.second = value;
      }

      return node;
    }

    node->height = std::max(heightOf(node->right), heightOf(node->left)) + 1;

    return balance(node);
  }

  Node*     m_root;
  size_type m_nodeCount;
};

#undef AT_COMP_KEY

template<typename Key, typename T, typename Compare = std::less<Key>>
void swap(AvlTree<Key, T, Compare>& lhs, AvlTree<Key, T, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace at
#endif // INCG_AVL_TREE_HPP
