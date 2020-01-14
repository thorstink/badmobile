#include "buffer.h"
#include <catch2/catch.hpp>
#include <iostream>

constexpr auto buffer_size = 4;

SCENARIO("full, empty and size of buffer", "[buffer]") {
  GIVEN("An empty fixed size ringbuffer") {
    Observable<int, buffer_size> o;
    THEN("It is empty and not full and its size is 0") {
      REQUIRE(o.empty());
      REQUIRE(!o.full());
      REQUIRE(o.size() == 0);
    }
    WHEN("We fill the buffer halfway") {
      for (int i = 0; i < buffer_size / 2;) {
        o.onNext(++i);
      }
      THEN("The buffer is not empty, not full, and has size half the buffer "
           "size") {
        REQUIRE(!o.empty());
        REQUIRE(!o.full());
        REQUIRE(o.size() == buffer_size / 2);
        WHEN("If we add one more sample") {
          int c = buffer_size / 2 + 1;
          o.onNext(c);
          THEN("The buffer's size is increased by one") {
            REQUIRE(o.size() == c);
            WHEN("we fill the remaing buffer") {
              for (int i = c; i < buffer_size;) {
                o.onNext(++i);
              }
              THEN("The buffer size is increased until it is full") {
                REQUIRE(o.full());
                REQUIRE(o.size() == buffer_size);
              }
            }
          }
        }
      }
    }
  }
}

SCENARIO("When there's a consumer", "[buffer]") {
  GIVEN("A full fixed size ringbuffer") {

    Observable<int, buffer_size> o;
    auto s = o.subscribe();
    for (int i = 0; i < buffer_size;) {
      o.onNext(++i);
    }
    THEN("Through a subscription we can consume the values") {
      for (int i = 0; i < buffer_size;) {
        if (auto j = s.pop())
          CHECK(++i == *j);
      }
    }
  }
}

SCENARIO("When there's are two consumers", "[buffer]") {
  GIVEN("A full fixed size ringbuffer with two early subscribers") {

    Observable<int, buffer_size> o;
    auto s1 = o.subscribe();
    auto s2 = o.subscribe();

    for (int i = 0; i < buffer_size;) {
      o.onNext(++i);
    }

    THEN("let both subscription process the values") {
      for (int i = 0; i < buffer_size;) {
        i++;
        if (auto j = s1.pop())
          CHECK(i == *j);
        if (auto j = s2.pop())
          CHECK(i == *j);
      }
      o.onNext(buffer_size + 1);
      if (auto j = s2.pop())
        CHECK(buffer_size + 1 == *j);
    }
  }

  GIVEN("A full fixed size ringbuffer with no subscribers") {
    Observable<int, buffer_size> o;
    for (int i = 0; i < buffer_size;) {
      o.onNext(++i);
    }
    WHEN("The static buffer size is exceeded")
    // o.onNext(buffer_size + 1);
    THEN("The oldest value will simply be overwritten") {}
  }
}
