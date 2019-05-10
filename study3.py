import community
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import re
import os

def community_size(path):
   T = []
   size = []
   for d in range(0, 10):
      T_d = []
      size_d = []
      for dir in os.listdir(path + ('/run_%d/partition' % d)):
         m = re.match(r"([0-9]+)\.dat", dir)
         if m != None:
            ma = np.loadtxt(path + ('/run_%d/matrix/' % d) + dir, delimiter=' ')
            G = nx.Graph()
            for [a1, b1, w] in ma:
               if w > 0:
                  G.add_edge(a1, b1, weight = w)
            partition = np.loadtxt(path + ('/run_%d/partition/' % d) + dir, delimiter=' ')
            size_d.append(len(np.unique(partition)))
            T_d.append(int(m.groups(0)[0]))

      T.append(T_d)
      size.append(size_d)
   
   T = np.mean(T, axis = 0)
   size = np.mean(size, axis = 0)

   [T, size] = list(zip(*sorted(zip(T, size))))

   fig, ax = plt.subplots()
   ax.set_xscale('log')
   # ax.set_ylim([-0.1, 1])
   ax.set_ylabel('Community Size')
   ax.set_xlabel('Games per player')
   ax.plot(np.array(T) / 100, np.array(size))
   # axes[i ,j].plot(np.array(T) / 100, np.array(size) / np.max(size))
   # fig.set_size_inches(2, 2)
   fig.tight_layout()
   plt.show()

def modularity():
   a = [0.15, 0.3, 0.45]
   b = [0.15, 0.3, 0.45]

   # plt.setp(axes.flat, xlabel='Games per player', ylabel='Modularity')

   # pad = 5

   # for ax, col in zip(axes[0], ['lambda2: {}'.format(col) for col in b]):
      # ax.annotate(col, xy=(0.5, 1), xytext=(0, pad),
                  # xycoords='axes fraction', textcoords='offset points',
                  # size='large', ha='center', va='baseline')

   # for ax, row in zip(axes[:,0], ['lambda1: {}'.format(row) for row in a]):
      # ax.annotate(row, xy=(0, 0.5), xytext=(-ax.yaxis.labelpad - pad, 0),
                  # xycoords=ax.yaxis.label, textcoords='offset points',
                  # size='large', ha='right', va='center')

   for dir0 in os.listdir('study3/null_model'):
      m = re.match(r"l1=([0-9\.]+)l2=([0-9\.]+)", dir0)
      if m != None:
         l1 = float(m.groups()[0])
         l2 = float(m.groups()[1])
         if l1 in a and l2 in b:
            i = int(a.index(float(m.groups()[0])))
            j = int(b.index(float(m.groups()[1])))

            T = []
            modularity = []
            # size = []
            # size_std = []
            for d in range(0, 10):
               T_d = []
               modularity_d = []
               for dir in os.listdir('study3/null_model/' + dir0 + ('/run_%d/partition' % d)):
                  m = re.match(r"([0-9]+)\.dat", dir)
                  if m != None:
                     ma = np.loadtxt('study3/null_model/'  + dir0 + ('/run_%d/matrix/' % d) + dir, delimiter=' ')
                     G = nx.Graph()
                     for [a1, b1, w] in ma:
                        if w > 0:
                           G.add_edge(a1, b1, weight = w)
                     partition = np.loadtxt('study3/null_model/'  + dir0 + ('/run_%d/partition/' % d) + dir, delimiter=' ')
                     partition = {k :int(partition[k]) for k in range(len(partition))}
                     modularity_d.append((community.modularity(partition, G)))
                     T_d.append(int(m.groups(0)[0])) 
                  

                  # print partition.values()
                  # size_std.append(np.sqrt(np.var([len([k for k in range(100) if partition[k] == c]) for c in np.unique(partition.values())])))
                  # print 100.0 / (np.max(partition.values()) + 1)
                  # size.append(100.0 / (np.max(partition.values()) + 1))

                  # print(np.unique([int(k) for k in partition.keys()]))
                  # print(len(np.unique([int(k) for k in partition.keys()])))
                  # count.append(len(np.unique([int(k) for k in partition.keys()]))) 
            T.append(T_d)
            modularity.append(modularity_d)
         
         T = np.mean(T, axis = 0)
         modularity = np.mean(modularity, axis = 0)

         [T, modularity] = list(zip(*sorted(zip(T, modularity))))

         fig, ax = plt.subplots()
         ax.set_xscale('log')
         ax.set_ylim([-0.1, 1])
         ax.set_ylabel('Modularity')
         ax.set_xlabel('Games per player')
         ax.plot(np.array(T) / 100, np.array(modularity))
         # axes[i ,j].plot(np.array(T) / 100, np.array(size) / np.max(size))
         fig.set_size_inches(2, 2)
         fig.tight_layout()
         fig.savefig('modularity%d%d.png' % (i, j))

def build_partition():
   a = [0.15, 0.3, 0.45]
   b = [0.15, 0.3, 0.45]
   for dir0 in os.listdir('study3/null_model'):
      m = re.match(r"l1=([0-9\.]+)l2=([0-9\.]+)", dir0)
      if m != None:
         l1 = float(m.groups()[0])
         l2 = float(m.groups()[1])
         if l1 in a and l2 in b:
            i = int(a.index(float(m.groups()[0])))
            j = int(b.index(float(m.groups()[1])))
            for d in range(0, 10):
               T_d = []
               modularity_d = []
               for dir in os.listdir('study3/null_model/' + dir0 + ('/run_%d/matrix' % d)):
                  m = re.match(r"([0-9]+)\.dat", dir)
                  if m != None:
                     ma = np.loadtxt('study3/null_model/'  + dir0 + ('/run_%d/matrix/' % d) + dir, delimiter=' ')
                     G = nx.Graph()
                     for [a1, b1, w] in ma:
                        if w > 0:
                           G.add_edge(a1, b1, weight = w)
                     partition = community.best_partition(G)
                     if not os.path.exists('study3/null_model/' + dir0 + ('/run_%d/partition/' % d)):
                        os.makedirs('study3/null_model/' + dir0 + ('/run_%d/partition/' % d))
                     np.savetxt('study3/null_model/' + dir0 + ('/run_%d/partition/' % d) + dir, partition.values(), delimiter=' ')

community_size('study3/null_model/l1=0.300000l2=0.300000')
community_size('study3/live_model/l1=0.300000l2=0.300000')