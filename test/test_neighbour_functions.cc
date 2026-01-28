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

TEST_CASE("Neighbour functions happy case") {

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

        WHEN("I get children of v1 (top level)") {

            auto children = my_graph.get_children(v1);

            THEN("The operation should succeed and I should get back a set with nothing in it.") {

                REQUIRE(children.is_ok());
                REQUIRE(children.get_ok().empty());

                AND_WHEN("I get the parents of v1") {

                    auto parents = my_graph.get_parents(v1);

                    THEN("The operation should succeed and I should get back a set with v2 and v3 in it.") {

                        REQUIRE(parents.is_ok());
                        REQUIRE(parents.get_ok().size() == 2);
                        REQUIRE(std::ranges::find(parents.get_ok(), v2) != parents.get_ok().end());
                        REQUIRE(std::ranges::find(parents.get_ok(), v3) != parents.get_ok().end());

                        AND_WHEN("I get all neighbours of v1.") {

                            auto neighbours = my_graph.get_neighbours(v1);

                            REQUIRE(neighbours.is_ok());
                            REQUIRE(neighbours.get_ok().size() == 2);
                            REQUIRE(std::ranges::find(neighbours.get_ok(), v2) != neighbours.get_ok().end());
                            REQUIRE(std::ranges::find(neighbours.get_ok(), v3) != neighbours.get_ok().end());

                        }
                    }
                }
            }
        }

        WHEN("I get children of v2 (mid level with both parents and children)") {

            auto children = my_graph.get_children(v2);

            THEN("The operation should succeed and I should get back v1.") {

                REQUIRE(children.is_ok());
                REQUIRE(1 == children.get_ok().size());
                REQUIRE(v1 == *children.get_ok().begin());

                AND_WHEN("I get the parents of v1") {

                    auto parents = my_graph.get_parents(v2);

                    THEN("The operation should succeed and I should get back a set with v2 and v3 in it.") {

                        REQUIRE(parents.is_ok());
                        REQUIRE(parents.get_ok().size() == 2);
                        REQUIRE(std::ranges::find(parents.get_ok(), v4) != parents.get_ok().end());
                        REQUIRE(std::ranges::find(parents.get_ok(), v5) != parents.get_ok().end());

                        AND_WHEN("I get all neighbours of v2.") {

                            auto neighbours = my_graph.get_neighbours(v2);

                            THEN("I should correctly get bacl ALL neighbour IDs of v2 (both children and parents).") {

                                REQUIRE(neighbours.is_ok());
                                REQUIRE(neighbours.get_ok().size() == 3);
                                REQUIRE(std::ranges::find(neighbours.get_ok(), v4) != neighbours.get_ok().end());
                                REQUIRE(std::ranges::find(neighbours.get_ok(), v5) != neighbours.get_ok().end());
                                REQUIRE(std::ranges::find(neighbours.get_ok(), v1) != neighbours.get_ok().end());
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("Neighbour functions edge cases") {

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

        WHEN("I get the neighbours of vN (Non existent vertex") {

            auto result1 = my_graph.get_parents(1000);
            auto result2 = my_graph.get_children(1000);
            auto result3 = my_graph.get_neighbours(1000);


            THEN("I should get an error") {

                REQUIRE(!result1.is_ok());
                REQUIRE(!result2.is_ok());
                REQUIRE(!result3.is_ok());

                REQUIRE(result1.get_error() == cgrapht::ErrorType::ABSENT_VERTX);
                REQUIRE(result2.get_error() == cgrapht::ErrorType::ABSENT_VERTX);
                REQUIRE(result3.get_error() == cgrapht::ErrorType::ABSENT_VERTX);

            }
        }

        WHEN("I get the neighbours of an unconnected vertex") {

            auto result1 = my_graph.get_parents(v6);
            auto result2 = my_graph.get_children(v6);
            auto result3 = my_graph.get_neighbours(v6);

            THEN("I should get back an empty list") {

                REQUIRE(result1.is_ok());
                REQUIRE(result2.is_ok());
                REQUIRE(result3.is_ok());

                REQUIRE(result1.get_ok().empty());
                REQUIRE(result2.get_ok().empty());
                REQUIRE(result3.get_ok().empty());
            }
        }
    }
}