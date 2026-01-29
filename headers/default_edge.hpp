#pragma once

#include <cstddef>
#include <functional>

namespace cgrapht {
    struct DefaultEdge {
        std::size_t id {};
        explicit DefaultEdge(const std::size_t& id) : id{id} {}
        bool operator==(const DefaultEdge& other) const {
            return id == other.id;
        }
    };
}

template<>
struct std::hash<cgrapht::DefaultEdge> {
    std::size_t operator()(const cgrapht::DefaultEdge& edge) const noexcept {
        return edge.id;
    }
};
