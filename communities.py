import community
import networkx as nx
import matplotlib.pyplot as plt
import re
import os

modularity = []
T = []
for dir in os.listdir('data/communities2'):
	m = re.match(r"dump_([0-9]+)\.dat", dir)
	if m != None:
		fh = open("data/communities2/" + dir, 'r')
		G = nx.read_weighted_edgelist(fh)
		fh.close()
		partition = community.best_partition(G)
		modularity.append(community.modularity(partition, G))
		T.append(int(m.groups(0)[0]))

[T, modularity] = [list(t) for t in zip(*sorted(zip(T, modularity)))]
fig, ax = plt.subplots(1, 1)
ax.set_xscale('log')
ax.set_ylim([-0.1, 1])
ax.plot(T, modularity)
plt.show()