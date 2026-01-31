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

TEST_CASE("Adding the same vertex multiple times should be a no op.") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I insert the same vertex multiple times") {

            auto res1 = my_graph.add_vertex(Vertex{1, "One"});
            auto res2 = my_graph.add_vertex(Vertex{1, "One"});

            THEN("The graph should have only one vertex") {

                REQUIRE(my_graph.get_vertices().size() == 1);
                REQUIRE(res1.is_ok());
                REQUIRE(res2.is_ok());
                REQUIRE(res1.get_ok() == res2.get_ok());
            }
        }
    }
}

TEST_CASE("Attempting to delete a vertex multiple times.") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I add a vertex") {

            auto ress_add = my_graph.add_vertex(Vertex{1, "One"});

            AND_WHEN("I attempt to delete it multiple times") {

                auto res1 = my_graph.delete_vertex(ress_add.get_ok());
                auto res2 = my_graph.delete_vertex(ress_add.get_ok());

                THEN("The result should be the OK followed by error.") {

                    REQUIRE(res1.is_ok());
                    REQUIRE(!res2.is_ok());
                    REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res2.get_error());
                }
            }
        }
    }
}

TEST_CASE("Deleting a non existent vertex.") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I attempt to delete a vertex off of the empty graph") {

            auto res = my_graph.delete_vertex(1);

            THEN("The result should be an error.") {

                REQUIRE(!res.is_ok());
                REQUIRE(cgrapht::ErrorType::ABSENT_VERTX == res.get_error());

                AND_WHEN("I add this missing vertex.") {

                    my_graph.add_vertex(Vertex{1, "One"});
                    my_graph.add_vertex(Vertex{2, "Two"});
                    my_graph.add_vertex(Vertex{3, "Three"});

                    THEN("The delete should succeed this time.") {

                        REQUIRE(my_graph.delete_vertex(1).is_ok());
                        REQUIRE(my_graph.get_vertices().size() == 2);
                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex {1, "One"}) == my_graph.get_vertices().end());
                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex {2, "Two"}) != my_graph.get_vertices().end());
                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex {3, "Three"}) != my_graph.get_vertices().end());

                        AND_WHEN("I delete another non existent vertex.") {

                            THEN("It should fail.") {

                                REQUIRE(!my_graph.delete_vertex(88).is_ok());
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("Attempting to delete a vertex that is connected to edges.") {

    GIVEN("I have a graph") {

        cgrapht::DirectedGraph<Vertex, cgrapht::DefaultEdge> my_graph;

        WHEN("I add some vertices and edges") {

            std::size_t v1_id {my_graph.add_vertex(Vertex{1, "One"}).get_ok()};
            std::size_t v2_id {my_graph.add_vertex(Vertex{2, "Two"}).get_ok()};
            std::size_t v3_id {my_graph.add_vertex(Vertex{3, "Three"}).get_ok()};

            std::size_t e1_id {my_graph.add_edge(v1_id, v2_id, cgrapht::DefaultEdge(123)).get_ok()};
            std::size_t e2_id {my_graph.add_edge(v2_id, v3_id, cgrapht::DefaultEdge(124)).get_ok()};
            std::size_t e3_id {my_graph.add_edge(v3_id, v1_id, cgrapht::DefaultEdge(125)).get_ok()};

            AND_WHEN("I attempt to delete the vertex with edges") {

                auto res = my_graph.delete_vertex(v1_id);
                auto res2 = my_graph.delete_vertex(v2_id);

                THEN("The result should be an error.") {

                    REQUIRE(!res.is_ok());
                    REQUIRE(!res2.is_ok());
                    REQUIRE(cgrapht::ErrorType::VERTEX_NOT_FREE == res.get_error());
                    REQUIRE(cgrapht::ErrorType::VERTEX_NOT_FREE == res2.get_error());

                    AND_WHEN("I delete the incoming edges.") {

                        my_graph.delete_edge(e2_id);

                        AND_WHEN("I attempt to delete the vertex again.") {

                            std::cout << "Deleting vertex " << v3_id << std::endl;
                            auto res3 = my_graph.delete_vertex(v3_id);

                            THEN("It should fail again because there is an outgoing edge.") {

                                REQUIRE(!res3.is_ok());
                                REQUIRE(cgrapht::ErrorType::VERTEX_NOT_FREE == res3.get_error());

                                AND_WHEN("I delete the outgoing edge as well and try again.") {

                                    my_graph.delete_edge(e3_id);

                                    THEN("The vertex should be deleted this time.") {

                                        REQUIRE(my_graph.delete_vertex(v3_id).is_ok());
                                        REQUIRE(my_graph.get_vertices().size() == 2);
                                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex {1, "One"}) != my_graph.get_vertices().end());
                                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex {2, "Two"}) != my_graph.get_vertices().end());
                                        REQUIRE(std::ranges::find(my_graph.get_vertices(), Vertex{3, "Three"}) == my_graph.get_vertices().end());

                                        AND_WHEN("I delete the remaining edges and vertices.") {

                                            my_graph.delete_edge(e1_id);

                                            auto res4 = my_graph.delete_vertex(v1_id);
                                            auto res5 = my_graph.delete_vertex(v2_id);

                                            THEN("All operations should succeed and my graph should be empty.") {

                                                REQUIRE(res4.is_ok());
                                                REQUIRE(res5.is_ok());
                                                REQUIRE(my_graph.get_vertices().empty());
                                                REQUIRE(my_graph.get_edges().empty());
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