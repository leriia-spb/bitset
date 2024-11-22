#pragma once
#include <bitset-iterator.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

template <typename T>
class bitset_reference {
public:
  using value_type = bool;
  using word_type = T;
  static constexpr std::size_t word_size = 64;

private:
  size_t index;
  word_type* word;

  template <typename U>
  friend class bitset_iterator;
  friend class bitset;
  template <typename U>
  friend class bitset_view;
  friend class bitset_reference<std::remove_const_t<T>>;

  bitset_reference(std::size_t index, word_type* word)
      : index(index)
      , word(word) {}

  word_type mask() const {
    return (word_type(1) << (word_size - index - 1));
  }

public:
  bitset_reference() = delete;

  operator bitset_reference<const T>() const {
    return {index, word};
  }

  bitset_reference& operator=(const value_type bit) {
    if (bit) {
      *word |= mask();
    } else {
      *word &= ~mask();
    }
    return *this;
  }

  ~bitset_reference() = default;

  operator bool() const {
    return (*word & mask()) != 0;
  }

  friend bool operator==(const bitset_reference& left, const bitset_reference& right) {
    return bool(left) == bool(right);
  }

  friend bool operator!=(const bitset_reference& left, const bitset_reference& right) {
    return !(left == right);
  }

  bitset_reference& flip()
    requires (!std::is_const_v<T>)
  {
    *word ^= mask();
    return *this;
  }
};
