#define CATCH_CONFIG_MAIN

#include <algorithm>
#include <iostream>
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

TEST_CASE("Add edge between non existent vertices") {
    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN ("I attempt tgo insert an edge into this empty graph") {

            auto res = my_graph.add_edge(1, 2, cgrapht::DefaultEdge(123));

            THEN("It should be a failure.") {

                REQUIRE(!res.is_ok());
                REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res.get_error());\

                AND_WHEN("I insert insert some vertices") {
                    std::size_t vid_1 = my_graph.add_vertex(Vertex{1, "One"}).get_ok();
                    std::size_t vid_2 = my_graph.add_vertex(Vertex{2, "Two"}).get_ok();

                    AND_WHEN("I add an edge where one the vertices is missing.") {

                        auto res1 = my_graph.add_edge(vid_1, 5, cgrapht::DefaultEdge(123));
                        auto res2 = my_graph.add_edge(5, vid_1, cgrapht::DefaultEdge(123));

                        THEN("Both should fail.") {

                            REQUIRE(!res1.is_ok());
                            REQUIRE(!res2.is_ok());
                            REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res1.get_error());
                            REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res2.get_error());

                            AND_WHEN("I insert an edge with both vertices missing") {

                                auto res3 = my_graph.add_edge(5, 6, cgrapht::DefaultEdge(123));

                                THEN("It should fail.") {

                                    REQUIRE(!res3.is_ok());
                                    REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res3.get_error());

                                    AND_WHEN("I insert the missing vertices and then try the same operation again.") {
                                        std::size_t vid_5 = my_graph.add_vertex(Vertex{5, "Five"}).get_ok();
                                        std::size_t vid_6 = my_graph.add_vertex(Vertex{6, "Six"}).get_ok();

                                        auto res4 = my_graph.add_edge(vid_5, vid_6, cgrapht::DefaultEdge(123));
                                        auto res5 = my_graph.add_edge(vid_1, vid_5, cgrapht::DefaultEdge(124));
                                        auto res6 = my_graph.add_edge(vid_5, vid_2, cgrapht::DefaultEdge(125));

                                        THEN("It should succeed.") {

                                            REQUIRE(res4.is_ok());
                                            REQUIRE(res5.is_ok());
                                            REQUIRE(res6.is_ok());

                                            AND_THEN("I should be able to retrieve all edges.") {

                                                auto edges = my_graph.get_edges();
                                                REQUIRE(3 == edges.size());
                                                REQUIRE(std::ranges::find(edges, cgrapht::Edge<cgrapht::DefaultEdge>(vid_1, vid_5, cgrapht::DefaultEdge(124))) != edges.end());
                                                REQUIRE(std::ranges::find(edges, cgrapht::Edge<cgrapht::DefaultEdge>(vid_5, vid_2, cgrapht::DefaultEdge(125))) != edges.end());
                                                REQUIRE(std::ranges::find(edges, cgrapht::Edge<cgrapht::DefaultEdge>(vid_5, vid_6, cgrapht::DefaultEdge(123))) != edges.end());
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

TEST_CASE("Add same edge between different vertices") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I insert insert some vertices") {
            std::size_t vid_1 = my_graph.add_vertex(Vertex{1, "One"}).get_ok();
            std::size_t vid_2 = my_graph.add_vertex(Vertex{2, "Two"}).get_ok();
            std::size_t vid_3 = my_graph.add_vertex(Vertex{3, "Three"}).get_ok();

            AND_WHEN("I add an edge") {

                auto res = my_graph.add_edge(vid_1, vid_2, cgrapht::DefaultEdge(123));
                REQUIRE(res.is_ok());

                AND_WHEN("I add the same edge again.") {

                    auto res_again = my_graph.add_edge(vid_1, vid_2, cgrapht::DefaultEdge(123));

                    THEN("The result should be the same as the first one.") {

                        REQUIRE(res_again.is_ok());
                        REQUIRE(res.get_ok() == res_again.get_ok());

                        AND_WHEN("I add the same edge again but between different vertices") {

                            auto res_again_again = my_graph.add_edge(vid_2, vid_3, cgrapht::DefaultEdge(123));

                            THEN("The result should be an error") {

                                REQUIRE(!res_again_again.is_ok());
                                REQUIRE(cgrapht::ErrorType::EDGE_ALREADY_EXISTS == res_again_again.get_error());
                            }
                        }
                    }
                }
            }
        }
    }
}