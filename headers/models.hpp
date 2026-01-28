/**
 * @file models.hpp
 * @brief Core result and error types.
 */
#pragma once
#include <stdexcept>
#include <variant>

namespace cgrapht {

    /**
     * @brief Result wrapper for success or error values.
     *
     * @tparam SUCCESS Success value type.
     * @tparam ERROR Error value type.
     */
    template <typename SUCCESS, typename ERROR> class Result {
    private:
        std::variant<std::monostate, SUCCESS, ERROR> result {};
        bool is_success {true};
        Result() = default;

    public:
        /**
         * @brief Construct a success result from a const reference.
         * @param s Success value.
         * @return Result containing success.
         */
        static Result success(const SUCCESS& s) {
            Result r;
            r.result = s;
            return r;
        }

        /**
         * @brief Construct a success result from an rvalue.
         * @param s Success value.
         * @return Result containing success.
         */
        static Result success(SUCCESS&& s) {
            Result r;
            r.result = std::move(s);
            return r;
        }

        /**
         * @brief Construct an error result from a const reference.
         * @param e Error value.
         * @return Result containing error.
         */
        static Result error(const ERROR& e) {
            Result r;
            r.result = e;
            r.is_success = false;
            return r;
        }

        /**
         * @brief Construct an error result from an rvalue.
         * @param e Error value.
         * @return Result containing error.
         */
        static Result error(ERROR&& e) {
            Result r;
            r.result = std::move(e);
            r.is_success = false;
            return r;
        }

        /**
         * @brief Equality comparison based on status and payload.
         */
        bool operator==(const Result& other) const {
            return is_success == other.is_success && result == other.result;
        }

        /**
         * @brief Check if the result represents success.
         * @return True if success.
         */
        [[nodiscard]] bool is_ok() const {
            return is_success;
        }

        /**
         * @brief Access success value as const reference.
         * @return Success value.
         * @throws std::runtime_error if this is an error result.
         */
        [[nodiscard]] const SUCCESS& get_ok() const & {
            if (is_success) {
                return std::get<SUCCESS>(result);
            }
            throw std::runtime_error("Cannot get ok value from error result");
        }

        /**
         * @brief Move success value out of the result.
         * @return Success value.
         * @throws std::runtime_error if this is an error result.
         */
        [[nodiscard]] SUCCESS consume_ok() && {
            if (is_success) {
                return std::move(std::get<SUCCESS>(result));
            }
            throw std::runtime_error("Cannot consume ok value from error result");
        }

        /**
         * @brief Access error value as const reference.
         * @return Error value.
         * @throws std::runtime_error if this is a success result.
         */
        [[nodiscard]] const ERROR& get_error() const & {
            if (!is_success) {
                return std::get<ERROR>(result);
            }
            throw std::runtime_error("Cannot get error value from success result");
        }

        /**
         * @brief Move error value out of the result.
         * @return Error value.
         * @throws std::runtime_error if this is a success result.
         */
        [[nodiscard]] ERROR consume_error() && {
            if (!is_success) {
                return std::move(std::get<ERROR>(result));
            }
            throw std::runtime_error("Cannot consume error value from success result");
        }
    };

    /**
     * @brief Error codes used by graph operations.
     */
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
