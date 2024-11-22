#pragma once
#include "bitset-reference.h"
#include "bitset.h"

#include <cstddef>
#include <cstdint>
#include <iterator>

template <class T>
class bitset_iterator {
public:
  using value_type = bool;
  using difference_type = std::ptrdiff_t;
  using reference = bitset_reference<T>;
  using iterator_category = std::random_access_iterator_tag;
  using word_type = uint64_t;
  static constexpr std::size_t word_size = 64;

private:
  friend class bitset;
  template <typename U>
  friend class bitset_view;
  friend bitset_iterator<std::remove_const_t<T>>;

  T* _word;
  size_t _index;

  word_type word(size_t size) const {
    word_type ans = 0;

    ans |= (*_word << _index);

    if (size > word_size - _index) {
      ans |= (*(_word + 1) >> (word_size - _index));
    }

    if (size < word_size) {
      ans &= ~((1ULL << (word_size - size)) - 1);
    }

    return ans;
  }

  bitset_iterator(T* word, size_t index) noexcept
      : _word{word}
      , _index{index} {}

public:
  bitset_iterator() = default;

  bitset_iterator(const bitset_iterator& o) = default;

  bitset_iterator& operator=(const bitset_iterator& o) = default;

  operator bitset_iterator<const T>() const {
    return {_word, _index};
  }

  reference operator*() const {
    return {_index, _word};
  }

  reference operator[](difference_type index) const {
    bitset_iterator tmp = *this;
    tmp += index;
    return *tmp;
  }

  bitset_iterator& operator++() {
    ++_index;
    if (_index == word_size) {
      _index = 0;
      ++_word;
    }
    return *this;
  }

  bitset_iterator operator++(int) {
    bitset_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  bitset_iterator& operator--() {
    if (_index == 0) {
      _index = word_size - 1;
      --_word;
    } else {
      --_index;
    }
    return *this;
  }

  bitset_iterator operator--(int) {
    bitset_iterator tmp = *this;
    --(*this);
    return tmp;
  }

  bitset_iterator& operator+=(difference_type n) {
    const auto new_index = static_cast<difference_type>(_index) + n;
    _word += new_index / difference_type(word_size) - ((new_index < 0) ? (new_index % word_size != 0) : 0);
    _index = (new_index % word_size + word_size) % word_size;
    return *this;
  }

  bitset_iterator& operator-=(difference_type n) {
    *this += -n;
    return *this;
  }

  friend bitset_iterator operator-(const bitset_iterator& iter, difference_type n) {
    bitset_iterator tmp = iter;
    tmp -= n;
    return tmp;
  }

  friend difference_type operator-(const bitset_iterator& left, const bitset_iterator& right) {
    const auto word_diff = left._word - right._word;
    const auto index_diff = left._index - right._index;
    return word_diff * word_size + index_diff;
  }

  friend bitset_iterator operator+(const bitset_iterator& iter, difference_type n) {
    bitset_iterator tmp = iter;
    tmp += n;
    return tmp;
  }

  friend bitset_iterator operator+(difference_type n, const bitset_iterator& iter) {
    bitset_iterator tmp = iter;
    tmp += n;
    return tmp;
  }

  friend bool operator<(const bitset_iterator& left, const bitset_iterator& right) {
    return left - right < 0;
  }

  friend bool operator>(const bitset_iterator& left, const bitset_iterator& right) {
    return left - right > 0;
  }

  friend bool operator<=(const bitset_iterator& left, const bitset_iterator& right) {
    return left - right <= 0;
  }

  friend bool operator>=(const bitset_iterator& left, const bitset_iterator& right) {
    return left - right >= 0;
  }

  friend bool operator==(const bitset_iterator& left, const bitset_iterator& right) {
    return left._word == right._word && left._index == right._index;
  }

  friend bool operator!=(const bitset_iterator& left, const bitset_iterator& right) {
    return !(left == right);
  }

  void swap(bitset_iterator& other) noexcept {
    std::swap(_word, other._word);
    std::swap(_index, other._index);
  }
};
