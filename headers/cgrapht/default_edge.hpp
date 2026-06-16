#pragma once

#include <cstddef>
#include <functional>

namespace cgrapht {
    /**
     * @brief A library supplied edge type.
     *
     * This is a library supplied edge type. This takes an `id` parameter in the constructor and uses that as it's
     * hash value itself. For simple academic cases, this is good enough to be used as is.
     *
     * Usage:
     * ```cpp
     * DirectedGraph<int, DefaultEdge> graph;
     * graph.add_edge(v1_id, v2_id, DefaultEdge{1});
     * ```
     *
     */
    struct DefaultEdge {
        /**
         * @brief User passed ID parameter that is used as the hash value
         */
        std::size_t id {};

        /**
         * @brief The constructor accepting a `stze_t` type argument that it uses as hash later
         *
         * @param id
         */
        explicit DefaultEdge(const std::size_t id) : id{id} {}
        bool operator==(const DefaultEdge& other) const = default;
    };
}

/// @cond INTERNAL
template<>
struct std::hash<cgrapht::DefaultEdge> {
    std::size_t operator()(const cgrapht::DefaultEdge& edge) const noexcept {
        return edge.id;
    }
};
/// @endcond
