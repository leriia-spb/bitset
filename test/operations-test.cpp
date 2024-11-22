#include "bitset.h"
#include "test-helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <utility>

TEST_CASE("left shift") {
  SECTION("empty") {
    bitset bs;
    std::string str;

    SECTION("without reallocation") {
      std::size_t shift_count = GENERATE(0, 1, 5);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      CHECK_THAT(bs, bitset_equals_string(str));
    }

    SECTION("with reallocation") {
      std::size_t shift_count = GENERATE(64, 190);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      CHECK_THAT(bs, bitset_equals_string(str));
    }
  }

  SECTION("single word") {
    std::string str = "1101101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    CHECK_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    CHECK_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("right shift") {
  SECTION("empty") {
    bitset bs;

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    CHECK(bs.empty());
    CHECK(bs.size() == 0);
    CHECK(bs.begin() == bs.end());
  }

  SECTION("single word") {
    std::string str = "1101101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    CHECK_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    CHECK_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("bitwise operations") {
  SECTION("empty") {
    bitset bs;

    bs &= bs;
    REQUIRE(bs.empty());

    bs |= bs;
    REQUIRE(bs.empty());

    bs ^= bs;
    REQUIRE(bs.empty());

    bs.flip();
    REQUIRE(bs.empty());

    bs.set();
    REQUIRE(bs.empty());

    bs.reset();
    REQUIRE(bs.empty());
  }

  SECTION("single bit") {
    bool lhs_bit = GENERATE(false, true);
    CAPTURE(lhs_bit);
    bitset lhs(1, lhs_bit);

    SECTION("flip") {
      lhs.flip();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == !lhs_bit);
    }

    SECTION("set") {
      lhs.set();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == true);
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == false);
    }

    bool rhs_bit = GENERATE(false, true);
    CAPTURE(rhs_bit);
    bitset rhs(1, rhs_bit);

    SECTION("bitwise and") {
      lhs &= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit && rhs_bit));
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit || rhs_bit));
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit != rhs_bit));
    }
  }

  SECTION("single word") {
    std::string_view lhs_str = "1101101";
    bitset lhs(lhs_str);

    SECTION("flip") {
      lhs.flip();
      CHECK_THAT(lhs, bitset_equals_string("0010010"));
    }

    SECTION("set") {
      lhs.set();
      CHECK_THAT(lhs, bitset_equals_string("1111111"));
    }

    SECTION("reset") {
      lhs.reset();
      CHECK_THAT(lhs, bitset_equals_string("0000000"));
    }

    std::string_view rhs_str = "0111001";
    CAPTURE(rhs_str);
    bitset rhs(rhs_str);

    SECTION("bitwise and") {
      lhs &= rhs;
      CHECK_THAT(lhs, bitset_equals_string("0101001"));
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      CHECK_THAT(lhs, bitset_equals_string("1111101"));
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      CHECK_THAT(lhs, bitset_equals_string("1010100"));
    }
  }

  SECTION("multiple words") {
    std::string_view lhs_str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset lhs(lhs_str);

    SECTION("flip") {
      lhs.flip();
      CHECK_THAT(
          lhs,
          bitset_equals_string("00001001000101111011010000010111100100000000111110011001101101110100011011001010")
      );
    }

    SECTION("set") {
      lhs.set();
      CHECK_THAT(
          lhs,
          bitset_equals_string("11111111111111111111111111111111111111111111111111111111111111111111111111111111")
      );
    }

    SECTION("reset") {
      lhs.reset();
      CHECK_THAT(
          lhs,
          bitset_equals_string("00000000000000000000000000000000000000000000000000000000000000000000000000000000")
      );
    }

    std::string_view rhs_str = "00011110011010000111001101110001000001000010001001011110010010110111011110111111";
    CAPTURE(rhs_str);
    bitset rhs(rhs_str);

    SECTION("bitwise and") {
      lhs &= rhs;
      CHECK_THAT(
          lhs,
          bitset_equals_string("00010110011010000100001101100000000001000010000001000110010010000011000100110101")
      );
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      CHECK_THAT(
          lhs,
          bitset_equals_string("11111110111010000111101111111001011011111111001001111110010010111111111110111111")
      );
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      CHECK_THAT(
          lhs,
          bitset_equals_string("11101000100000000011100010011001011010111101001000111000000000111100111010001010")
      );
    }
  }
}

TEST_CASE("bitset::all/any/count") {
  SECTION("empty") {
    bitset bs;
    CHECK(bs.all() == true);
    CHECK(bs.any() == false);
    CHECK(bs.count() == 0);
  }

  SECTION("single word") {
    std::string_view str = GENERATE("0000000", "1111111", "1101101");
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    bitset bs(str);
    CHECK(bs.all() == (ones == str.size()));
    CHECK(bs.any() == (ones != 0));
    CHECK(bs.count() == ones);
  }

  SECTION("multiple words") {
    std::string_view str = GENERATE(
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "11111111111111111111111111111111111111111111111111111111111111111111111111111111",
        "11110110111010000100101111101000011011111111000001100110010010001011100100110101"
    );
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    bitset bs(str);
    CHECK(bs.all() == (ones == str.size()));
    CHECK(bs.any() == (ones != 0));
    CHECK(bs.count() == ones);
  }
}

TEST_CASE("bitset comparison") {
  SECTION("empty") {
    bitset bs_1;
    bitset bs_2;

    CHECK(bs_1 == bs_2);
    CHECK_FALSE(bs_1 != bs_2);

    CHECK(bs_1 == bs_1);
    CHECK_FALSE(bs_1 != bs_1);
  }

  SECTION("single word") {
    std::string_view str_1 = GENERATE("10110", "10111", "101101", "101110");
    std::string_view str_2 = GENERATE("10110", "10111", "101101", "101110");
    CAPTURE(str_1, str_2);

    const bitset bs_1(str_1);
    const bitset bs_2(str_2);

    if (str_1 == str_2) {
      CHECK(bs_1 == bs_2);
      CHECK_FALSE(bs_1 != bs_2);
    } else {
      CHECK(bs_1 != bs_2);
      CHECK_FALSE(bs_1 == bs_2);
    }
  }

  SECTION("multiple words") {
    std::array strings = {
        "1111011011101000010010111110100001101111111100000110011001001",
        "11110110111010000100101111101000011011111111000001100110010010001011100100110101",
        "11110110111010000100101111101000011011111111000001100110010010000000000000000000",
    };
    std::string_view str_1 = GENERATE_REF(from_range(strings));
    std::string_view str_2 = GENERATE_REF(from_range(strings));
    CAPTURE(str_1, str_2);

    const bitset bs_1(str_1);
    const bitset bs_2(str_2);

    if (str_1 == str_2) {
      CHECK(bs_1 == bs_2);
      CHECK_FALSE(bs_1 != bs_2);
    } else {
      CHECK(bs_1 != bs_2);
      CHECK_FALSE(bs_1 == bs_2);
    }
  }
}

TEST_CASE("view operations") {
  bitset bs("1110010101");

  bitset::const_view bs_view_1 = bs.subview(0, 5);
  bitset::const_view bs_view_2 = bs.subview(5);

  CHECK((bs_view_1 & bs_view_2) == bitset("10100"));
  CHECK((bs_view_1 | bs_view_2) == bitset("11101"));
  CHECK((bs_view_1 ^ bs_view_2) == bitset("01001"));
  CHECK(~bs_view_1 == bitset("00011"));
  CHECK(bs_view_1.count() == 3);

  bs.subview()[0] = false;
  CHECK(bs == bitset("0110010101"));

  bs.subview(5).flip();
  CHECK(bs == bitset("0110001010"));

  const bitset::view bs_view_3 = bs.subview(2, 3);
  bs_view_3.set();
  CHECK(bs == bitset("0111101010"));

  bs.subview(2, 3) ^= bs.subview(7, 3);
  CHECK(bs == bitset("0110101010"));

  bs.subview() &= std::as_const(bs).subview();
  CHECK(bs == bitset("0110101010"));
}

TEST_CASE("chained view operations") {
  bitset bs_1("0011000011");
  bitset bs_2("1110010101");

  bitset::view bs_view_1 = bs_1.subview();
  bitset::view bs_view_2 = bs_2.subview();

  bs_view_1 |= bs_view_2.flip();
  CHECK(bs_1 == bitset("0011101011"));
  CHECK(bs_2 == bitset("0001101010"));

  bs_view_1.flip() |= bs_view_2;
  CHECK(bs_1 == bitset("1101111110"));
  CHECK(bs_2 == bitset("0001101010"));

  bs_view_1 &= std::as_const(bs_view_2).flip();
  CHECK(bs_1 == bitset("1100010100"));
  CHECK(bs_2 == bitset("1110010101"));

  std::as_const(bs_view_1).flip() &= bs_view_2;
  CHECK(bs_1 == bitset("0010000001"));
  CHECK(bs_2 == bitset("1110010101"));
}

TEST_CASE("Anton's mega tests") {
  const bitset zeros_bs = bitset("00000000000000000000");
  const bitset ones_bs = bitset("11111111111111111111");
  const bitset chess_bs = bitset("10101010101010101010");
  const bitset half_half_bs = bitset("11111111110000000000");
  CHECK(zeros_bs.size() == 20);
  CHECK(zeros_bs == zeros_bs);
  CHECK(ones_bs == ones_bs);
  CHECK(chess_bs == chess_bs);
  CHECK(half_half_bs == half_half_bs);
  CHECK(zeros_bs == bitset(20, false));
  CHECK(ones_bs == bitset(ones_bs));
  CHECK(chess_bs == bitset(chess_bs.subview(0)));
  CHECK(bitset(zeros_bs.begin(), zeros_bs.begin() + 10) == bitset(half_half_bs.begin() + 10, half_half_bs.end()));
  CHECK(zeros_bs != ones_bs);
  CHECK(half_half_bs != chess_bs);
  CHECK(!(ones_bs.subview(0, 10) != half_half_bs.subview(0, 10)));
  bitset zeros_cp;
  zeros_cp = zeros_bs;
  CHECK(zeros_cp == zeros_bs);
  bitset ones_cp;
  ones_cp = ones_bs.subview(0, 20);
  CHECK(ones_cp == ones_bs.subview(0, 20));
  bitset random_bs;
  random_bs = "00011101000101110101";

  bitset chess_cp;
  chess_cp = chess_bs;

  swap(random_bs, chess_cp);
  CHECK(random_bs == chess_bs);
  CHECK(chess_cp == bitset("00011101000101110101"));
  swap(random_bs, chess_cp);
  CHECK(chess_cp == chess_bs);
  CHECK(random_bs == bitset("00011101000101110101"));
  swap(random_bs, random_bs);
  CHECK(random_bs == bitset("00011101000101110101"));

  CHECK(random_bs.size() == 20);
  const bitset empty_bs;
  CHECK(empty_bs.size() == 0);
  CHECK(empty_bs.empty());
  const bitset bit_bs = bitset("1");
  CHECK(bit_bs.size() == 1);
  CHECK(!bit_bs.empty());

  CHECK(bit_bs[0]);
  CHECK(!random_bs[0]);
  CHECK(random_bs[3]);
  CHECK(ones_bs[13]);
  CHECK(ones_cp[13]);
  for (std::size_t i = 0; i < chess_cp.size(); ++i) {
    CHECK(chess_cp[i] == chess_bs[i]);
  }
  std::size_t i = 0;
  for (bitset::const_iterator it = random_bs.begin(); it != random_bs.end(); ++it, ++i) {
    CHECK(*it == random_bs[i]);
  }

  for (bitset::const_iterator it = random_bs.begin(); it != random_bs.end(); ++it) {
    CHECK(*it == *it);
  }

  for (bitset::const_iterator it = chess_bs.begin(), it2 = chess_cp.begin(); it != chess_bs.end(); ++it, ++it2) {
    CHECK(*it == *it2);
  }

  bitset random_cp = bitset(random_bs.size(), false);
  bitset::const_iterator it = random_bs.begin();
  for (bitset::iterator it2 = random_cp.begin(); it2 != random_cp.end(); ++it2, ++it) {
    *it2 = *it;
  }
  CHECK(random_cp == random_bs);
  bitset few_ones = bitset(ones_bs.size() / 3, false);
  it = ones_bs.begin();
  for (bitset::iterator it2 = few_ones.begin(); it < ones_bs.end(); it += 3, ++it2) {
    *it2 = *it;
  }
  CHECK(few_ones == bitset(ones_bs.size() / 3, true));

  CHECK((random_cp &= zeros_cp) == zeros_bs);
  random_cp = random_bs;
  CHECK((random_cp |= zeros_cp) == random_bs);
  CHECK((random_cp &= ones_cp &= zeros_cp) == zeros_bs);
  ones_cp = ones_bs;
  random_cp = random_bs;
  CHECK((random_bs ^ random_bs) == zeros_bs);
  CHECK((random_bs | ones_bs) == ones_bs);
  CHECK((random_bs & ones_bs) == random_bs);
  CHECK((random_bs & zeros_bs) == zeros_bs);
  CHECK((random_bs ^ chess_bs.subview(0)) == (random_bs.subview(0) ^ chess_bs));
  CHECK((random_bs.subview(0) ^ random_bs.subview(0)) == zeros_cp.subview(0));
  const bitset ten_ones_bs = ones_bs >> 10;
  CHECK(ten_ones_bs.size() == 10);
  CHECK(ten_ones_bs == bitset("1111111111"));
  CHECK((ten_ones_bs << 10) == half_half_bs);
  CHECK((half_half_bs >> 10) == ten_ones_bs);
  bitset ten_ones_cp = ten_ones_bs << 10 >> 10 << 1000 >> 1000;
  ten_ones_cp.flip();
  CHECK(ten_ones_cp == zeros_cp.subview(5, 10));
  ten_ones_cp.set();
  CHECK(ten_ones_cp == ones_cp.subview(5, 10));

  CHECK(ten_ones_cp.reset() == zeros_cp.subview(7, 10));
  ten_ones_cp.set();
  ten_ones_cp = ten_ones_cp <<= 100;
  ten_ones_cp = ten_ones_cp <<= 300;
  CHECK(ten_ones_cp.size() == 410);
  ten_ones_cp >>= 400;
  CHECK(ten_ones_cp == ones_bs.subview(0, 10));
  CHECK(ones_cp.count() == 20);
  CHECK(ones_bs.count() == 20);
  CHECK(zeros_cp.count() == 0);
  CHECK(chess_cp.count() == 10);
  CHECK(ten_ones_cp.count() == 10);
  CHECK(random_bs.count() == 10);

  bitset::const_view zeros_r_cview = zeros_cp.subview(10, 10);
  bitset::view zeros_r_view = zeros_cp.subview(10, 10);
  zeros_r_view.flip();
  CHECK(zeros_r_cview.all());
  CHECK(zeros_r_view.all());
  zeros_r_view.reset();
  CHECK(!zeros_r_cview.any());
  zeros_r_view |= zeros_r_view |= ten_ones_bs;
  CHECK(zeros_r_cview.all());
  CHECK(zeros_r_view.all());
  zeros_r_view &= ~zeros_r_view;
  CHECK(!zeros_r_cview.any());
  zeros_cp.subview(0) &= zeros_cp &= zeros_cp.subview(0);
  CHECK(!zeros_r_cview.any());

  bitset::view view_eq;
  view_eq = chess_cp;
  bitset::view view_eq_cp;
  view_eq_cp = view_eq;
  CHECK(view_eq_cp == chess_bs);
  const bitset::const_view ones_cview = ones_bs;

  bitset big_chess_bs = bitset(1000, false);
  for (bitset::reference bit : big_chess_bs) {
    CHECK(!bit);
  }
  for (bitset::iterator it = big_chess_bs.begin(); it != big_chess_bs.end(); it += 2) {
    *it = true;
  }
  CHECK(big_chess_bs.subview(36, 20) == chess_bs);
  i = 0;
  for (bool bit : big_chess_bs) {
    CHECK(big_chess_bs[i] == bit);
    ++i;
  }

  bitset::const_iterator big_chess_cit = big_chess_bs.begin() + 100;
  bitset::iterator big_chess_it = big_chess_bs.begin() + 100;
  CHECK(big_chess_it == big_chess_cit);
  CHECK(big_chess_cit == big_chess_it);
  CHECK(big_chess_cit >= big_chess_it);
  CHECK(big_chess_cit <= big_chess_it);

  big_chess_it += 100;
  big_chess_cit += 100;
  CHECK(100 == (big_chess_it + 100) - big_chess_it);
  CHECK(-100 == (big_chess_it - 100) - big_chess_it);
  CHECK(100 == big_chess_it - (big_chess_it - 100));
  CHECK(-100 == big_chess_it - (big_chess_it + 100));
  bitset_view<uint64_t> s = big_chess_bs.subview();
  bitset_view<const uint64_t> p = s.subview();

  CHECK(big_chess_cit == ((big_chess_it += 200) -= 200));
  CHECK((big_chess_cit += 100) == ((big_chess_it += 200) -= 100));
  big_chess_cit += 100;
  CHECK(big_chess_cit > big_chess_it);
  big_chess_it += 200;
  CHECK(big_chess_cit < big_chess_it);
  CHECK(big_chess_it > big_chess_cit);
  CHECK(big_chess_cit == (big_chess_it -= 100));
  big_chess_cit += 65;
  for (std::size_t j = 0; j < 65; ++j) {
    big_chess_it++;
  }
  CHECK(big_chess_cit == big_chess_it);

  for (bitset::iterator it = big_chess_bs.end(); it > big_chess_bs.begin(); it -= 2) {
    CHECK(!*(it - 1));
  }

  bitset for_conversions = bitset("010");
  const bitset for_conversions_const = bitset("010");
  bitset::view big_chess_view = big_chess_bs.subview(0);
  bitset::const_view big_chess_cview = big_chess_bs.subview(0);
  for_conversions = big_chess_view;
  for_conversions = big_chess_cview;
  for_conversions = for_conversions_const;
  CHECK(for_conversions_const == for_conversions_const.subview(0));
  CHECK(for_conversions_const[0] == for_conversions_const[0]);
  CHECK(for_conversions[0] == for_conversions_const[0]);
  CHECK(for_conversions_const[0] == for_conversions[0]);
  static_assert(std::is_trivial_v<bitset::const_reference>);
  static_assert(std::is_trivial_v<bitset::reference>);
  static_assert(std::is_trivial_v<bitset::iterator>);
  static_assert(std::is_trivial_v<bitset::view>);
}

TEST_CASE("bad") {
  bitset bs("11111111111111111111111111111111111111111111111111111111111111110");

  size_t offset = 63;
  size_t count = 2;

  bitset copy = bs;
  bitset::view view = copy.subview(offset, count);

  bs.flip();
  view.flip();

  CHECK(copy.subview(offset, count) == bs.subview(offset, count));
  bitset c(view.begin(), view.begin());
  std::cout << c;
}

TEST_CASE("1") {
  bitset original(10, true); // Создаем битсет с 10 установленными битами
  bitset::const_view view = original.subview(0, 10); // Получаем вью
  bitset copy(view); // Конструктор из вью

  CHECK(copy.size() == original.size());
  for (std::size_t i = 0; i < original.size(); ++i) {
    CHECK(copy[i] == original[i]);
  }
}

TEST_CASE("2") {
  bitset original(8, false); // Создаем битсет с 8 сброшенными битами
  original.set(); // Устанавливаем все биты
  auto begin = original.begin();
  auto end = original.end();

  bitset copy(begin, end); // Конструктор из итераторов

  CHECK(copy.size() == original.size());
  for (std::size_t i = 0; i < original.size(); ++i) {
    CHECK(copy[i] == original[i]);
  }
}

TEST_CASE("3") {
  bitset copy("11010010"); // Конструктор из строкового представления
  CHECK(copy.size() == 8); // Проверяем размер
  CHECK(copy[0] == 1); // Проверяем, что первый бит установлен
  CHECK(copy[1] == 1); // Проверяем, что второй бит сброшен
  CHECK(copy[2] == 0); // Проверяем, что третий бит установлен
  CHECK(copy[3] == 1);
  auto v = copy.subview(3, -1);
  bitset t(v);
  CHECK(t == v);
  bitset r;
  r = v;
}

TEST_CASE("4") {
  bitset original(5, true); // Создаем битсет
  bitset copy = original; // Используем оператор присваивания

  CHECK(copy.size() == original.size());
  for (std::size_t i = 0; i < original.size(); ++i) {
    CHECK(copy[i] == original[i]);
  }
}

TEST_CASE("5") {
  bitset a(5, true);
  bitset b(5, true);
  bitset c(5, false);

  CHECK(a == b);
  CHECK(a != c);
}

TEST_CASE("ConstructorFromFullRange") {
  bitset original(10, true); // Исходный bitset с 10 установленными битами
  bitset copy(original.begin(), original.end()); // Конструктор из итераторов

  CHECK(copy.size() == original.size()); // Проверяем совпадение размеров
  for (std::size_t i = 0; i < original.size(); ++i) {
    CHECK(copy[i] == original[i]); // Проверяем, что биты совпадают
  }
}

// Тест для конструктора с итераторами: часть диапазона
TEST_CASE("ConstructorFromPartialRange") {
  bitset original(10, true); // Исходный bitset с 10 установленными битами
  original[2].flip(); // Сбрасываем бит в позиции 2
  original[7].flip(); // Сбрасываем бит в позиции 7

  // Создаем копию с итераторами от 3 до 8
  bitset copy(original.begin() + 3, original.begin() + 8);

  CHECK(copy.size() == 5); // Новый bitset должен быть размером 5
  for (std::size_t i = 0; i < copy.size(); ++i) {
    CHECK(copy[i] == original[i + 3]); // Проверяем, что биты совпадают с исходным диапазоном
  }
}

// Тест для конструктора с итераторами: пустой диапазон
TEST_CASE("ConstructorFromEmptyRange") {
  bitset original(10, true); // Исходный bitset с 10 установленными битами

  // Пытаемся создать bitset из пустого диапазона
  bitset copy(original.begin(), original.begin());

  CHECK(copy.size() == 0); // Новый bitset должен быть пустым
  CHECK(copy.empty()); // Проверка на пустоту
}

// Тест для конструктора с итераторами: один бит
TEST_CASE("ConstructorFromSingleElement") {
  bitset original(10, true); // Исходный bitset с 10 установленными битами
  original[4].flip(); // Сбрасываем бит в позиции 4

  // Создаем новый bitset из итератора, указывающего на единственный бит
  bitset copy(original.begin() + 4, original.begin() + 5);

  CHECK(copy.size() == 1); // Новый bitset должен быть размером 1
  CHECK(!copy[0]); // Проверяем, что единственный бит сброшен
}

// Тест для конструктора с итераторами: весь диапазон с пустыми битами
TEST_CASE("ConstructorFromFullRangeWithEmptyBits") {
  bitset original("1100101010100000111010010"); // Исходный bitset с 10 сброшенными битами

  // Создаем копию всего диапазона
  auto copy = original.subview(1);

  auto it1 = copy.begin();
  auto it2 = original.begin() + 1;
  CHECK(copy.size() + 1 == original.size()); // Размер должен совпадать с исходным
  for (std::size_t i = 1; i < copy.size(); ++i) {
    CHECK(it1++ == it2++);
    CHECK(copy[i - 1] == original[i]); // Проверяем, что все биты сброшены
  }
  copy.subview(2, 3).flip();
  it1 = copy.begin();
  it2 = original.begin() + 1;
  CHECK(copy.size() + 1 == original.size()); // Размер должен совпадать с исходным
  for (std::size_t i = 1; i < copy.size(); ++i) {
    CHECK(it1++ == it2++);

    CHECK(copy[i - 1] == original[i]); // Проверяем, что все биты сброшены
  }
  original.flip();
  it1 = copy.begin();
  it2 = original.begin() + 1;
  CHECK(copy.size() + 1 == original.size()); // Размер должен совпадать с исходным
  for (std::size_t i = 1; i < copy.size(); ++i) {
    CHECK(it1++ == it2++);
    CHECK(copy[i - 1] == original[i]); // Проверяем, что все биты сброшены
  }
  original[1] = true;

  original[3] = true;

  original[5] = true;

  original[7] = true;
  it1 = copy.begin();
  it2 = original.begin() + 1;
  CHECK(copy.size() + 1 == original.size()); // Размер должен совпадать с исходным
  for (std::size_t i = 1; i < copy.size(); ++i) {
    CHECK(it1++ == it2++);
    CHECK(copy[i - 1] == original[i]); // Проверяем, что все биты сброшены
  }
}

TEST_CASE("LAST") {
  bitset bs1(65, false);

  bitset copy = bs1;
  bitset::view view = copy.subview(1, 64);

  view.set();

  CHECK(view.all());
  bitset data("1011");
  bitset bs(data.begin(), data.end());

  CHECK(bs.size() == data.size());
  CHECK(bs[0] == true);
  CHECK(bs[1] == false);
  CHECK(bs[2] == true);
  CHECK(bs[3] == true);
}

TEST_CASE("LAST-last") {
  bitset bs(65, false);

  bitset bs1(257, false);
  bitset bs2(255, true);

  bitset copy = bs;
  bitset::view view = copy.subview(1, 64);

  view.set();
  CHECK(bs1 == bs1);
  CHECK(bs2 == bs2);
  CHECK(view.all());

  bitset original("11001100");
  // Assuming we want a subview from index 2 to 5
  auto subview = (original >> 2) | ~bitset(6, false);
  CHECK(subview == bitset(6, true));
}
TEST_CASE("may be baby") {
  bitset bs(0, false);
  bs.set();

  bitset bs1(0, true);
  bs1.reset();
  CHECK(bs1.all());
  CHECK(!bs.any());


  bitset copy = bs;
  bitset::view view = copy.subview(1, 64);

  view.set();
  CHECK(bs1 == bs1);
  CHECK(bs1 == bs);

  bitset original("11001100");
  // Assuming we want a subview from index 2 to 5
  auto subview = (original >> 10) | ~bitset(0, false);
}
TEST_CASE("ub?") {
  bitset bs(0, false);
  bs.set();

  bitset bs1(0, true);
  bs1.reset();

  CHECK((bs1 & bs) == bs1);
  CHECK((bs1 | bs) == bs1);
  CHECK((bs1 ^ bs) == bs1);

}