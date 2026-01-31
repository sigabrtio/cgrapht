#define CATCH_CONFIG_MAIN

#include <algorithm>
#include <catch2/catch_test_macros.hpp>

#include "cgrapht/default_edge.hpp"
#include "cgrapht/graph.hpp"

struct Vertex final{
    std::size_t id {};
    std::string name {};

    bool operator==(const Vertex&) const = default;
};

template<>
struct std::hash<Vertex> {
    std::size_t operator()(const Vertex& v) const noexcept {
        return v.id;
    }
};

TEST_CASE("Test vertex manipulation") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I insert a vertex") {

            auto v1 = my_graph.add_vertex(Vertex{1, "One"});
            auto v2 = my_graph.add_vertex(Vertex {2, "Two"});

            THEN("I should be able to retrieve it") {

                auto vertices_range = my_graph.get_vertices();
                std::vector<Vertex> vertices {vertices_range.begin(), vertices_range.end()};
                REQUIRE(2 == vertices.size());
                REQUIRE(std::ranges::find(vertices, Vertex{1, "One"}) != vertices.end());
                REQUIRE(std::ranges::find(vertices, Vertex{2, "Two"}) != vertices.end());

                REQUIRE(Vertex {1, "One"} == my_graph.get_vertex(v1.get_ok()).get_ok());
                REQUIRE(Vertex {2, "Two"} == my_graph.get_vertex(v2.get_ok()).get_ok());

                AND_WHEN("I delete some vertices") {
                    REQUIRE(my_graph.delete_vertex(v1.get_ok()).is_ok());

                    THEN("That vertex should be gone") {
                        auto vertices_range_now = my_graph.get_vertices();
                        std::vector<Vertex> vertices_now {vertices_range_now.begin(), vertices_range_now.end()};
                        REQUIRE(1 == vertices_now.size());
                        REQUIRE(Vertex {2, "Two"} == vertices_now[0]);

                        AND_WHEN("I delete the last vertex") {
                           REQUIRE(my_graph.delete_vertex(2).is_ok());

                           THEN("The graph should be empty") {
                               REQUIRE(my_graph.get_vertices().empty());
                           }
                        }
                    }
                }
            }
        }

    }
}
