# Bitset

This repository contains a data structure representing a sequence of bits with support for iteration and bitwise operations.

## Key Features

- **Fixed Size:**  
  The size is determined at construction and does not change (except through assignment operators and shifts).
- **Compactness:**  
  The memory used by `bitset` does not exceed `size + C` bits, where `size` is the number of stored bits, and `C` is a constant independent of `size`.

## Supporting Classes

In addition to the `bitset` class itself, the following are included:

### Iterators

### Views

A **view** is a lightweight object that references a range of values (without owning them) and can be iterated over. In this implementation, a view is represented as a pair of iterators.

To create a `view`, use `bitset::subview(offset, count)` or the `view` constructors.

### Proxy Objects for Reference Emulation

Since bits are stored in a packed format, it is problematic to return references from `bitset::operator[]`. It’s clear that under this constraint, a `bool&` cannot be used.

To address this, the `bitset::reference` helper class is provided. It supports the following operations:

- `bs[i]` &mdash; implicitly converts to `bool`.
- `bs[i] = false` &mdash; an assignment operator that modifies the bit to the specified value.
- `bs[i].flip()` &mdash; inverts the value of the bit (for non-constant references).
