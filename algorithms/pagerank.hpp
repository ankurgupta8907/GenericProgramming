//
// Created by Ankur Gupta on 10/24/15.
//

#ifndef GENERICPROGRAMMINGBGL_PAGERANK_HPP
#define GENERICPROGRAMMINGBGL_PAGERANK_HPP

#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <numeric>

using namespace boost;

// Vertex property to store page rank.
enum vertex_pagerank_t { vertex_pagerank };
namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, pagerank);
}

/**
* The parameters to the algorithm are:
*    1) Graph g : The graph.
*    2) IndexMap indexMap : indexMap maps each vertex to a value between 0 and (vertex list size - 1).
*    3) RankValueType beta : Fraction of value given to regular jumps via transition matrix vs random teleport.
*    4) RankValueType allowedDiff : Value of allowed diff in vector of pagerank values between two iterations.
*/
template<typename Graph, typename RankValueType, typename IndexMap>
void pagerank(Graph& g, IndexMap indexMap, RankValueType beta, RankValueType allowedDiff) {
    typedef typename graph_traits<Graph>::vertex_iterator VertexIterator;
    typedef typename graph_traits<Graph>::in_edge_iterator InEdgeIterator;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename property_map<Graph, vertex_pagerank_t>::type PageRankMap;
    typedef typename property_traits<PageRankMap>::value_type PageRankValueType;


    // Compare two vectors of page rank to determine if total difference of page rank in two vectors in within
    // threshold allowedDiff.
    auto compare = [&g, &allowedDiff, &indexMap](const std::vector<PageRankValueType>& a1,
                                                 const std::vector<PageRankValueType>& a2) {
        PageRankValueType diff;
        VertexIterator ui, ui_end;
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            diff += abs(a1[get(indexMap, *ui)] - a2[get(indexMap, *ui)]);
        }
        return diff > allowedDiff;
    };

    // Number of vertices.
    int N = num_vertices(g);


    // Pagerank vector.
    std::vector<PageRankValueType> pageRankMap(N);

    // Temporary pagerank vector to calculate new pagerank in each iteration.
    std::vector<PageRankValueType> tempPageRankMap(N);

    // PageRank of value one. The initialization is supposed to give a value of zero. ++ operator is used to increment
    // the value to one.
    PageRankValueType one; ++one;

    // Assign the initial pagerank.
    VertexIterator ui, ui_end;
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        pageRankMap[get(indexMap, *ui)] = one/N;
    }

    // Keep iterating until the difference of total page rank between two successive iterations is less than allowed
    // threshold.
    do {
        std::copy(pageRankMap.begin(), pageRankMap.end(), tempPageRankMap.begin());
        // Adjust the page rank of each vertex in each iteration.
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            InEdgeIterator ei, ei_end;
            PageRankValueType temp;
            for (boost::tie(ei, ei_end) = in_edges(*ui, g); ei != ei_end; ++ei) {
                Vertex sourceVertex = source(*ei, g);
                temp += tempPageRankMap[get(indexMap, sourceVertex)] / out_degree(sourceVertex, g);
            }
            temp = temp * beta;
            pageRankMap[get(indexMap, *ui)] = temp;
        }

        // Calculate the total of page rank value.
        PageRankValueType S;
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            S += pageRankMap[get(indexMap, *ui)];
        }

        // Distribute the leaked value of page rank equally to all nodes.
        for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
            pageRankMap[get(indexMap, *ui)] += (one - S)/N;
        }
    }
    while (compare(tempPageRankMap, pageRankMap));

    // Putting the page rank values in the vertex property.
    PageRankMap outPageRankMap = get(vertex_pagerank, g);
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        put(outPageRankMap, *ui, pageRankMap[get(indexMap, *ui)]);
    }
}

#endif //GENERICPROGRAMMINGBGL_PAGERANK_HPP
