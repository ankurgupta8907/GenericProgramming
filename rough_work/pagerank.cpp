//=======================================================================
// Copyright 2015 Ankur Gupta
//
//=======================================================================
#include <boost/config.hpp>
#include <iostream>

#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
//#include <boost/graph/edge_list.hpp>
#include <vector>
#include <numeric>

using namespace boost;

enum vertex_pagerank_t { vertex_pagerank };

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, pagerank);
}


template<typename Graph, typename RankValueType, typename IndexMap>
void pagerank(Graph& g, IndexMap indexMap, RankValueType beta, RankValueType allowedDiff) {
    typedef typename graph_traits<Graph>::vertex_iterator VertexIterator;
    typedef typename graph_traits<Graph>::in_edge_iterator InEdgeIterator;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename property_map<Graph, vertex_pagerank_t>::type PageRankMap;
    typedef typename property_traits<PageRankMap>::value_type PageRankValueType;

    auto compare = [&g, &allowedDiff, &indexMap](const std::vector<PageRankValueType>& a1,
                                                 const std::vector<PageRankValueType>& a2) {
        PageRankValueType diff = PageRankValueType();
        VertexIterator ui, ui_end;
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            diff += std::abs(a1[get(indexMap, *ui)] - a2[get(indexMap, *ui)]);
        }
        return diff > allowedDiff;
    };

    int N = num_vertices(g);


    std::vector<PageRankValueType> tempPageRankMap(N);
    std::vector<PageRankValueType> pageRankMap(N);

    PageRankValueType one; ++one;

    VertexIterator ui, ui_end;
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        pageRankMap[get(indexMap, *ui)] = one/N;
    }


    do {
        std::copy(pageRankMap.begin(), pageRankMap.end(), tempPageRankMap.begin());
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            InEdgeIterator ei, ei_end;
            PageRankValueType temp = PageRankValueType();
            for (boost::tie(ei, ei_end) = in_edges(*ui, g); ei != ei_end; ++ei) {
                Vertex sourceVertex = source(*ei, g);
                temp += tempPageRankMap[get(indexMap, sourceVertex)] / out_degree(sourceVertex, g);
            }
            temp *= beta;
            pageRankMap[get(indexMap, *ui)] = temp;
        }

        PageRankValueType S = PageRankValueType();
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            S += pageRankMap[get(indexMap, *ui)];
        }

        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            pageRankMap[get(indexMap, *ui)] += (one - S)/N;
        }
    }
    while (compare(tempPageRankMap, pageRankMap));

    PageRankMap outPageRankMap = get(vertex_pagerank, g);
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        put(outPageRankMap, *ui, pageRankMap[get(indexMap, *ui)]);
    }
}


int main(int, char *[]) {

    // Using set as vertexList.
    typedef adjacency_list < setS, setS, bidirectionalS, property < vertex_pagerank_t, double >> Graph;

    Graph g(4);
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename graph_traits<Graph>::vertex_iterator VertexIterator;
    VertexIterator ui, ui_end;
    boost::tie(ui, ui_end) = vertices(g);
//    add_edge(0, 2, g);
//    add_edge(0, 1, g);
//    add_edge(0, 3, g);
//    add_edge(1, 2, g);
//    add_edge(1, 3, g);
//    add_edge(2, 0, g);
//    add_edge(3, 2, g);
//    add_edge(3, 0, g);


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
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
    {
        put(propmapIndex, *ui, i++);
    }
    pagerank(g, propmapIndex, 1.0, 0.00001);



    // Using arrays.
//    std::vector< std::vector<int> > graph(7);
//    graph[0].push_back(3);
//    graph[1].push_back(3);
//    graph[1].push_back(4);
//    graph[2].push_back(1);
//    graph[3].push_back(5);
//    graph[4].push_back(6);
//    graph[5].push_back(6);
//
//    pagerank(graph, boost::identity_property_map(), 1.0, 0.00001);


//    enum { u, v, x, y, z, N };
//
//    typedef std::pair<int,int> E;
//    E edges[] = { E(u,y), E(u,x), E(u,v),
//                  E(v,u),
//                  E(x,y), E(x,v),
//                  E(y,v), E(y,z),
//                  E(z,u), E(z,x) };
//
//
//    typedef boost::edge_list<E*> Graph;
//    Graph g(edges, edges + sizeof(edges) / sizeof(E));
//
//    pagerank(g, boost::identity_property_map(), 1.0, 0.00001);

    return 0;
}