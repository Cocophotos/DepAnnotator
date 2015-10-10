#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

#ifndef Q_MOC_RUN
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <boost/range/concepts.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/lexical_cast.hpp>

#include <map>

#endif

#include <QtDebug>


/* To access properties of edges and vertices with
   a property map */
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };
namespace boost {
BOOST_INSTALL_PROPERTY(vertex, properties);
BOOST_INSTALL_PROPERTY(edge, properties);
}
/******************************************************/

using namespace boost;
using namespace boost::unordered;
using namespace std;

/**
 * Graph template around Boost Graph Library to ease the use of this library
 * This template takes Node properties and Edge propeties in parameters.
 *
 * This properties can be structure or class, but please MAKE SURE you have
 * a id variable publicly accessible into the structure of the class and a copy constructor.
 * This will be use to give access to directly to nodes and edges.
 */
template <typename N, typename E>
class Graph
{
public:

    typedef adjacency_list<listS, listS, bidirectionalS, N, E> GraphContainer;

    typedef typename graph_traits<GraphContainer>::vertex_descriptor Node;
    typedef typename graph_traits<GraphContainer>::edge_descriptor Edge;

    typedef typename graph_traits<GraphContainer>::vertex_iterator node_iter;
    typedef typename graph_traits<GraphContainer>::edge_iterator edge_iter;
    typedef typename graph_traits<GraphContainer>::adjacency_iterator adjacency_iter;

    typedef typename graph_traits<GraphContainer>::out_edge_iterator out_edge_iter;
    typedef typename graph_traits<GraphContainer>::in_edge_iterator in_edge_iter;

    typedef typename graph_traits<GraphContainer>::degree_size_type degree_t;

    typedef pair<adjacency_iter, adjacency_iter> adjacency_node_range_t;

    typedef pair<out_edge_iter, out_edge_iter> out_edge_range_t;
    typedef pair<in_edge_iter, in_edge_iter> in_edge_range_t;

    typedef pair<node_iter, node_iter> node_range_t;
    typedef pair<edge_iter, edge_iter> edge_range_t;

    typedef typename map<unsigned, Node>::iterator ordered_node_iter;
    typedef pair<ordered_node_iter, ordered_node_iter> ordered_node_range_t;



    Graph()
    {
        count = 0;
    }

    virtual ~Graph(){}

    void clear()
    {
        graph.clear();
        edgesMap.clear();
        nodesMap.clear();
    }

    //Primitives

    /**
         * Add a new node into the graph structure
         * @param id : node's id (must be unique)
         * @param prop : node's properties
         * @return the new node (or the former one, if id exists)
         */
    const Node addNode(unsigned id, const N prop)
    {
        if(!existsNode(id))
        {
            Node n = add_vertex(prop, graph);
            nodesMap.insert(make_pair(id, n));
            return n;
        }
        else
        {
            return nodesMap[id];
        }
    }

    void removeNode(unsigned id)
    {
        Node& n = nodeById(id);

        out_edge_iter out_eit, out_edge_end;
        in_edge_iter in_eit, in_edge_end;

        for(tie(out_eit, out_edge_end) = out_edges(n, graph); out_eit != out_edge_end; ++out_eit)
            removeEdge( graph[*out_eit].id );

        for(tie(in_eit, in_edge_end) = in_edges(n, graph); in_eit != in_edge_end; ++in_eit)
            removeEdge( graph[*in_eit].id );

        remove_vertex(n, graph);
        nodesMap.erase(id);
    }

    void removeEdge(unsigned id)
    {
        Edge &e = edgeById(id);
        remove_edge(e, graph);
        edgesMap.erase(id);        
    }

    void removeEdge(const Node& src, const Node& tar)
    {
        bool found = false;
        out_edge_iter eit, edge_end;
        unsigned toRemove;
        for(tie(eit, edge_end) = out_edges(src, graph); eit != edge_end; ++eit)
        {
            Node _tar = target(*eit, graph);
            //If edge is found, ok delete it.
            if(nodeProperties(_tar).id == nodeProperties(tar).id)
            {
                toRemove = edgeProperties(*eit).id;
                break;
            }
        }
        if(found)
            removeEdge(toRemove);
    }

    void invertEdge(Edge& e)
    {
        Node src = source(e, graph);
        Node tar = target(e, graph);

        E prop(edgeProperties(e));
        removeEdge(prop.id);
        addEdge(nodeProperties(tar).id, nodeProperties(src).id, prop);
    }

    void invertEdge(unsigned eid)
    {
        Edge &e = edgeById(eid);
        invertEdge(e);
    }  

    const Edge addEdge(unsigned src, unsigned tar, const E prop)
    {
        Node &n_src = nodeById(src), &n_tar = nodeById(tar);
        if( !existsEdge(prop.id) )
        {
            pair<Edge, bool> e = add_edge(n_src, n_tar, prop, graph);
            edgesMap.insert(pair<unsigned, Edge>(prop.id, e.first));
            count++;
            return e.first;
        }
        else
            return edgesMap[prop.id];
    }

    //High-level methods

    Graph(const Graph<N,E> &other)
    {
        node_iter vb, ve;
        for(boost::tie(vb,ve) = boost::vertices(other.graph); vb!=ve; ++vb)
        {
            Node node = *vb;
            addNode(other.graph[node].id, other.graph[node]);

            out_edge_iter eb, ee;
            for(boost::tie(eb, ee) = boost::out_edges(node, other.graph); eb != ee; ++eb){
                Edge edge = *eb;
                Node target = boost::target(edge, other.graph);

                addNode(other.graph[target].id, other.graph[target]);
                addEdge(other.graph[node].id, other.graph[target].id, other.graph[edge]);
            }
        }
    }


    //Accessors

    //Properties accessors

    N& nodeProperties(unsigned id)
    {
        const Node& n = nodeById(id);
        return graph[n];
    }

    E& edgeProperties(unsigned id)
    {
        const Edge& e = edgeById(id);
        return graph[e];
    }

    const N& nodeProperties(const Node& n)
    {
        return graph[n];
    }

    const E& edgeProperties(const Edge &e)
    {
        return graph[e];
    }

    void setNodeProperties(unsigned id, const N prop){
        const Node& n = nodeById(id);
        graph[n] = prop;
    }

    void setEdgeProperties(unsigned id, const E prop){
        const Edge& e = edgeById(id);
        graph[e] = prop;
    }

    /////////////////////////////////////

    //Node accessors

    node_range_t nodes()
    {
        return vertices(graph);
    }

    ordered_node_range_t orderedNodes()
    {
        return make_pair(nodesMap.begin(), nodesMap.end());
    }

    adjacency_node_range_t adjacentNodes(unsigned id)
    {
        Node& n = nodeById(id);
        return adjacent_vertices(n, graph);
    }

    Node sourceNode(unsigned eid)
    {
        return sourceNode(edgeById(eid));
    }

    Node targetNode(unsigned eid)
    {
        return targetNode(edgeById(eid));
    }

    Node sourceNode(Edge& edge)
    {
        return source(edge, graph);
    }

    Node targetNode(Edge& edge)
    {
        return target(edge, graph);
    }

    unsigned nodesSize() const
    {
        return num_vertices(graph);
    }

    unsigned nodeDegree(unsigned id)
    {
        const Node& n = nodeById(id);
        return in_degree(n, graph) + out_degree(n, graph);
    }

    unsigned nodeOutDegree(unsigned id)
    {
        Node& n = nodeById(id);
        return out_degree(n, graph);
    }

    unsigned nodeInDegree(unsigned id)
    {
        Node& n = nodeById(id);
        return in_degree(n, graph);
    }

    ////////////////////////////////////////

    // Edge accessors

    out_edge_range_t outEdges(unsigned id)
    {
        return out_edges(nodeById(id), graph);
    }

    in_edge_range_t inEdges(unsigned id)
    {
        return in_edges(nodeById(id), graph);
    }

    pair<Edge, bool> edge(unsigned src, unsigned tar)
    {
        const Node &n_src = nodeById(src), &n_tar = nodeById(tar);
        return edge(n_src, n_tar, graph);
    }

    pair<Edge, bool> edge(const Node &n_src, const Node &n_tar)
    {
        return edge(n_src, n_tar, graph);
    }

    unsigned edgesSize()const
    {
        return edgesMap.size();
    }

    unsigned counter()const
    {
        return count*10;
    }

    ////////////////////////////////////////

    //-------------------
    // Helper functions
    //-------------------

    bool existsNode(unsigned id) const
    {
        if(nodesMap.find(id) != nodesMap.end())
            return true;
        else
            return false;
    }

    bool existsEdge(unsigned id) const
    {
        if(edgesMap.find(id) != edgesMap.end())
            return true;
        else
            return false;
    }

    ////////////////////////////////////////

protected:
    Node& nodeById(unsigned id)
    {
        typename map<unsigned, Node>::iterator node_it = nodesMap.find(id);
        if(node_it != nodesMap.end())
            return node_it->second;
        throw out_of_range("The requested node does not exist");
    }

    Edge& edgeById(unsigned id)
    {
        typename unordered_map<unsigned, Edge>::iterator edge_it = edgesMap.find(id);
        if(edge_it != edgesMap.end())
            return edge_it->second;
        throw out_of_range("The requested edge does not exist");
    }

protected:
    GraphContainer graph;
    map<unsigned, Node> nodesMap;
    unordered_map<unsigned, Edge> edgesMap;
    unsigned count;
};

#endif // ABSTRACTGRAPH_H

