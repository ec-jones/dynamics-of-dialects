import community
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import re
import os

xs, ys = np.loadtxt('data/communities0/local_overlap.dat', delimiter=' ', unpack = True)

fig, ax = plt.subplots(1, 1)
ax.set_xscale('log')
ax.set_ylim([0, 1])
ax.set_xlabel('Games per player')
ax.set_ylabel('Local overlap')
ax.plot(xs / 100, ys)
fig.savefig('communities_local.png')
plt.show()

modularity = []
T = []
count = []
for dir in os.listdir('data/communities0/dump'):
   m = re.match(r"([0-9]+)\.dat", dir)
   if m != None:
      fh = open("data/communities0/dump/" + dir, 'r')
      G = nx.read_weighted_edgelist(fh)
      fh.close()
      partition = community.best_partition(G)
      modularity.append(community.modularity(partition, G))
      count.append(max(partition.values()))
      T.append(int(m.groups(0)[0]))

[T0, modularity0, count0] = [list(t) for t in zip(*sorted(zip(T, modularity, count)))]

modularity = []
T = []
count = []
for dir in os.listdir('data/communities1/dump'):
   m = re.match(r"([0-9]+)\.dat", dir)
   if m != None:
      fh = open("data/communities1/dump/" + dir, 'r')
      G = nx.read_weighted_edgelist(fh)
      fh.close()
      partition = community.best_partition(G)
      modularity.append(community.modularity(partition, G))
      count.append(max(partition.values()))
      T.append(int(m.groups(0)[0]))


[T1, modularity1, count1] = [list(t) for t in zip(*sorted(zip(T, modularity, count)))]

modularity = []
T = []
count = []
for dir in os.listdir('data/communities2/dump'):
   m = re.match(r"([0-9]+)\.dat", dir)
   if m != None:
      fh = open("data/communities2/dump/" + dir, 'r')
      G = nx.read_weighted_edgelist(fh)
      fh.close()
      partition = community.best_partition(G)
      modularity.append(community.modularity(partition, G))
      count.append(max(partition.values()))
      T.append(int(m.groups(0)[0]))

[T2, modularity2, count2] = [list(t) for t in zip(*sorted(zip(T, modularity, count)))]

T = (np.array(T0) + np.array(T1) + np.array(T2)) / 3
modularity = (np.array(modularity0) + np.array(modularity1) + np.array(modularity2)) / 3
count = (np.array(count0) + np.array(count1) + np.array(count2)) / 3

fig, ax = plt.subplots(1, 1)
ax.set_xscale('log')
ax.set_ylim([-0.1, 1])
ax.set_ylabel('Modularity')
ax.set_xlabel('Games per player')
ax.plot(T / 100, modularity)
fig.savefig('communities_modularity.png')
plt.show()

fig, ax = plt.subplots(1, 1)
ax.set_xscale('log')
ax.set_ylabel('Communities')
ax.set_xlabel('Games per player')
ax.plot(T / 100, count)
fig.savefig('communities_communities.png')
plt.show()