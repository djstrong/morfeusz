/* 
 * File:   FlexionGraph.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:03
 */

#ifndef FLEXIONGRAPH_HPP
#define	FLEXIONGRAPH_HPP

#include <vector>
#include <set>
#include <utility>
#include "InterpretedChunk.hpp"

namespace morfeusz {

/**
 * This class build inflection graph (indexes the nodes, takes into account segments marked as "weak").
 * Takes care to make the number of nodes as little as possible.
 */
class InflexionGraph {
public:
    
    InflexionGraph(): graph(), node2ChunkStartPtr(), onlyWeakPaths(true) {
        
    }

    struct Edge {
        InterpretedChunk chunk;
        unsigned int nextNode;
    };
    
    /**
     * Adds new path to the graph.
     * 
     * @param path
     * @param weak
     */
    void addPath(const std::vector<InterpretedChunk>& path, bool weak);

    //    void getResults(const Tagset& tagset, const CharsetConverter& charsetConverter, std::vector<MorphInterpretation>& results);

    /**
     * Return current graph.
     * 
     * @return 
     */
    const std::vector< std::vector<InflexionGraph::Edge> >& getTheGraph();

    /**
     * True iff the graph is empty.
     * 
     * @return 
     */
    bool empty() const;
    
    /**
     * Clears the graph.
     */
    void clear();

private:

    typedef std::pair<const char*, int> PathElement;
    typedef std::set<PathElement> Path;

    /**
     * Adds an edge that starts a chunk.
     * 
     * @param e
     */
    void addStartEdge(const Edge& e);
    
    /**
     * Adds non-starting edge.
     * @param startNode
     * @param e
     */
    void addMiddleEdge(unsigned int startNode, const Edge& e);

    /**
     * Minimizes the graph so it contains as little number of nodes as possible.
     */
    void minimizeGraph();
    
    bool canMergeNodes(unsigned int node1, unsigned int node2);

    void doMergeNodes(unsigned int node1, unsigned int node2);

    bool tryToMergeTwoNodes();

    std::set<Path> getPossiblePaths(unsigned int node);

    void redirectEdges(unsigned int fromNode, unsigned int toNode);

    void doRemoveNode(unsigned int node);
    
    void repairLastNodeNumbers();
    
    void sortNodeNumbersTopologically();
    
    void repairOrthShifts();

    std::vector< std::vector<Edge> > graph;
    std::vector< const char* > node2ChunkStartPtr;
    bool onlyWeakPaths;
};

}

#endif	/* FLEXIONGRAPH_HPP */

