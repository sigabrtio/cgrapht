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
     * This concept is used in the graph definition to ake sure that all vertex and edge types are actually hashable. Hashability
     * is at the core of the design of this library.
     *
     * @tparam T Candidate type.
     */
    template <typename T>
    concept Hashable = requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

}
