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

TEST_CASE("Test edges manipulation") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I insert insert some vertices") {

            std::size_t v1 {my_graph.add_vertex(Vertex{1, "One"}).get_ok()};
            std::size_t v2 {my_graph.add_vertex(Vertex{2, "Two"}).get_ok()};
            std::size_t v3 {my_graph.add_vertex(Vertex{3, "Three"}).get_ok()};
            std::size_t v4 {my_graph.add_vertex(Vertex{4, "Four"}).get_ok()};

            AND_WHEN("I add an edge")  {

                auto edge_add_result = my_graph.add_edge(v1, v2, cgrapht::DefaultEdge(123));

                THEN("I should be able to retrieve it") {

                    REQUIRE(edge_add_result.is_ok());

                    auto edges = my_graph.get_edges();

                    REQUIRE(1 == edges.size());
                    REQUIRE(v1 == (*edges.begin()).from_id);
                    REQUIRE(v2 == (*edges.begin()).to_id);
                    REQUIRE(cgrapht::DefaultEdge(123) == (*edges.begin()).edge);


                    REQUIRE(v1 == my_graph.get_edge(edge_add_result.get_ok()).get_ok().from_id);
                    REQUIRE(v2 == my_graph.get_edge(edge_add_result.get_ok()).get_ok().to_id);
                    REQUIRE(cgrapht::DefaultEdge(123) == my_graph.get_edge(edge_add_result.get_ok()).get_ok().edge);

                    AND_WHEN("I add the same edge again.") {

                        auto edge_add_result_again = my_graph.add_edge(v1, v2, cgrapht::DefaultEdge(123));

                        THEN("The result should effectively be a no op") {

                            REQUIRE(edge_add_result_again.is_ok());

                            auto edges_again = my_graph.get_edges();

                            REQUIRE(1 == edges_again.size());
                            REQUIRE(v1 == (*edges_again.begin()).from_id);
                            REQUIRE(v2 == (*edges_again.begin()).to_id);
                            REQUIRE(cgrapht::DefaultEdge(123) == (*edges_again.begin()).edge);


                            REQUIRE(v1 == my_graph.get_edge(edge_add_result_again.get_ok()).get_ok().from_id);
                            REQUIRE(v2 == my_graph.get_edge(edge_add_result_again.get_ok()).get_ok().to_id);
                            REQUIRE(cgrapht::DefaultEdge(123) == my_graph.get_edge(edge_add_result_again.get_ok()).get_ok().edge);


                            AND_WHEN("I add even more edges") {

                                auto ids = std::vector {
                                    my_graph.add_edge(v1, v2, cgrapht::DefaultEdge(123)),
                                    my_graph.add_edge(v2, v3, cgrapht::DefaultEdge(124)),
                                    my_graph.add_edge(v1, v3, cgrapht::DefaultEdge(125)),
                                    my_graph.add_edge(v2, v4, cgrapht::DefaultEdge(126))
                                } | std::views::transform([](auto res) { return res.get_ok();});

                                REQUIRE(4 == my_graph.get_edges().size());
                                REQUIRE(std::ranges::find(ids, 123) != ids.end());
                                REQUIRE(std::ranges::find(ids, 124) != ids.end());
                                REQUIRE(std::ranges::find(ids, 125) != ids.end());
                                REQUIRE(std::ranges::find(ids, 126) != ids.end());

                                THEN("I should be able to retrieve them") {

                                    auto retrieved_edges {my_graph.get_edges()};
                                    REQUIRE(std::ranges::find(retrieved_edges, cgrapht::Edge<cgrapht::DefaultEdge>(v1, v2, cgrapht::DefaultEdge(123))) != retrieved_edges.end());
                                    REQUIRE(std::ranges::find(retrieved_edges, cgrapht::Edge<cgrapht::DefaultEdge>(v2, v3, cgrapht::DefaultEdge(124))) != retrieved_edges.end());
                                    REQUIRE(std::ranges::find(retrieved_edges, cgrapht::Edge<cgrapht::DefaultEdge>(v1, v3, cgrapht::DefaultEdge(125))) != retrieved_edges.end());
                                    REQUIRE(std::ranges::find(retrieved_edges, cgrapht::Edge<cgrapht::DefaultEdge>(v2, v4, cgrapht::DefaultEdge(126))) != retrieved_edges.end());

                                    AND_WHEN("I delete some edges") {

                                        auto res = my_graph.delete_edge(123);
                                        REQUIRE(res.is_ok());
                                        REQUIRE(ids[0] == res.get_ok());

                                        THEN("The edge I deleted should be gone") {

                                            auto retrieved_edges_again {my_graph.get_edges()};
                                            REQUIRE(3 == retrieved_edges_again.size());
                                            REQUIRE(std::ranges::find(retrieved_edges_again, cgrapht::Edge<cgrapht::DefaultEdge>(1, 2, cgrapht::DefaultEdge(123))) == retrieved_edges_again.end());
                                            REQUIRE(std::ranges::find(retrieved_edges_again, cgrapht::Edge<cgrapht::DefaultEdge>(2, 3, cgrapht::DefaultEdge(124))) != retrieved_edges_again.end());
                                            REQUIRE(std::ranges::find(retrieved_edges_again, cgrapht::Edge<cgrapht::DefaultEdge>(1, 3, cgrapht::DefaultEdge(125))) != retrieved_edges_again.end());
                                            REQUIRE(std::ranges::find(retrieved_edges_again, cgrapht::Edge<cgrapht::DefaultEdge>(2, 4, cgrapht::DefaultEdge(126))) != retrieved_edges_again.end());

                                            AND_WHEN("I delete the last edge")
                                            {
                                                std::vector delete_results {
                                                    my_graph.delete_edge(124),
                                                    my_graph.delete_edge(125),
                                                    my_graph.delete_edge(126)
                                                };
                                                REQUIRE(delete_results[0].is_ok());
                                                REQUIRE(delete_results[1].is_ok());
                                                REQUIRE(delete_results[2].is_ok());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}