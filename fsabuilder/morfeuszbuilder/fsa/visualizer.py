'''
Created on Oct 20, 2013

@author: mlenart
'''

import networkx as nx
import matplotlib.pyplot as plt

class Visualizer(object):

    def __init__(self):
        pass
    
    def visualize(self, fsa, charLabels=True):
        G = nx.DiGraph()
        allStates = list(reversed(list(fsa.initialState.dfs(set()))))
        edgeLabelsMap = {}
        nodeLabelsMap = {}
        for idx, state in enumerate(allStates):
            G.add_node(idx, offset=state.offset)
            for c, targetState in state.transitionsMap.iteritems():
                G.add_edge(idx, allStates.index(targetState))
                label = (chr(c) if c <= 127 else '%') if charLabels \
                    else c
                edgeLabelsMap[(idx, allStates.index(targetState))] = label
            nodeLabelsMap[idx] = state.offset if not state.isAccepting() else state.encodedData + '(' + str(state.offset) + ')'
        pos=nx.shell_layout(G)
#         pos=nx.random_layout(G)
        nx.draw_networkx_nodes(G,
                               pos, 
                               nodelist=list([allStates.index(s) for s in allStates if not s.isAccepting()]),
                               node_shape='s',
                               node_color='w')
        nx.draw_networkx_nodes(G,
                               pos, 
                               nodelist=list([allStates.index(s) for s in allStates if s.isAccepting()]),
                               node_shape='s')
#         nx.draw_networkx_nodes(G, pos, nodelist=list([allStates.index(s) for s in allStates if s.isFinal()])), )
        nx.draw_networkx_edges(G, pos, edgelist=edgeLabelsMap.keys())
        nx.draw_networkx_edge_labels(G, pos, edge_labels = edgeLabelsMap)
        nx.draw_networkx_labels(G, pos, labels=nodeLabelsMap)
        plt.axis('off')
        plt.draw()
        plt.show()
#         plt.savefig('filename.png')
        print 'done'
