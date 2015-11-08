//
// Created by Ankur Gupta on 10/29/15.
//

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <vector>
#include <ostream>
#include <iostream>
#include <boost/graph/graphviz.hpp>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace boost;

template <typename NumPathsMap, typename Vertex>
class record_num_paths : public dfs_visitor<> {
public:
    record_num_paths(NumPathsMap map, Vertex d) : numPathsMap(map), dest(d) { }

    template <class Graph>
    void discover_vertex(Vertex v, Graph& g) {
        if (v == dest)
            put(numPathsMap, v, 1);
        else
            put(numPathsMap, v, 0);
    }

    template <class Graph>
    void finish_vertex(Vertex v, Graph& g) {
        typedef typename graph_traits<Graph>::adjacency_iterator AdjacencyIterator;
        AdjacencyIterator ui, ui_end;
        for (boost::tie(ui, ui_end) = adjacent_vertices(v, g); ui != ui_end; ++ui) {
            put(numPathsMap, v, get(numPathsMap, v) + get(numPathsMap, *ui));
        }
    }

protected:
    NumPathsMap numPathsMap;
    Vertex dest;
};


template <class NumPathsMap, class Vertex>
record_num_paths<NumPathsMap, Vertex>
make_num_paths_recorder(NumPathsMap map, Vertex dest) {
    return record_num_paths<NumPathsMap, Vertex>(map, dest);
}



template <typename VertexListGraph>
typename graph_traits<VertexListGraph>::edges_size_type
        path_count(VertexListGraph& G,
                   typename graph_traits<VertexListGraph>::vertex_descriptor source,
                   typename graph_traits<VertexListGraph>::vertex_descriptor target) {

    typedef typename graph_traits<VertexListGraph>::vertex_descriptor Vertex;
    typename graph_traits<VertexListGraph>::edges_size_type Edge_Size;
    typedef typename graph_traits<VertexListGraph>::vertex_iterator VertexIterator;
    vector<unsigned int> numPaths(num_vertices(G));
    depth_first_search(G, visitor(make_num_paths_recorder(&numPaths[0], target)).root_vertex(source));
    return numPaths.at(source);
}



int main() {
    typedef adjacency_list <vecS, vecS, directedS> Graph;

    Graph g(4);
    add_edge(0, 2, g);
    add_edge(0, 1, g);
    add_edge(0, 3, g);
    add_edge(1, 2, g);
    add_edge(1, 3, g);
    add_edge(3, 2, g);

//    cout << path_count(g, 0, 2) << endl;

    dynamic_properties dp;
    Graph gr(0);
    // Sample graph as an std::istream;
    std::istringstream
            gvgraph("digraph { graph [name=\"graphname\"]  a  c e [mass = 6.66] }");
    read_graphviz(gvgraph, gr, dp, "node_id");
}