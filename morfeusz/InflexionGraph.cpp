
#include <string>
#include <cassert>
#include <climits>
#include <vector>
#include <iostream>
#include <algorithm>
#include "utils.hpp"
#include "InflexionGraph.hpp"

using namespace std;

namespace morfeusz {

void InflexionGraph::addStartEdge(const Edge& e) {
    if (this->graph.empty()) {
        assert(this->node2ChunkStartPtr.empty());
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.textStartPtr);
    }
    assert(this->node2ChunkStartPtr[0] == e.chunk.textStartPtr);
    this->graph[0].push_back(e);
}

void InflexionGraph::addMiddleEdge(unsigned int startNode, const Edge& e) {
    assert(startNode < e.nextNode);
    assert(startNode == this->graph.size());
    if (startNode == this->graph.size()) {
        this->graph.push_back(vector<Edge>());
        this->node2ChunkStartPtr.push_back(e.chunk.textStartPtr);
    }
    this->graph[startNode].push_back(e);
}

static inline bool chunkIsAtFront(
        const InterpretedChunk& chunk,
        const std::vector<InterpretedChunk>& path) {
    unsigned int i;
    for (i = 0; i < path.size() - 1 && path[i].orthWasShifted; i++) {
    }
    assert(!path[i].orthWasShifted);
    return &chunk == &(path[i]);
}

static inline bool chunkIsAtBack(
        const InterpretedChunk& chunk,
        const std::vector<InterpretedChunk>& path) {
    return &chunk == &(path.back());
}

static inline bool chunkIsTheOnlyOne(
        const InterpretedChunk& chunk,
        const std::vector<InterpretedChunk>& path) {
    return chunkIsAtFront(chunk, path) && chunkIsAtBack(chunk, path);
}

void InflexionGraph::addPath(const std::vector<InterpretedChunk>& path, bool weak) {
    
    if (weak && !this->empty() && !this->onlyWeakPaths) {
        return;
    }
    else if (this->onlyWeakPaths && !weak) {
        this->graph.resize(0);
        this->node2ChunkStartPtr.resize(0);
        this->onlyWeakPaths = false;
    }
    for (unsigned int i = 0; i < path.size(); i++) {
        const InterpretedChunk& chunk = path[i];
        if (!chunk.orthWasShifted) {
            if (chunkIsTheOnlyOne(chunk, path)) {
                Edge e = {chunk, UINT_MAX};
                this->addStartEdge(e);
            }
            else if (chunkIsAtFront(chunk, path)) {
                Edge e = {chunk, this->graph.empty() ? 1 : (unsigned int) this->graph.size()};
                this->addStartEdge(e);
            }
            else if (chunkIsAtBack(chunk, path)) {
                Edge e = {chunk, UINT_MAX};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            }
            else {
                Edge e = {chunk, (unsigned int) this->graph.size() + 1};
                this->addMiddleEdge((unsigned int) this->graph.size(), e);
            }
        }
    }
}

bool InflexionGraph::canMergeNodes(unsigned int node1, unsigned int node2) {
    return this->node2ChunkStartPtr[node1] == this->node2ChunkStartPtr[node2]
            && this->getPossiblePaths(node1) == this->getPossiblePaths(node2);
}

set<InflexionGraph::Path> InflexionGraph::getPossiblePaths(unsigned int node) {
    if (node == UINT_MAX || node == this->graph.size() - 1) {
        set<InflexionGraph::Path> res;
        return res;
    }
    else {
        set<InflexionGraph::Path> res;
        vector<Edge>& edges = this->graph.at(node);
        for (unsigned int i = 0; i < edges.size(); i++) {
            Edge& e = edges[i];
            InflexionGraph::PathElement pathElem(e.chunk.textStartPtr, e.chunk.segmentType);
            if (e.nextNode != this->graph.size()) {
                set<Path> possiblePaths = this->getPossiblePaths(e.nextNode);
                vector<Path> nextPaths(possiblePaths.begin(), possiblePaths.end());
                vector<Path>::iterator it;
                for (it = nextPaths.begin(); it != nextPaths.end(); ++it) {
                    (*it).insert(pathElem);
                }
                res.insert(nextPaths.begin(), nextPaths.end());
            }
        }
        return res;
    }
}

static bool containsEqualEdge(const vector<InflexionGraph::Edge>& edges, const InflexionGraph::Edge& e) {
    for (unsigned int i = 0; i < edges.size(); i++) {
        const InflexionGraph::Edge& e1 = edges[i];
        if (e1.chunk.textStartPtr == e.chunk.textStartPtr
                && e1.chunk.textStartPtr == e.chunk.textStartPtr
                && e1.chunk.textEndPtr == e.chunk.textEndPtr
                && e1.chunk.segmentType == e.chunk.segmentType
                && e1.nextNode == e.nextNode
                && e1.chunk.interpsGroupPtr == e.chunk.interpsGroupPtr) {
            return true;
        }
    }
    return false;
}

void InflexionGraph::redirectEdges(unsigned int fromNode, unsigned int toNode) {
    for (unsigned int node = 0; node < fromNode; node++) {
        vector<Edge>& edges = this->graph[node];
        vector<Edge>::iterator edgesIt = edges.begin();
        while (edgesIt != edges.end()) {
            Edge& oldEdge = *edgesIt;
            if (oldEdge.nextNode == fromNode) {
                Edge newEdge = {oldEdge.chunk, toNode};
                if (!containsEqualEdge(edges, newEdge)) {
                    // if newEdge is not in edges, redirect oldEdge
                    // so it becomes newEdge
                    oldEdge.nextNode = toNode;
                }
                else {
                    // if newEdge is already there, just remove old edge
                    edges.erase(edgesIt);
                }
            }
            else {
                ++edgesIt;
            }
        }
    }
}

void InflexionGraph::doRemoveNode(unsigned int node) {
    for (unsigned int i = node + 1; i < this->graph.size(); i++) {
        redirectEdges(i, i - 1);
        this->graph[i - 1] = this->graph[i];
        this->node2ChunkStartPtr[i - 1] = this->node2ChunkStartPtr[i];
    }
    this->graph.pop_back();
    this->node2ChunkStartPtr.pop_back();
}

void InflexionGraph::doMergeNodes(unsigned int node1, unsigned int node2) {
    if (node1 > node2) {
        doMergeNodes(node2, node1);
    }
    else {
        // node1 < node2
        for (unsigned int i = 0; i < this->graph[node2].size(); i++) {
            Edge& e = this->graph[node2][i];
            if (!containsEqualEdge(graph[node1], e)) {
                this->graph[node1].push_back(e);
            }
        }
        this->redirectEdges(node2, node1);
        this->doRemoveNode(node2);
    }
}

bool InflexionGraph::tryToMergeTwoNodes() {
    for (unsigned int node1 = 0; node1 < this->graph.size(); node1++) {
        for (unsigned int node2 = (unsigned int) this->graph.size() - 1; node2 > node1; node2--) {
            if (this->canMergeNodes(node1, node2)) {
                this->doMergeNodes(node1, node2);
                return true;
            }
        }
    }
    return false;
}

void InflexionGraph::minimizeGraph() {
    if (this->graph.size() > 2) {
        //        debugGraph(this->graph);
        while (this->tryToMergeTwoNodes()) {
                        //debugGraph(this->graph);
        }
    }
}

bool InflexionGraph::empty() const {
    return this->graph.empty();
}

void InflexionGraph::repairLastNodeNumbers() {
    for (unsigned int i = 0; i < this->graph.size(); i++) {
        vector<Edge>& edges = this->graph[i];
        for (unsigned int j = 0; j < edges.size(); j++) {
            Edge& e = edges[j];
            if (e.nextNode == UINT_MAX) {
                e.nextNode = (unsigned int) this->graph.size();
            }
        }
    }
}

static vector<unsigned int> createIdentityNodesMap(long unsigned int n) {
    vector<unsigned int> res(n);
    for (unsigned int i = 0; i < n; i++) {
        res[i] = i;
    }
    return res;
}

class TopologicalComparator {
public:
    TopologicalComparator(vector< const char* > node2ChunkStartPtr)
    : node2ChunkStartPtr(node2ChunkStartPtr) {}
    
    bool operator()(unsigned int i, unsigned int j) {
        return node2ChunkStartPtr[i] < node2ChunkStartPtr[j];
    }
private:
    vector< const char* > node2ChunkStartPtr;
};

// XXX this is a bit dirty
// fixes problem with "radem," (incorrect node numbers when inflexion graph is NOT a tree)
// and "Z rozdrażnienieniem:)"
void InflexionGraph::sortNodeNumbersTopologically() {
    vector<unsigned int> nodesTopologicallySorted(createIdentityNodesMap(this->graph.size()));
    TopologicalComparator comparator(this->node2ChunkStartPtr);
    sort(nodesTopologicallySorted.begin(), nodesTopologicallySorted.end(), comparator);
    vector<unsigned int> oldNode2NewNode(nodesTopologicallySorted.size());
    for (unsigned int newNode = 0; newNode < nodesTopologicallySorted.size(); newNode++) {
        unsigned int oldNode = nodesTopologicallySorted[newNode];
        oldNode2NewNode[oldNode] = newNode;
    }
    // swap pointers in edges
    for (unsigned int node = 0; node < this->graph.size(); node++) {
        for (unsigned int edgeIdx = 0; edgeIdx < this->graph[node].size(); edgeIdx++) {
            InflexionGraph::Edge& edge = this->graph[node][edgeIdx];
            if (edge.nextNode < this->graph.size()) { // don't change UINT_MAX nodes (outside current graph)
                edge.nextNode = oldNode2NewNode[edge.nextNode];
            }
        }
    }
    
    // swap nodes
    vector< vector<Edge> > graphCopy(this->graph);
    vector< const char* > node2ChunkStartPtrCopy(this->node2ChunkStartPtr);
    for (unsigned int oldNode = 0; oldNode < this->graph.size(); oldNode++) {
        unsigned int newNode = oldNode2NewNode[oldNode];
        this->graph[newNode] = graphCopy[oldNode];
        this->node2ChunkStartPtr[newNode] = node2ChunkStartPtrCopy[oldNode];
    }
}

const vector< vector<InflexionGraph::Edge> >& InflexionGraph::getTheGraph() {
    minimizeGraph();
    if (this->graph.size() > 2) {
        sortNodeNumbersTopologically();
    }
    repairLastNodeNumbers();
    return this->graph;
}

void InflexionGraph::clear() {
    graph.resize(0);
    node2ChunkStartPtr.resize(0);
    onlyWeakPaths = true;
}

}
