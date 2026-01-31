/**
 * @file graph.hpp
 *
 * @brief This class represents a generic Directed Graph.
 *
 * @Detail
 * This class provides a generic implementation of a directed graph with hashed vertex and edge ids.
 * It provides interfaces for constructing a graph by adding vertices and edges. Adding any item in a graph
 * like a vertex or edge returns an identifier that can be used later to retrieve the item.
 *
 * Also, this graph operates on an identifier / query based approach. Any graph operations like fetching
 * neighbouring vertices or getting a list of edges do not return the actual vertex or edge, but the said
 * identifiers. You can use these identifiers to retrieve the actual vertex or edge from the graph.
 *
 */

#pragma once

#include <unordered_set>
#include <functional>
#include <ranges>
#include <unordered_map>
#include <string>

#include <iostream>
#include <set>

#include "cgrapht/commons.hpp"
#include "models.hpp"

namespace cgrapht {
    /**
     * @brief Edge record for a directed graph.
     *
     * @tparam E Edge payload type.
     */
    template <Hashable E>
    struct Edge {
        std::size_t from_id;
        std::size_t to_id;
        E edge;

        bool operator==(const Edge& other) const& {
            return from_id == other.from_id && edge == other.edge && to_id == other.to_id;
        }
    };

    /**
     * @internal
     * @brief Incoming/outgoing edge index sets for a vertex.
     */
    struct EdgeSet {
        std::unordered_set<std::size_t> incoming_edges;
        std::unordered_set<std::size_t> outgoing_edges;
    };

    /**
     * @brief Directed graph with hashed vertex and edge ids.
     *
     * @tparam V Vertex payload type.
     * @tparam E Edge payload type.
     *
     * @note Both `V` and `E` must be hashable types.
     */
    template <Hashable V, Hashable E>
    class DirectedGraph {
    private:
        std::unordered_map<std::size_t, V> vertex_index{};
        std::unordered_map<std::size_t, Edge<E>> edge_index{};
        std::unordered_map<std::size_t, EdgeSet> adjacency_list{};

    public:
        /**
         * @brief Add a vertex to the graph.
         * @param v Vertex payload.
         * @return Result containing the vertex id or an error.
         */
        Result<std::size_t, ErrorType> add_vertex(const V& v);
        /**
         * @brief Delete a vertex if it has no incident edges.
         * @param vertex_id Vertex id.
         * @return Result containing the deleted vertex id or an error.
         */
        Result<std::size_t, ErrorType> delete_vertex(const std::size_t& vertex_id);
        /**
         * @brief Add a directed edge between two vertices.
         * @param from_id Source vertex id.
         * @param to_id Destination vertex id.
         * @param e Edge payload.
         * @return Result containing the edge id or an error.
         */
        Result<std::size_t, ErrorType> add_edge(const std::size_t& from_id, const std::size_t& to_id, const E& e);
        /**
         * @brief Delete an edge by id.
         * @param edge_id Edge id.
         * @return Result containing the deleted edge id or an error.
         */
        Result<std::size_t, ErrorType> delete_edge(const std::size_t& edge_id);
        /**
         * @brief Fetch a vertex payload by id.
         * @param id Vertex id.
         * @return Result containing the vertex or an error.
         */
        Result<V, ErrorType> get_vertex(const std::size_t& id) const;
        /**
         * @brief Fetch an edge record by id.
         * @param id Edge id.
         * @return Result containing the edge or an error.
         */
        Result<Edge<E>, ErrorType> get_edge(const std::size_t& id) const;
        /**
         * @brief Get adjacent children (outgoing neighbors).
         * @param vertex_id Vertex id.
         * @return Result containing a set of vertex ids or an error.
         */
        Result<std::unordered_set<std::size_t>, ErrorType> get_children(const std::size_t& vertex_id) const;
        /**
         * @brief Get adjacent parents (incoming neighbors).
         * @param vertex_id Vertex id.
         * @return Result containing a set of vertex ids or an error.
         */
        Result<std::unordered_set<std::size_t>, ErrorType> get_parents(const std::size_t& vertex_id) const;
        /**
         * @brief Get all adjacent neighbors (incoming or outgoing).
         * @param vertex_id Vertex id.
         * @return Result containing a set of vertex ids or an error.
         */
        Result<std::unordered_set<std::size_t>, ErrorType> get_neighbours(const std::size_t& vertex_id) const;
        /**
         * @brief Get outgoing edge ids for a vertex.
         * @param vertex_id Vertex id.
         * @return Result containing a set of edge ids or an error.
         */
        Result<std::unordered_set<std::size_t>, ErrorType> get_outgoing_edges(const std::size_t& vertex_id) const;
        /**
         * @brief Get incoming edge ids for a vertex.
         * @param vertex_id Vertex id.
         * @return Result containing a set of edge ids or an error.
         */
        Result<std::unordered_set<std::size_t>, ErrorType> get_incoming_edges(const std::size_t& vertex_id) const;

        /**
         * @brief View of all vertex payloads.
         * @return Forward range of vertices.
         */
        std::ranges::forward_range auto get_vertices() const & {
            return vertex_index | std::views::values;
        }

        /**
         * @brief View of all edge records.
         * @return Forward range of edges.
         */
        std::ranges::forward_range auto get_edges() const & {
            return edge_index | std::views::values;
        }
    };

    template <Hashable V, Hashable E> Result<std::size_t, ErrorType> DirectedGraph<V, E>::add_vertex(const V& v) {
        std::size_t vertex_id {std::hash<V>{}(v)};
        if (!vertex_index.contains(vertex_id)) {
            vertex_index.emplace(vertex_id, v);
            adjacency_list.emplace(vertex_id, EdgeSet{});
        }
        return Result<std::size_t, ErrorType>::success(vertex_id);
    }

    template <Hashable V, Hashable E> Result<std::size_t, ErrorType> DirectedGraph<V, E>::delete_vertex(const std::size_t& vertex_id) {
        if (!vertex_index.contains(vertex_id)) {
            return Result<std::size_t, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }
        if (adjacency_list.contains(vertex_id) && adjacency_list[vertex_id].incoming_edges.empty() && adjacency_list[vertex_id].outgoing_edges.empty()) {
            adjacency_list.erase(vertex_id);
            vertex_index.erase(vertex_id);
            return Result<std::size_t, ErrorType>::success(vertex_id);
        }
        return Result<std::size_t, ErrorType>::error(ErrorType::VERTEX_NOT_FREE);
    }

    template <Hashable V, Hashable E> Result<std::size_t, ErrorType> DirectedGraph<V, E>::add_edge(const std::size_t& from_id, const std::size_t& to_id, const E& e) {
        if (!vertex_index.contains(from_id) || !vertex_index.contains(to_id)) {
            return Result<std::size_t, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }

        if (std::size_t edge_id {std::hash<E>{}(e)}; edge_index.contains(edge_id)) {
            if (Edge edge_info = edge_index.at(edge_id); edge_info.from_id != from_id || edge_info.to_id != to_id) {
                return Result<std::size_t, ErrorType>::error(ErrorType::EDGE_ALREADY_EXISTS);
            }
            return Result<std::size_t, ErrorType>::success(edge_id);
        } else {
            edge_index.emplace(edge_id, Edge<E>{from_id, to_id, e});
            adjacency_list[from_id].outgoing_edges.insert(edge_id);
            adjacency_list[to_id].incoming_edges.insert(edge_id);
            return Result<std::size_t, ErrorType>::success(edge_id);
        }
    }

    template <Hashable V, Hashable E> Result<std::size_t, ErrorType> DirectedGraph<V, E>::delete_edge(const std::size_t& edge_id) {
        if (edge_index.contains(edge_id)) {
            const std::size_t from_id = edge_index.at(edge_id).from_id;
            const std::size_t to_id = edge_index.at(edge_id).to_id;
            edge_index.erase(edge_id);
            adjacency_list[from_id].outgoing_edges.erase(edge_id);
            adjacency_list[to_id].incoming_edges.erase(edge_id);
            return Result<std::size_t, ErrorType>::success(edge_id);
        }
        return Result<std::size_t, ErrorType>::error(ErrorType::ABSENT_EDGE);
    }

    template <Hashable V, Hashable E> Result<V, ErrorType> DirectedGraph<V, E>::get_vertex(const std::size_t& id) const {
        if (vertex_index.contains(id)) {
            return Result<V, ErrorType>::success(vertex_index.at(id));
        }
        return Result<V, ErrorType>::error(ErrorType::ABSENT_VERTX);
    }

    template <Hashable V, Hashable E> Result<Edge<E>, ErrorType> DirectedGraph<V, E>::get_edge(const std::size_t& id) const {
        if (edge_index.contains(id)) {
            return Result<Edge<E>, ErrorType>::success(edge_index.at(id));
        }
        return Result<Edge<E>, ErrorType>::error(ErrorType::ABSENT_EDGE);
    }

    template <Hashable V, Hashable E> Result<std::unordered_set<std::size_t>, ErrorType> DirectedGraph<V, E>::get_children(const std::size_t& vertex_id) const {
        if (!adjacency_list.contains(vertex_id)) {
            return Result<std::unordered_set<std::size_t>, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }
        auto children = adjacency_list.at(vertex_id).outgoing_edges
        | std::views::transform([this](const auto& edge_id) {
            return edge_index.at(edge_id).to_id;
          });
        std::unordered_set<std::size_t> children_set(children.begin(), children.end());
        return Result<std::unordered_set<std::size_t>, ErrorType>::success(std::move(children_set));
    }

    template <Hashable V, Hashable E> Result<std::unordered_set<std::size_t>, ErrorType> DirectedGraph<V, E>::get_parents(const std::size_t& vertex_id) const {
        if (!adjacency_list.contains(vertex_id)) {
            return Result<std::unordered_set<std::size_t>, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }
        auto parents = adjacency_list.at(vertex_id).incoming_edges
        | std::views::transform([this](const auto& edge_id) {
            return edge_index.at(edge_id).from_id;
          });
        std::unordered_set<std::size_t> parent_set(parents.begin(), parents.end());
        return Result<std::unordered_set<std::size_t>, ErrorType>::success(std::move(parent_set));
    }

    template <Hashable V, Hashable E> Result<std::unordered_set<std::size_t>, ErrorType> DirectedGraph<V, E>::get_neighbours(const std::size_t& vertex_id) const {
        if (!adjacency_list.contains(vertex_id)) {
            return Result<std::unordered_set<std::size_t>, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }

        auto children = adjacency_list.at(vertex_id).outgoing_edges
        | std::views::transform([this](const auto& edge_id) {
            return edge_index.at(edge_id).to_id;
          });

        auto parents = adjacency_list.at(vertex_id).incoming_edges
        | std::views::transform([this](const auto& edge_id) {
            return edge_index.at(edge_id).from_id;
          });

        std::unordered_set<std::size_t> neighbours {children.begin(), children.end()};
        neighbours.insert(parents.begin(), parents.end());

        return Result<std::unordered_set<std::size_t>, ErrorType>::success(std::move(neighbours));
    }

    template <Hashable V, Hashable E> Result<std::unordered_set<std::size_t>, ErrorType> DirectedGraph<V, E>::get_outgoing_edges(const std::size_t& vertex_id) const {
        if (!adjacency_list.contains(vertex_id)) {
            return Result<std::unordered_set<std::size_t>, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }
        auto children {adjacency_list.at(vertex_id).outgoing_edges};
        return Result<std::unordered_set<std::size_t>, ErrorType>::success(std::move(children));
    }

    template <Hashable V, Hashable E> Result<std::unordered_set<std::size_t>, ErrorType> DirectedGraph<V, E>::get_incoming_edges(const std::size_t& vertex_id) const {
        if (!adjacency_list.contains(vertex_id)) {
            return Result<std::unordered_set<std::size_t>, ErrorType>::error(ErrorType::ABSENT_VERTX);
        }
        auto children {adjacency_list.at(vertex_id).incoming_edges};
        return Result<std::unordered_set<std::size_t>, ErrorType>::success(std::move(children));
    }
}
