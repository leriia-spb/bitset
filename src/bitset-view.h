#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

template <typename U>
class bitset_view {
public:
  using word_type = uint64_t;
  using value_type = bool;
  using reference = bitset_reference<U>;
  using pointer = U*;
  using const_reference = bitset_reference<const U>;
  using iterator = bitset_iterator<U>;
  using const_iterator = bitset_iterator<const U>;
  using view = bitset_view<U>;
  using const_view = bitset_view<const U>;

  static constexpr std::size_t word_size = 64;
  static constexpr std::size_t npos = -1;

private:
  friend class bitset;
  friend bitset_view<std::remove_const<U>>;
  iterator left;
  iterator right;

  static word_type get_mask(std::size_t begin, std::size_t end) {
    return (~word_type(0) >> begin) & (~word_type(0) << (word_size - end));
  }

  template <typename Function>
  void bit_operator(const const_view& other, Function operation) const {
    auto this_iter = begin();
    auto other_iter = other.begin();

    pointer current_word;
    word_type other_word;

    if (this_iter._index != 0) {
      current_word = this_iter._word;
      word_type mask = get_mask(this_iter._index, (this_iter._word == end()._word) ? end()._index : word_size);
      other_word = other_iter.word(std::min(word_size - this_iter._index, std::size_t(end() - this_iter)));
      word_type new_word = (mask & operation(*current_word, other_word >> this_iter._index));
      *current_word = (new_word | (~mask & *current_word));

      other_iter += word_size - this_iter._index;
      this_iter += word_size - this_iter._index;
    }

    while (this_iter < end() && this_iter + word_size <= end()) {
      current_word = this_iter._word;
      other_word = other_iter.word(word_size);
      *current_word = operation(*current_word, other_word);

      this_iter += word_size;
      other_iter += word_size;
    }

    if (this_iter < end()) {
      current_word = this_iter._word;
      word_type mask = get_mask(0, end()._index);
      other_word = other_iter.word(end()._index - this_iter._index);
      word_type new_word = (mask & operation(*current_word, other_word >> this_iter._index));
      *current_word = (new_word | (~mask & *current_word));
    }
  }

  template <typename Function>
  void unary_operator(Function operation) const {
    auto this_iter = begin();
    pointer current_word;

    if (this_iter._index != 0) {
      current_word = this_iter._word;
      word_type mask = get_mask(this_iter._index, (this_iter._word == end()._word) ? end()._index : word_size);
      word_type new_word = (mask & operation(*current_word));
      *current_word = (new_word | (~mask & *current_word));

      this_iter += word_size - this_iter._index;
    }

    while (this_iter < end() && this_iter + word_size <= end()) {
      current_word = this_iter._word;
      *current_word = operation(*current_word);
      this_iter += word_size;
    }

    if (this_iter < end()) {
      current_word = this_iter._word;
      word_type mask = get_mask(0, end()._index);
      word_type new_word = (mask & operation(*current_word));
      *current_word = (new_word | (~mask & *current_word));
    }
  }

  word_type get_word(size_t num, size_t size) const {
    return (left + num * word_size).word(size);
  }

  template <typename V>
  bitset_view<V> subview_helper(std::size_t offset, std::size_t count) const {
    if (offset > size()) {
      return {end(), end()};
    }
    if (count <= size() - offset) {
      return {begin() + offset, begin() + offset + count};
    }
    return {begin() + offset, begin() + size()};
  }

  bool pattern_matching(std::size_t pattern) const {
    iterator iter = begin();
    while (iter < end()) {
      std::size_t count = len(iter);
      if (iter.word(count) != (pattern & get_mask(0, count))) {
        return false;
      }
      iter += count;
    }
    return true;
  }

  std::size_t len(const iterator& iter) const {
    return std::min(word_size, std::size_t(end() - iter));
  }

public:
  bitset_view() = default;
  bitset_view(const bitset_view&) = default;
  bitset_view& operator=(const bitset_view& other) = default;

  bitset_view(iterator left, iterator right)
      : left(left)
      , right(right) {}

  size_t size() const {
    return right - left;
  }

  size_t empty() const {
    return size() == 0;
  }

  iterator begin() const {
    return left;
  }

  iterator end() const {
    return right;
  }

  reference operator[](std::size_t index) const {
    auto iter = begin() + index;
    return {iter._index, iter._word};
  }

  bitset_view<U> operator&=(const const_view& other) const {
    bit_operator(other, [](word_type a, word_type b) { return a & b; });
    return *this;
  }

  bitset_view<U> operator|=(const const_view& other) const {
    bit_operator(other, [](word_type a, word_type b) { return a | b; });
    return *this;
  }

  bitset_view<U> operator^=(const const_view& other) const {
    bit_operator(other, [](word_type a, word_type b) { return a ^ b; });
    return *this;
  }

  bitset_view<U> flip() const {
    unary_operator([](word_type b) { return ~b; });
    return *this;
  }

  bitset_view<U> set() const {
    unary_operator([](word_type /*b*/) { return ~word_type(0); });
    return *this;
  }

  bitset_view<U> reset() const {
    unary_operator([](word_type /*b*/) { return 0; });
    return *this;
  }

  bool all() const {
    return pattern_matching(~word_type(0));
  }

  bool any() const {
    return !pattern_matching(0);
  }

  std::size_t count() const {
    std::size_t ans = 0;
    iterator iter = begin();
    while (iter < end()) {
      std::size_t count = len(iter);
      ans += std::popcount(iter.word(count));
      iter += count;
    }
    return ans;
  }

  friend bool operator==(const bitset_view& left, const bitset_view& right) {
    if (left.size() != right.size()) {
      return false;
    }
    auto this_iter = left.begin();
    auto other_iter = right.begin();

    pointer current_word;
    word_type other_word;

    if (this_iter._index != 0) {
      current_word = this_iter._word;
      word_type mask =
          get_mask(this_iter._index, (this_iter._word == left.end()._word) ? left.end()._index : word_size);
      other_word = other_iter.word(std::min(word_size - this_iter._index, std::size_t(left.end() - this_iter)));
      if ((*current_word & mask) != (mask & (other_word >> this_iter._index))) {
        return false;
      }
      other_iter += word_size - this_iter._index;
      this_iter += word_size - this_iter._index;
    }

    while (this_iter < left.end() && this_iter + word_size <= left.end()) {
      current_word = this_iter._word;
      other_word = other_iter.word(word_size);
      if (*current_word != other_word) {
        return false;
      }

      this_iter += word_size;
      other_iter += word_size;
    }

    if (this_iter < left.end()) {
      current_word = this_iter._word;
      word_type mask = get_mask(0, left.end()._index);
      other_word = other_iter.word(left.end()._index - this_iter._index);
      if ((*current_word & mask) != (mask & other_word)) {
        return false;
      }
    }
    return true;
  }

  operator const_view() const {
    return const_view(begin(), end());
  }

  friend bool operator!=(const bitset_view& left, const bitset_view& right) {
    return !(left == right);
  }

  void swap(bitset_view& other) noexcept {
    std::swap(left, other.left);
    std::swap(right, other.right);
  }

  view subview(std::size_t offset = 0, std::size_t count = npos) const {
    return subview_helper<U>(offset, count);
  }
};

template <typename T>
std::string to_string(const bitset_view<T>& bs) {
  std::string out;
  for (std::size_t i = 0; i < bs.size(); ++i) {
    out += (bs[i]) ? '1' : '0';
  }
  return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const bitset_view<T>& bs) {
  for (std::size_t i = 0; i < bs.size(); ++i) {
    out << bs[i];
  }
  return out;
}
