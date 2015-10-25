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

enum vertex_pagerank_t { vertex_pagerank };

namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, pagerank);
}

/**
 * TODO Documentation.
 */
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
            diff += abs(a1[get(indexMap, *ui)] - a2[get(indexMap, *ui)]);
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
            PageRankValueType temp;
            for (boost::tie(ei, ei_end) = in_edges(*ui, g); ei != ei_end; ++ei) {
                Vertex sourceVertex = source(*ei, g);
                temp += tempPageRankMap[get(indexMap, sourceVertex)] / out_degree(sourceVertex, g);
            }
            temp = temp * beta;
            pageRankMap[get(indexMap, *ui)] = temp;
        }

        PageRankValueType S;
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

#endif //GENERICPROGRAMMINGBGL_PAGERANK_HPP
