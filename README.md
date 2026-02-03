# cgraph\<T>

> A modern, header-only C++20 graph library for in-memory directed graphs

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Bazel](https://img.shields.io/badge/Build-Bazel-green.svg)](https://bazel.build/)

## Features

- 🚀 **Header-only** - No compilation required, just include and use
- 🎯 **Type-safe** - Leverages C++20 concepts for compile-time type checking
- 💪 **Generic** - Works with any hashable vertex and edge types
- 📊 **Directed graphs** - Efficient representation with adjacency lists
- 🔍 **ID-based queries** - Hash-based vertex and edge identification
- ⚡ **Modern C++** - Uses C++20 features: concepts, ranges, and more
- 🛡️ **Error handling** - Type-safe `Result` type instead of exceptions for graph operations

## Quick Start

### Basic Usage

```cpp
#include "cgrapht/graph.hpp"
#include "cgrapht/default_edge.hpp"

using namespace cgrapht;

int main() {
    // Create a graph with int vertices and DefaultEdge edges
    DirectedGraph<int, DefaultEdge> graph;
    
    // Add vertices
    auto v1_result = graph.add_vertex(1);
    auto v2_result = graph.add_vertex(2);
    auto v3_result = graph.add_vertex(3);
    
    if (v1_result.is_ok() && v2_result.is_ok() && v3_result.is_ok()) {
        auto v1_id = v1_result.get_ok();
        auto v2_id = v2_result.get_ok();
        auto v3_id = v3_result.get_ok();
        
        // Add edges
        graph.add_edge(v1_id, v2_id, DefaultEdge{1});
        graph.add_edge(v2_id, v3_id, DefaultEdge{2});
        
        // Query neighbors
        auto children = graph.get_children(v2_id);
        if (children.is_ok()) {
            // v3_id will be in the children set
        }
    }
    
    return 0;
}
```

### Custom Types

```cpp
struct City {
    std::string name;
    int population;
    
    bool operator==(const City& other) const {
        return name == other.name;
    }
};

// Make City hashable
template<>
struct std::hash<City> {
    std::size_t operator()(const City& city) const {
        return std::hash<std::string>{}(city.name);
    }
};

struct Road {
    std::string name;
    double distance_km;
    
    bool operator==(const Road& other) const {
        return name == other.name;
    }
};

// Make Road hashable
template<>
struct std::hash<Road> {
    std::size_t operator()(const Road& road) const {
        return std::hash<std::string>{}(road.name);
    }
};

// Use in graph
DirectedGraph<City, Road> city_graph;
auto seattle = city_graph.add_vertex(City{"Seattle", 750000}).get_ok();
auto portland = city_graph.add_vertex(City{"Portland", 650000}).get_ok();
city_graph.add_edge(seattle, portland, Road{"I-5", 280.5});
```

## Installation

### Using Bazel (Recommended)

Add cgrapht to your `MODULE.bazel`:

```python
bazel_dep(name = "cgrapht", version = "1.0.0")
```

Then use it in your `BUILD.bazel`:

```python
load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "my_app",
    srcs = ["main.cpp"],
    deps = ["@cgrapht//:cgrapht"],
)
```

### Manual Installation

Since cgrapht is header-only, you can simply copy the `headers/` directory to your include path:

```bash
# Clone the repository
git clone https://github.com/sigabrtio/cgrapht.git

# Copy headers to your project
cp -r cgrapht/headers/* /path/to/your/project/include/
```

Then include in your code:

```cpp
#include "cgrapht/graph.hpp"
```

## API Overview

### Graph Operations

#### Vertex Management
- `add_vertex(const V& v)` - Add a vertex, returns vertex ID
- `delete_vertex(vertex_id)` - Remove a vertex (only if no incident edges)
- `get_vertex(vertex_id)` - Retrieve vertex data by ID

#### Edge Management
- `add_edge(from_id, to_id, const E& e)` - Add a directed edge
- `delete_edge(edge_id)` - Remove an edge
- `get_edge(edge_id)` - Retrieve edge data by ID

#### Graph Queries
- `get_children(vertex_id)` - Get outgoing neighbors
- `get_parents(vertex_id)` - Get incoming neighbors
- `get_neighbours(vertex_id)` - Get all adjacent vertices
- `get_outgoing_edges(vertex_id)` - Get outgoing edge IDs
- `get_incoming_edges(vertex_id)` - Get incoming edge IDs

#### Iteration
- `get_vertices()` - View all vertex payloads (C++20 range)
- `get_edges()` - View all edge records (C++20 range)

### Error Handling

All operations return `Result<T, ErrorType>`:

```cpp
auto result = graph.add_vertex(42);

if (result.is_ok()) {
    auto vertex_id = result.get_ok();
    // Use vertex_id
} else {
    auto error = result.get_error();
    // Handle error
}
```

Error types:
- `INVALID_ARGUMENT`
- `ABSENT_VERTEX`
- `ABSENT_EDGE`
- `EDGE_ALREADY_EXISTS`
- `VERTEX_NOT_FREE` (vertex has incident edges)

## Building and Testing

### Requirements
- C++20 compatible compiler (GCC 10+, Clang 13+, MSVC 19.29+)
- Bazel 9.0.0 or later

### Build
```bash
bazel build //...
```

### Run Tests
```bash
bazel test //...
```

### Run Specific Test
```bash
bazel test //test:cgrapht_unit_tests
```

## Documentation

Full API documentation is available at: [https://sigabrtio.github.io/cgrapht/](https://sigabrtio.github.io/cgrapht/)

Generate documentation locally using Doxygen:
```bash
doxygen Doxyfile
# Open docs/html/index.html in your browser
```

## Design Principles

### ID-Based Architecture
cgrapht uses hash-based IDs instead of direct object references. This design:
- Prevents dangling references when vertices/edges are removed
- Enables efficient lookups via hash maps
- Provides stable identifiers across graph modifications

### Type Requirements
Both vertex (`V`) and edge (`E`) types must satisfy the `Hashable` concept:
- Must be hashable via `std::hash<T>`
- Must implement `operator==` for equality comparison


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by [JGraphT](https://jgrapht.org/) - A Java graph library
- Built with modern C++20 features and best practices

## Contact

- **Author**: Amartya
- **GitHub**: [@sigabrtio](https://github.com/sigabrtio)
- **Issues**: [GitHub Issues](https://github.com/sigabrtio/cgrapht/issues)

---

**Note**: This library is under active development. APIs may change before the 1.0 release.