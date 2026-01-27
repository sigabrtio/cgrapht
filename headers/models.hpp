#pragma once
#include <stdexcept>
#include <variant>

namespace cgrapht {

    template <typename SUCCESS, typename ERROR> class Result {
    private:
        std::variant<std::monostate, SUCCESS, ERROR> result {};
        bool is_success {true};
        bool is_initialized {true};

    public:
        static Result success(const SUCCESS& s) {
            Result r;
            r.result = s;
            return r;
        }

        static Result success(SUCCESS&& s) {
            Result r;
            r.result = std::move(s);
            return r;
        }

        static Result error(const ERROR& e) {
            Result r;
            r.result = e;
            r.is_success = false;
            return r;
        }

        static Result error(ERROR&& e) {
            Result r;
            r.result = std::move(e);
            r.is_success = false;
            return r;
        }

        bool operator==(const Result& other) const {
            return is_success == other.is_success && result == other.result;
        }

        [[nodiscard]] bool is_ok() const {
            return is_success;
        }

        [[nodiscard]] const SUCCESS& get_ok() const & {
            if (is_success) {
                return std::get<SUCCESS>(result);
            }
            throw std::runtime_error("Cannot get ok value from error result");
        }

        [[nodiscard]] SUCCESS consume_ok() && {
            if (!is_success) {
                return std::move(std::get<SUCCESS>(result));
            }
            throw std::runtime_error("Cannot consume ok value from error result");
        }

        [[nodiscard]] const ERROR& get_error() const & {
            if (!is_success) {
                return std::get<ERROR>(result);
            }
            throw std::runtime_error("Cannot get error value from success result");
        }

        [[nodiscard]] ERROR consume_error() && {
            if (is_success) {
                return std::move(std::get<ERROR>(result));
            }
            throw std::runtime_error("Cannot consume error value from success result");
        }
    };

    enum class ErrorType {
        INVALID_ARGUMENT,
        ABSENT_VERTX,
        ABSENT_EDGE,
        EDGE_ALREADY_EXISTS,
        VERTEX_NOT_FREE,
        INVALID_OPERATION,
        OUT_OF_RANGE,
        UNKNOWN
    };
}
