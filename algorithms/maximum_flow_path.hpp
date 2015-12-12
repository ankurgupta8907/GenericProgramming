//
// Created by Ankur Gupta on 10/24/15.
//

#ifndef GENERICPROGRAMMINGBGL_MAXIMUM_FLOW_PATH_HPP
#define GENERICPROGRAMMINGBGL_MAXIMUM_FLOW_PATH_HPP

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>

using namespace boost;


template <typename Graph, typename P, typename T, typename R>
void maximum_flow_path(Graph& g,
                       typename graph_traits<Graph>::vertex_descriptor s,
                       const bgl_named_params<P, T, R>& params) {
    typedef typename property_traits<typename property_map<Graph, edge_weight_t>::type>::value_type WeightDataType;
    dijkstra_shortest_paths(g,
                            s,
                            params.distance_compare(std::greater<WeightDataType>()).
                                    distance_combine([](WeightDataType a, WeightDataType b) { return std::min(a, b); }).
                                    distance_inf(WeightDataType()).
                                    distance_zero(std::numeric_limits<WeightDataType>::max())
    );
}

#endif // GENERICPROGRAMMINGBGL_MAXIMUM_FLOW_PATH_HPP