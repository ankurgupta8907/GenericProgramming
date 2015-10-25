//=======================================================================
// Copyright 2015 Ankur Gupta
//
//=======================================================================
#include <boost/config.hpp>
#include <iostream>

#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>
#include <list>
#include <numeric>
#include <deque>
#include <initializer_list>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/topological_sort.hpp>

using namespace std;
using namespace boost;

int main(int, char *[]) {

    const char* tasks[] = {
            "pick up kids from school",
            "buy groceries (and snacks)",
            "get cash at ATM",
            "drop off kids at soccer practice",
            "cook dinner",
            "pick up kids from soccer",
            "eat dinner" };
    const int n_tasks = sizeof(tasks) / sizeof(char*);


    std::vector< std::list<int> > g(n_tasks);
    g[0].push_back(3);
    g[1].push_back(3);
    g[1].push_back(4);
    g[2].push_back(1);
    g[3].push_back(5);
    g[4].push_back(6);
    g[5].push_back(6);

    std::deque<int> topo_order;
    boost::topological_sort(g,
                            std::front_inserter(topo_order),
                            vertex_index_map(boost::identity_property_map()));
    for (std::deque<int>::iterator i = topo_order.begin();
         i != topo_order.end(); ++i) {
        std::cout << tasks[*i] << std::endl;
    }


}