#define CATCH_CONFIG_MAIN

#include <algorithm>
#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "headers/default_edge.hpp"
#include "headers/graph.hpp"

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

TEST_CASE("Neighbouring edge functions happy case") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;
        const std::size_t v1 {my_graph.add_vertex(Vertex{1, "One"}).get_ok()};
        const std::size_t v2 {my_graph.add_vertex(Vertex{2, "Two"}).get_ok()};
        const std::size_t v3 {my_graph.add_vertex(Vertex{3, "Three"}).get_ok()};
        const std::size_t v4 {my_graph.add_vertex(Vertex{4, "Four"}).get_ok()};
        const std::size_t v5 {my_graph.add_vertex(Vertex{5, "Five"}).get_ok()};

        my_graph.add_edge(v2, v1, cgrapht::DefaultEdge(21));
        my_graph.add_edge(v3, v1, cgrapht::DefaultEdge(31));
        my_graph.add_edge(v4, v2, cgrapht::DefaultEdge(42));
        my_graph.add_edge(v5, v2, cgrapht::DefaultEdge(52));

        WHEN("I fetch the outgoing edges of some vertices.") {

            auto res_v1 {my_graph.get_outgoing_edges(v1)};
            auto res_v2 {my_graph.get_outgoing_edges(v2)};
            auto res_v3 {my_graph.get_outgoing_edges(v3)};
            auto res_v4 {my_graph.get_outgoing_edges(v4)};
            auto res_v5 {my_graph.get_outgoing_edges(v5)};

            THEN("I get the outgoing edges of the said vertices") {

                REQUIRE(res_v1.is_ok());
                REQUIRE(res_v2.is_ok());
                REQUIRE(res_v3.is_ok());
                REQUIRE(res_v4.is_ok());
                REQUIRE(res_v5.is_ok());

                REQUIRE(res_v1.get_ok().empty());
                REQUIRE(res_v2.get_ok().size() == 1);
                REQUIRE(res_v3.get_ok().size() == 1);
                REQUIRE(res_v4.get_ok().size() == 1);
                REQUIRE(res_v5.get_ok().size() == 1);

                REQUIRE(res_v2.get_ok().contains(21));
                REQUIRE(res_v3.get_ok().contains(31));
                REQUIRE(res_v4.get_ok().contains(42));
                REQUIRE(res_v5.get_ok().contains(52));

                REQUIRE(my_graph.get_edge(21).get_ok().from_id == v2);
                REQUIRE(my_graph.get_edge(21).get_ok().to_id == v1);

                REQUIRE(my_graph.get_edge(31).get_ok().from_id == v3);
                REQUIRE(my_graph.get_edge(31).get_ok().to_id == v1);

                REQUIRE(my_graph.get_edge(42).get_ok().from_id == v4);
                REQUIRE(my_graph.get_edge(42).get_ok().to_id == v2);

                REQUIRE(my_graph.get_edge(52).get_ok().from_id == v5);
                REQUIRE(my_graph.get_edge(52).get_ok().to_id == v2);
            }
        }

        WHEN("I fetch the incomming edges of some vertices") {

            auto res_v1 {my_graph.get_incoming_edges(v1)};
            auto res_v2 {my_graph.get_incoming_edges(v2)};
            auto res_v3 {my_graph.get_incoming_edges(v3)};
            auto res_v4 {my_graph.get_incoming_edges(v4)};
            auto res_v5 {my_graph.get_incoming_edges(v5)};

            THEN("I get the incoming edges to the said vertices") {

                REQUIRE(res_v1.is_ok());
                REQUIRE(res_v2.is_ok());
                REQUIRE(res_v3.is_ok());
                REQUIRE(res_v4.is_ok());
                REQUIRE(res_v5.is_ok());

                REQUIRE(res_v1.get_ok().size() == 2);
                REQUIRE(res_v2.get_ok().size() == 2);
                REQUIRE(res_v3.get_ok().empty());
                REQUIRE(res_v4.get_ok().empty());
                REQUIRE(res_v5.get_ok().empty());

                REQUIRE(res_v1.get_ok().contains(21));
                REQUIRE(res_v1.get_ok().contains(31));
                REQUIRE(res_v2.get_ok().contains(42));
                REQUIRE(res_v2.get_ok().contains(52));
            }
        }
    }
}

TEST_CASE("Neighbouring edges edge case (no pun intended)") {

    GIVEN("I have a graph") {
        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;
        const std::size_t v1 {my_graph.add_vertex(Vertex{1, "One"}).get_ok()};
        const std::size_t v2 {my_graph.add_vertex(Vertex{2, "Two"}).get_ok()};
        const std::size_t v3 {my_graph.add_vertex(Vertex{3, "Three"}).get_ok()};
        const std::size_t v4 {my_graph.add_vertex(Vertex{4, "Four"}).get_ok()};
        const std::size_t v5 {my_graph.add_vertex(Vertex{5, "Five"}).get_ok()};
        const std::size_t v6 {my_graph.add_vertex(Vertex{6, "Six"}).get_ok()};

        my_graph.add_edge(v2, v1, cgrapht::DefaultEdge(21));
        my_graph.add_edge(v3, v1, cgrapht::DefaultEdge(31));
        my_graph.add_edge(v4, v2, cgrapht::DefaultEdge(42));
        my_graph.add_edge(v5, v2, cgrapht::DefaultEdge(52));

        WHEN("I fetch the outgoing edges of some non existent vertices") {

            auto res_v1 {my_graph.get_outgoing_edges(2001)};

            THEN("The result should be an error") {

                REQUIRE(!res_v1.is_ok());
            }
        }

        WHEN("I fetch the edges of an unconnected vertex") {

            auto res_v1 {my_graph.get_incoming_edges(v6)};
            auto res_v2 {my_graph.get_outgoing_edges(v6)};

            THEN("I should get back an empty list") {

                REQUIRE(res_v1.get_ok().empty());
                REQUIRE(res_v2.get_ok().empty());
            }
        }
    }

}