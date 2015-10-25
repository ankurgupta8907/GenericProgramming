//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <fstream>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>

using namespace boost;

template <typename Graph, typename P, typename T, typename R>
void maximum_flow_algorithm(Graph& g,
                                typename graph_traits<Graph>::vertex_descriptor s,
                                const bgl_named_params<P, T, R>& params) {
    typedef typename property_traits<typename property_map<Graph, edge_weight_t>::type>::value_type WT;

//    dummy_property_map p_map;
//    typedef decltype(choose_param(get_param(params, vertex_distance), p_map)) DistanceMap;
//    std::cout<< typeid(property_map<Graph, edge_weight_t>).name() << std::endl;
//    typedef typename property_traits<DistanceMap>::value_type DT;

    dijkstra_shortest_paths(g, s, params.distance_compare(std::greater<WT>()).
            distance_combine([](WT a, WT b) { return std::min(a, b); }).
            distance_inf(WT()).
            distance_zero(std::numeric_limits<WT>::max()));

}

int main(int, char *[])
{
    typedef adjacency_list < listS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
    typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef std::pair<int, int> Edge;

    const int num_nodes = 5;
    enum nodes { A, B, C, D, E };
    char name[] = "ABCDE";
    Edge edge_array[] = {
            Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
            Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
    };
    int weights[] = { 5, 2, 1, 2, 7, 3, 1, 1, 1 };
    int num_arcs = sizeof(edge_array) / sizeof(Edge);
    graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
    property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
    std::vector<vertex_descriptor> p(num_vertices(g));
    std::vector<int> d(num_vertices(g));
    vertex_descriptor s = vertex(A, g);

    maximum_flow_algorithm(g, s,
                            predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
                                    distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g)))
                                    );

    std::cout << "distances and parents:" << std::endl;
    graph_traits < graph_t >::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
        std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
        std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::
        endl;
    }
    std::cout << std::endl;

    std::ofstream dot_file("figs/dijkstra-eg.dot");

    dot_file << "digraph D {\n"
    << "  rankdir=LR\n"
    << "  size=\"4,3\"\n"
    << "  ratio=\"fill\"\n"
    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

    graph_traits < graph_t >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
        graph_traits < graph_t >::edge_descriptor e = *ei;
        graph_traits < graph_t >::vertex_descriptor
                u = source(e, g), v = target(e, g);
        dot_file << name[u] << " -> " << name[v]
        << "[label=\"" << get(weightmap, e) << "\"";
        if (p[v] == u)
            dot_file << ", color=\"black\"";
        else
            dot_file << ", color=\"grey\"";
        dot_file << "]";
    }
    dot_file << "}";
    return EXIT_SUCCESS;
}