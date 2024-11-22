#include "bitset.h"

bitset::bitset()
    : _data{nullptr}
    , _size{0}
    , _capacity{0} {}

bitset::bitset(std::size_t size, bool value)
    : _size(size)
    , _capacity((size + word_size - 1) / word_size) {
  if (size != 0) {
    _data = static_cast<word_type*>(operator new(_capacity * sizeof(word_type)));
    std::fill_n(_data, _capacity, ((value) ? ~word_type(0) : 0));
  } else {
    _data = nullptr;
  }
}

bitset::bitset(const bitset& other)
    : bitset(other.begin(), other.end()) {}

bitset::bitset(std::string_view str)
    : _size(str.length())
    , _capacity((_size + word_size - 1) / word_size) {
  if (size() != 0) {
    _data = static_cast<word_type*>(operator new(_capacity * sizeof(word_type)));
    for (std::size_t i = 0; i < _capacity; ++i) {
      word_type word = 0;
      for (std::size_t j = 0; j < word_size && i * word_size + j < size(); ++j) {
        if (str[i * word_size + j] == '1') {
          word |= (word_type(1) << (word_size - 1 - j));
        }
      }
      _data[i] = word;
    }
  } else {
    _data = nullptr;
  }
}

bitset::bitset(const const_view& other)
    : bitset(other.begin(), other.end()) {}

bitset::bitset(bitset::const_iterator first, bitset::const_iterator last)
    : bitset() {
  bitset copy = bitset(last - first, false);
  copy |= const_view(first, last);
  swap(copy);
}

bitset& bitset::operator=(const bitset& other) & {
  if (this == &other) {
    return *this;
  }
  bitset copy(other);
  swap(copy);
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset copy(str);
  swap(copy);
  return *this;
}

bitset& bitset::operator=(const const_view& other) & {
  bitset copy(other);
  swap(copy);
  return *this;
}

bitset::~bitset() {
  if (_data != nullptr) {
    std::reverse_iterator<word_type*> iter(_data + _capacity);
    operator delete(_data);
  }
}

void bitset::swap(bitset& other) noexcept {
  std::swap(_data, other._data);
  std::swap(_size, other._size);
  std::swap(_capacity, other._capacity);
}

std::size_t bitset::size() const {
  return _size;
}

bool bitset::empty() const {
  return _size == 0;
}

bitset::reference bitset::operator[](std::size_t index) {
  return {
      index % word_size,
      _data + index / word_size,
  };
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return {
      index % word_size,
      _data + index / word_size,
  };
}

bitset::iterator bitset::begin() {
  return {_data, 0};
}

bitset::const_iterator bitset::begin() const {
  return {_data, 0};
}

bitset::iterator bitset::end() {
  return {_data + (_size / word_size), _size % word_size};
}

bitset::const_iterator bitset::end() const {
  return {_data + (_size / word_size), _size % word_size};
}

bitset& bitset::operator&=(const const_view& other) & {
  view(*this) &= other;
  return *this;
}

bitset& bitset::operator|=(const const_view& other) & {
  view(*this) |= other;
  return *this;
}

bitset& bitset::operator^=(const const_view& other) & {
  view(*this) ^= other;
  return *this;
}

bitset& bitset::operator<<=(std::size_t count) & {
  if (count + size() < _capacity) {
    _size += count;
    return *this;
  }
  bitset ans(size() + count, false);
  ans.subview(0, size()) |= *this;
  swap(ans);
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  _size = (count >= size()) ? 0 : size() - count;
  return *this;
}

void bitset::flip() & {
  view(*this).flip();
}

bitset& bitset::set() & {
  view(*this).set();
  return *this;
}

bitset& bitset::reset() & {
  view(*this).reset();
  return *this;
}

bool bitset::all() const {
  return const_view(begin(), end()).all();
}

bool bitset::any() const {
  return const_view(begin(), end()).any();
}

std::size_t bitset::count() const {
  return const_view(begin(), end()).count();
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  return view(*this).subview(offset, count);
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  return const_view(*this).subview(offset, count);
}

std::string to_string(const bitset& bs) {
  return to_string(bs.subview());
}

bitset operator<<(const bitset::const_view& bs, std::size_t count) {
  bitset copy(bs);
  copy <<= count;
  return copy;
}

bitset operator>>(const bitset::const_view& bs, std::size_t count) {
  bitset copy(bs);
  copy >>= count;
  return copy;
}

bitset::operator const_view() const {
  return {begin(), end()};
}

bitset::operator view() {
  return {begin(), end()};
}

std::ostream& operator<<(std::ostream& out, const bitset& bs) {
  for (auto b : bs) {
    out << b;
  }
  return out;
}

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result &= rhs;
  return result;
}

bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result |= rhs;
  return result;
}

bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result ^= rhs;
  return result;
}

bitset operator~(const bitset::const_view& view) {
  bitset result(view);
  result.flip();
  return result;
}

void swap(bitset& lhs, bitset& rhs) noexcept {
  lhs.swap(rhs);
}

void swap(bitset::view& lhs, bitset::view& rhs) noexcept {
  lhs.swap(rhs);
}

void swap(bitset::iterator& lhs, bitset::iterator& rhs) noexcept {
  lhs.swap(rhs);
}
