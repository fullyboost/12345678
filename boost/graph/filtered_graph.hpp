//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================

#ifndef BOOST_FILTERED_GRAPH_HPP
#define BOOST_FILTERED_GRAPH_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/filter_iterator.hpp>

namespace boost {

  //=========================================================================
  // Some predicate classes.

  struct keep_all {
    template <typename T>
    bool operator()(const T&) const { return true; }
  };

  // Keep residual edges (used in maximum-flow algorithms).
  template <typename ResidualCapacityEdgeMap>
  struct is_residual_edge {
    is_residual_edge() { }
    is_residual_edge(ResidualCapacityEdgeMap rcap) : m_rcap(rcap) { }
    template <typename Edge>
    bool operator()(const Edge& e) const {
      return 0 < get(m_rcap, e);
    }
    ResidualCapacityEdgeMap m_rcap;
  };

  template <typename Set>
  struct is_in_subset {
    is_in_subset() : m_s(0) { }
    is_in_subset(const Set& s) : m_s(&s) { }

    template <typename Elt>
    bool operator()(const Elt& x) const {
      return set_contains(*m_s, x);
    }
    const Set* m_s;
  };

  template <typename Set>
  struct is_not_in_subset {
    is_not_in_subset() : m_s(0) { }
    is_not_in_subset(const Set& s) : m_s(&s) { }

    template <typename Elt>
    bool operator()(const Elt& x) const {
      return !set_contains(*m_s, x);
    }
    const Set* m_s;
  };
  
  namespace detail {

    template <typename EdgePredicate, typename VertexPredicate, typename Graph>
    struct out_edge_predicate {
      out_edge_predicate() { }
      out_edge_predicate(EdgePredicate ep, VertexPredicate vp, 
                         const Graph& g)
        : m_edge_pred(ep), m_vertex_pred(vp), m_g(&g) { }

      template <typename Edge>
      bool operator()(const Edge& e) const {
        return m_edge_pred(e) && m_vertex_pred(target(e, *m_g));
      }
      EdgePredicate m_edge_pred;
      VertexPredicate m_vertex_pred;
      const Graph* m_g;
    };

    template <typename EdgePredicate, typename VertexPredicate, typename Graph>
    struct in_edge_predicate {
      in_edge_predicate() { }
      in_edge_predicate(EdgePredicate ep, VertexPredicate vp, 
                         const Graph& g)
        : m_edge_pred(ep), m_vertex_pred(vp), m_g(&g) { }

      template <typename Edge>
      bool operator()(const Edge& e) const {
        return m_edge_pred(e) && m_vertex_pred(source(e, *m_g));
      }
      EdgePredicate m_edge_pred;
      VertexPredicate m_vertex_pred;
      const Graph* m_g;
    };

    template <typename EdgePredicate, typename VertexPredicate, typename Graph>
    struct edge_predicate {
      edge_predicate() { }
      edge_predicate(EdgePredicate ep, VertexPredicate vp, 
                     const Graph& g)
        : m_edge_pred(ep), m_vertex_pred(vp), m_g(&g) { }

      template <typename Edge>
      bool operator()(const Edge& e) const {
        return m_edge_pred(e)
          && m_vertex_pred(source(e, *m_g)) && m_vertex_pred(target(e, *m_g));
      }
      EdgePredicate m_edge_pred;
      VertexPredicate m_vertex_pred;
      const Graph* m_g;
    };

  } // namespace detail


  //===========================================================================
  // Filtered Graph

  struct filtered_graph_tag { };

  template <typename Graph, 
            typename EdgePredicate,
            typename VertexPredicate = keep_all>
  class filtered_graph {
    typedef graph_traits<Graph> Traits;
    typedef filtered_graph self;
  public:
    typedef Graph graph_type;
    typedef detail::out_edge_predicate<EdgePredicate, 
      VertexPredicate, self> OutEdgePred;
    typedef detail::in_edge_predicate<EdgePredicate, 
      VertexPredicate, self> InEdgePred;
    typedef detail::edge_predicate<EdgePredicate, 
      VertexPredicate, self> EdgePred;

    // Constructors
    filtered_graph(const Graph& g, EdgePredicate ep)
      : m_g(g), m_edge_pred(ep) { }

    filtered_graph(const Graph& g, EdgePredicate ep, VertexPredicate vp)
      : m_g(g), m_edge_pred(ep), m_vertex_pred(vp) { }

    // Graph requirements
    typedef typename Traits::vertex_descriptor          vertex_descriptor;
    typedef typename Traits::edge_descriptor            edge_descriptor;
    typedef typename Traits::directed_category          directed_category;
    typedef typename Traits::edge_parallel_category     edge_parallel_category;
    typedef typename Traits::traversal_category         traversal_category;

    // IncidenceGraph requirements
    typedef filter_iterator<
        OutEdgePred, typename Traits::out_edge_iterator
    > out_edge_iterator;
      
    typedef typename Traits::degree_size_type          degree_size_type;

    // AdjacencyGraph requirements
    typedef typename adjacency_iterator_generator<self,
      vertex_descriptor, out_edge_iterator>::type      adjacency_iterator;

    // BidirectionalGraph requirements
    typedef filter_iterator<
        InEdgePred, typename Traits::in_edge_iterator
    > in_edge_iterator;

    // VertexListGraph requirements
    typedef filter_iterator<
        VertexPredicate, typename Traits::vertex_iterator
    > vertex_iterator;
    typedef typename Traits::vertices_size_type        vertices_size_type;

    // EdgeListGraph requirements
    typedef filter_iterator<
        EdgePred, typename Traits::edge_iterator
    > edge_iterator;
    typedef typename Traits::edges_size_type           edges_size_type;

    typedef typename Graph::edge_property_type         edge_property_type;
    typedef typename Graph::vertex_property_type       vertex_property_type;
    typedef filtered_graph_tag graph_tag;

    //private:
    const Graph& m_g;
    EdgePredicate m_edge_pred;
    VertexPredicate m_vertex_pred;
  };

  //===========================================================================
  // Non-member functions for the Filtered Edge Graph

  // Helper functions
  template <typename Graph, typename EdgePredicate>
  inline filtered_graph<Graph, EdgePredicate>
  make_filtered_graph(Graph& g, EdgePredicate ep) {
    return filtered_graph<Graph, EdgePredicate>(g, ep);
  }
  template <typename Graph, typename EdgePredicate, typename VertexPredicate>
  inline filtered_graph<Graph, EdgePredicate, VertexPredicate>
  make_filtered_graph(Graph& g, EdgePredicate ep, VertexPredicate vp) {
    return filtered_graph<Graph, EdgePredicate, VertexPredicate>(g, ep, vp);
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::vertex_iterator,
            typename filtered_graph<G, EP, VP>::vertex_iterator>
  vertices(const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;    
    typename graph_traits<G>::vertex_iterator f, l;
    tie(f, l) = vertices(g.m_g);
    typedef typename Graph::vertex_iterator iter;
    return std::make_pair(iter(g.m_vertex_pred, f, l), 
                          iter(g.m_vertex_pred, l, l));
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::edge_iterator,
            typename filtered_graph<G, EP, VP>::edge_iterator>
  edges(const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;
    typename Graph::EdgePred pred(g.m_edge_pred, g.m_vertex_pred, g);
    typename graph_traits<G>::edge_iterator f, l;
    tie(f, l) = edges(g.m_g);
    typedef typename Graph::edge_iterator iter;
    return std::make_pair(iter(pred, f, l), iter(pred, l, l));
  }

  // An alternative for num_vertices() and num_edges() would be to
  // count the number in the filtered graph. This is problematic
  // because of the interaction with the vertex indices...  they would
  // no longer go from 0 to num_vertices(), which would cause trouble
  // for algorithms allocating property storage in an array. We could
  // try to create a mapping to new recalibrated indices, but I don't
  // see an efficient way to do this.
  //
  // However, the current solution is still unsatisfactory because
  // the following semantic constraints no longer hold:
  // tie(vi, viend) = vertices(g);
  // assert(std::distance(vi, viend) == num_vertices(g));

  template <typename G, typename EP, typename VP>  
  typename filtered_graph<G, EP, VP>::vertices_size_type
  num_vertices(const filtered_graph<G, EP, VP>& g) {
    return num_vertices(g.m_g);
  }

  template <typename G, typename EP, typename VP>  
  typename filtered_graph<G, EP, VP>::edges_size_type
  num_edges(const filtered_graph<G, EP, VP>& g) {
    return num_edges(g.m_g);
  }
  
  template <typename G, typename EP, typename VP>
  typename filtered_graph<G, EP, VP>::vertex_descriptor
  source(typename filtered_graph<G, EP, VP>::edge_descriptor e,
         const filtered_graph<G, EP, VP>& g)
  {
    return source(e, g.m_g);
  }

  template <typename G, typename EP, typename VP>
  typename filtered_graph<G, EP, VP>::vertex_descriptor
  target(typename filtered_graph<G, EP, VP>::edge_descriptor e,
         const filtered_graph<G, EP, VP>& g)
  {
    return target(e, g.m_g);
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::out_edge_iterator,
            typename filtered_graph<G, EP, VP>::out_edge_iterator>
  out_edges(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
            const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;
    typename Graph::OutEdgePred pred(g.m_edge_pred, g.m_vertex_pred, g);
    typedef typename Graph::out_edge_iterator iter;
    typename graph_traits<G>::out_edge_iterator f, l;
    tie(f, l) = out_edges(u, g.m_g);
    return std::make_pair(iter(pred, f, l), iter(pred, l, l));
  }

  template <typename G, typename EP, typename VP>
  typename filtered_graph<G, EP, VP>::degree_size_type
  out_degree(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
             const filtered_graph<G, EP, VP>& g)
  {
    typename filtered_graph<G, EP, VP>::degree_size_type n = 0;
    typename filtered_graph<G, EP, VP>::out_edge_iterator f, l;
    for (tie(f, l) = out_edges(u, g); f != l; ++f)
      ++n;
    return n;
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::adjacency_iterator,
            typename filtered_graph<G, EP, VP>::adjacency_iterator>
  adjacent_vertices(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
                    const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;
    typedef typename Graph::adjacency_iterator adjacency_iterator;
    typename Graph::out_edge_iterator f, l;
    tie(f, l) = out_edges(u, g);
    return std::make_pair(adjacency_iterator(f, const_cast<Graph*>(&g)),
                          adjacency_iterator(l, const_cast<Graph*>(&g)));
  }
  
  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::in_edge_iterator,
            typename filtered_graph<G, EP, VP>::in_edge_iterator>
  in_edges(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
            const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;
    typename Graph::InEdgePred pred(g.m_edge_pred, g.m_vertex_pred, g);
    typedef typename Graph::in_edge_iterator iter;
    typename graph_traits<G>::in_edge_iterator f, l;
    tie(f, l) = in_edges(u, g.m_g);
    return std::make_pair(iter(pred, f, l), iter(pred, l, l));
  }

  template <typename G, typename EP, typename VP>
  typename filtered_graph<G, EP, VP>::degree_size_type
  in_degree(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
             const filtered_graph<G, EP, VP>& g)
  {
    typename filtered_graph<G, EP, VP>::degree_size_type n = 0;
    typename filtered_graph<G, EP, VP>::in_edge_iterator f, l;
    for (tie(f, l) = in_edges(u, g); f != l; ++f)
      ++n;
    return n;
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::edge_descriptor, bool>
  edge(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
       typename filtered_graph<G, EP, VP>::vertex_descriptor v,
       const filtered_graph<G, EP, VP>& g)
  {
    typename graph_traits<G>::edge_descriptor e;
    bool exists;
    tie(e, exists) = edge(u, v, g.m_g);
    return std::make_pair(e, exists && g.m_edge_pred(e));
  }

  template <typename G, typename EP, typename VP>
  std::pair<typename filtered_graph<G, EP, VP>::out_edge_iterator,
            typename filtered_graph<G, EP, VP>::out_edge_iterator>
  edge_range(typename filtered_graph<G, EP, VP>::vertex_descriptor u,
             typename filtered_graph<G, EP, VP>::vertex_descriptor v,
             const filtered_graph<G, EP, VP>& g)
  {
    typedef filtered_graph<G, EP, VP> Graph;
    typename Graph::OutEdgePred pred(g.m_edge_pred, g.m_vertex_pred, g);
    typedef typename Graph::out_edge_iterator iter;
    typename graph_traits<G>::out_edge_iterator f, l;
    tie(f, l) = edge_range(u, v, g.m_g);
    return std::make_pair(iter(pred, f, l), iter(pred, l, l));
  }


  //===========================================================================
  // Property map

  namespace detail {
    struct filtered_graph_property_selector {
      template <class FilteredGraph, class Property, class Tag>
      struct bind_ {
        typedef typename FilteredGraph::graph_type Graph;
        typedef property_map<Graph, Tag> Map;
        typedef typename Map::type type;
        typedef typename Map::const_type const_type;
      };
    };    
  } // namespace detail

  template <>  
  struct vertex_property_selector<filtered_graph_tag> {
    typedef detail::filtered_graph_property_selector type;
  };
  template <>  
  struct edge_property_selector<filtered_graph_tag> {
    typedef detail::filtered_graph_property_selector type;
  };

  template <typename G, typename EP, typename VP, typename Property>
  typename property_map<G, Property>::type
  get(Property p, filtered_graph<G, EP, VP>& g)
  {
    return get(p, const_cast<G&>(g.m_g));
  }

  template <typename G, typename EP, typename VP,typename Property>
  typename property_map<G, Property>::const_type
  get(Property p, const filtered_graph<G, EP, VP>& g)
  {
    return get(p, (const G&)g.m_g);
  }

  template <typename G, typename EP, typename VP, typename Property,
            typename Key>
  typename property_map_value<G, Property>::type
  get(Property p, const filtered_graph<G, EP, VP>& g, const Key& k)
  {
    return get(p, (const G&)g.m_g, k);
  }

  template <typename G, typename EP, typename VP, typename Property, 
            typename Key, typename Value>
  void
  put(Property p, const filtered_graph<G, EP, VP>& g, const Key& k,
      const Value& val)
  {
    put(p, const_cast<G&>(g.m_g), k, val);
  }

  //===========================================================================
  // Some filtered subgraph specializations

  template <typename Graph, typename Set>
  struct vertex_subset_filter {
    typedef filtered_graph<Graph, keep_all, is_in_subset<Set> > type;
  };
  template <typename Graph, typename Set>
  inline typename vertex_subset_filter<Graph, Set>::type
  make_vertex_subset_filter(Graph& g, const Set& s) {
    typedef typename vertex_subset_filter<Graph, Set>::type Filter;
    is_in_subset<Set> p(s);
    return Filter(g, keep_all(), p);
  }

  template <typename Graph, typename Set>
  struct vertex_subset_compliment_filter {
    typedef filtered_graph<Graph, keep_all, is_not_in_subset<Set> > type;
  };
  template <typename Graph, typename Set>
  inline typename vertex_subset_compliment_filter<Graph, Set>::type
  make_vertex_subset_compliment_filter(Graph& g, const Set& s) {
    typedef typename vertex_subset_compliment_filter<Graph, Set>::type Filter;
    is_not_in_subset<Set> p(s);
    return Filter(g, keep_all(), p);
  }


} // namespace boost


#endif // BOOST_FILTERED_GRAPH_HPP
