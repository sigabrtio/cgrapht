/**
 * @file commons.hpp
 * @brief Common concepts and utilities.
 */
#pragma once

#include <cstddef>
#include <functional>

namespace cgrapht {

    /**
     * @brief Concept for types hashable by std::hash.
     *
     * @tparam T Candidate type.
     */
    template <typename T>
    concept Hashable = requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

}
