//
// Created by Ankur Gupta on 10/24/15.
//

#include "test_pagerank.h"
#include <iostream>

#include "../algorithms/pagerank.hpp"
#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
//#include <boost/graph/edge_list.hpp>
#include <vector>
#include <numeric>

using namespace std;
using namespace boost;

int main(int, char *[]) {

    // Using set as vertexList.
    typedef adjacency_list < setS, setS, bidirectionalS, property < vertex_pagerank_t, double >> Graph;

    Graph g(4);
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename graph_traits<Graph>::vertex_iterator VertexIterator;
    VertexIterator ui, ui_end;
    boost::tie(ui, ui_end) = vertices(g);

    Vertex one = *ui++;
    Vertex two = *ui++;
    Vertex three = *ui++;
    Vertex four = *ui++;

    add_edge(one, three, g);
    add_edge(one, two, g);
    add_edge(one, four, g);
    add_edge(two, three, g);
    add_edge(two, four, g);
    add_edge(three, one, g);
    add_edge(four, three, g);
    add_edge(four, one, g);

    typedef Graph::vertex_descriptor NodeID;
    typedef std::map<NodeID, size_t> IndexMap;
    IndexMap mapIndex;
    associative_property_map<IndexMap> propmapIndex(mapIndex);
    int i=0;
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        put(propmapIndex, *ui, i++);
    }
    pagerank(g, propmapIndex, 1.0, 0.00001);

    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        cout << "Vertex " << get(propmapIndex, *ui) << " has pagerank " <<
                get(get(vertex_pagerank, g), *ui) << "." << endl;
    }


    return 0;
}