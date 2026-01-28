#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>

#include "models.hpp"

using cgrapht::Result;
using cgrapht::ErrorType;

class Movable {
private:
    bool valid;
    int val;

public:
    explicit Movable(const int v) : valid(true), val(v) {};
    Movable()=delete;

    Movable(Movable&& m) noexcept {
            valid = m.valid;
            val = m.val;
            m.valid = false;
    };

    Movable& operator=(Movable&& m) noexcept {
        this->val = m.val;
        this->valid = m.valid;
        m.valid = false;
        return *this;
    }

    bool is_valid() const {return valid;}
    int get_val() const {return val;}

};

SCENARIO("Result construction cases") {

    GIVEN("I construct a success") {

        Result r {Result<int, ErrorType>::success(1)};

        THEN("The result should be a success") {

            REQUIRE(r.is_ok());
            REQUIRE(1 == r.get_ok());
        }
    }

    GIVEN("I construct an error") {

        Result r {Result<int, ErrorType>::error(ErrorType::ABSENT_VERTX)};

        THEN("The result should be an error") {

            REQUIRE(!r.is_ok());
            REQUIRE(ErrorType::ABSENT_VERTX == r.get_error());
        }
    }
}

SCENARIO("Result consumption cases") {

    GIVEN("I consume a result") {

        Result r {Result<Movable, ErrorType>::success(Movable(23))};
        Result r2 {Result<int, Movable>::error(Movable(32))};

        THEN("The result should be consumed") {

            Movable m {std::move(r).consume_ok()};
            Movable m2 {std::move(r2).consume_error()};

            AND_THEN("The consumed values should be valid") {

                REQUIRE(m.is_valid());
                REQUIRE(m2.is_valid());
                REQUIRE(m.get_val() == 23);
                REQUIRE(m2.get_val() == 32);

                AND_THEN("The original values should be invalid because they have been moved") {

                    REQUIRE(!r.get_ok().is_valid());
                    REQUIRE(!r2.get_error().is_valid());
                }
            }
        }
    }
}