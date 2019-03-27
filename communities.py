import community
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import re
import os

a = [0.15, 0.3, 0.45]
b = [0.15, 0.3, 0.45]

fig, axes = plt.subplots(3, 3)
plt.setp(axes.flat, xlabel='Games per player', ylabel='Modularity') #ylabel='Overlap') 

pad = 5

for ax, col in zip(axes[0], ['Lambda2: {}'.format(col) for col in b]):
   ax.annotate(col, xy=(0.5, 1), xytext=(0, pad),
               xycoords='axes fraction', textcoords='offset points',
               size='large', ha='center', va='baseline')

for ax, row in zip(axes[:,0], ['Lambda1: {}'.format(row) for row in a]):
   ax.annotate(row, xy=(0, 0.5), xytext=(-ax.yaxis.labelpad - pad, 0),
               xycoords=ax.yaxis.label, textcoords='offset points',
               size='large', ha='right', va='center')

fig.tight_layout()

for dir0 in os.listdir('study3'):
   m = re.match(r"communities_(0\.[0-9]+)_(0\.[0-9]+)", dir0)
   try:
      if m != None:
         i = a.index(float(m.groups()[0]))
         j = b.index(float(m.groups()[1]))
         #T, overlap = np.loadtxt('data/' + dir0 + '/local_overlap.dat', delimiter=' ', unpack=True)
         
         T = []
         modularity = []
         for dir in os.listdir('study3/' + dir0 + '/dump'):
            m = re.match(r"([0-9]+)\.dat", dir)
            if m != None:
               fh = open('study3/' + dir0 + '/dump/' + dir, 'r')
               G = nx.read_weighted_edgelist(fh)
               fh.close()
               partition = community.best_partition(G)
               modularity.append(community.modularity(partition, G))
               T.append(int(m.groups(0)[0])) 

         [T, modularity] = list(zip(*sorted(zip(T, modularity))))

         axes[i ,j].set_xscale('log')
         axes[i ,j].set_ylim([-0.1, 1])
         axes[i ,j].set_ylabel('Modularity')
         axes[i ,j].set_xlabel('Games per player')
         axes[i ,j].plot(np.array(T) / 100, np.array(modularity))

         # axes[i ,j].set_xscale('log')
         # axes[i ,j].set_ylim([0, 1])
         # axes[i ,j].set_ylabel('Overlap')
         # axes[i ,j].set_xlabel('Games per player')
         # axes[i ,j].plot(T/ 100, overlap)
   except:
      pass 

plt.show()