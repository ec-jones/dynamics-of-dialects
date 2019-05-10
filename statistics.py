#!/usr/bin/env python
# -*- coding: utf-8 -*- 

from __future__ import unicode_literals
import networkx as nx
import os
import numpy as np
import re
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy.interpolate as interpolate

def average(path, local = False, lingcat = False, percat = False, matrix = False, split = False, runs = 10):   
   lingcats = []
   percat = []
   overlap = []
   local_overlap = []
   GLOR = []
   path_length = []
   clustering = []
   # ratiosA = []
   # ratiosB = []
   # windowA = []
   # windowB = []
   for i in range(0, runs):
      try:
         run = 'run_%d' % i
         o = np.loadtxt(path +'/' + run + '/overlap.dat', delimiter=' ')
         overlap.append(o)

         if lingcat:
            l = np.loadtxt(path +'/' + run + '/lingcat.dat', delimiter=' ')
            lingcats.append(l)

         if percat:
            p = np.loadtxt(path +'/' + run + '/percat.dat', delimiter=' ')
            percat.append(p)

         if local:
            lo = np.loadtxt(path +'/' + run + '/local_overlap.dat', delimiter=' ')
            local_overlap.append(lo)
            GLOR.append([[o[i][0], o[i][1] / lo[i][1]] for i in range(len(o))])

         if matrix:
            m = np.loadtxt(path +'/' + run + '/matrix.dat', delimiter=' ')
            G = nx.Graph()
            for [i, j, w] in m:
               if w > 0:
                  G.add_edge(i, j, weight = w)

            clustering.append(nx.average_clustering(G, weight = 'weight', count_zeros = True))
            path_length.append(nx.average_shortest_path_length(G, weight = 'weight'))

         if split:
            pass
            # with open(path + '/' + run + '/split.dat') as f:
            #    k = 0
            #    for line in f:
            #       if k == 0:
            #          ratiosA.append(float(line[3:-1]))
            #       elif k == 21:
            #          ratiosB.append(float(line[3:-1]))
            #       elif 1 <= k <= 20:
            #          windowA.append(read(line))
            #       elif 22 <= k <= 41:
            #          windowB.append(read(line))
            #       i += 1

      except:
         pass

   # trans = []
   # for i in range(0, 10):
   #    diff = (np.array(percat[i]) - np.array(lingcat[i]))[:,1].flatten()
   #    trans.append([i for i, d in enumerate(diff) if d > 1][0])
   # trans = lingcat[0][trans][:, 0]
   # print np.min(trans)
   # print np.mean(trans)
   # print np.max(trans)
   if not os.path.exists(path + '/run_avg'):
      os.mkdir(path + '/run_avg')
      os.mkdir(path + '/run_err')

   if lingcat:
      np.savetxt(path + '/run_avg/lingcat.dat', np.mean(lingcats, axis=0), delimiter=' ')
      np.savetxt(path + '/run_err/lingcat.dat', np.sqrt(np.var(lingcats, axis=0)), delimiter=' ')
   if percat:
      np.savetxt(path + '/run_avg/percat.dat', np.mean(percat, axis=0), delimiter=' ')
      np.savetxt(path + '/run_err/percat.dat', np.sqrt(np.var(percat, axis=0)), delimiter=' ')
   if matrix:
      np.savetxt(path + '/run_avg/path_length.dat', [np.mean(path_length)], delimiter=' ')
      np.savetxt(path + '/run_err/path_length.dat', [np.sqrt(np.var(path_length))], delimiter=' ')
      np.savetxt(path + '/run_avg/clustering.dat', [np.mean(clustering)], delimiter=' ')
      np.savetxt(path + '/run_err/clustering.dat', [np.sqrt(np.var(clustering))], delimiter=' ')


   np.savetxt(path + '/run_avg/overlap.dat', np.mean(overlap, axis=0), delimiter=' ')
   np.savetxt(path + '/run_err/overlap.dat', np.sqrt(np.var(overlap, axis=0)), delimiter=' ')

   if local:
      # print np.min([g[-1] for g in GLOR])
      # print np.mean(GLOR, axis = 0)[-1]
      # print np.max([g[-1] for g in GLOR])

      np.savetxt(path + '/run_avg/local_overlap.dat', np.mean(local_overlap, axis=0), delimiter=' ')
      np.savetxt(path + '/run_avg/GLOR.dat', np.mean(GLOR, axis = 0), delimiter = ' ')

      np.savetxt(path + '/run_err/local_overlap.dat', np.sqrt(np.var(local_overlap, axis=0)), delimiter=' ')
      np.savetxt(path + '/run_err/GLOR.dat', np.sqrt(np.var(GLOR, axis=0)), delimiter=' ')

def category_combined(path, N = 100, error = False):
   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('Categories')

   x, y = np.loadtxt(path + '/run_avg/percat.dat', delimiter=' ', unpack=True)
   if error:
      _, yerr = np.loadtxt(path + '/run_err/percat.dat', delimiter=' ', unpack=True)
      for i in range(0, len(yerr)):
         if i % 10 != 0:
            yerr[i] = 0
      ax.errorbar(x/N, y, yerr = yerr, label='Perceptual categories')
   else: 
      ax.plot(x/N, y, label='Perceptual categories')

   x, y = np.loadtxt(path + '/run_avg/lingcat.dat', delimiter=' ', unpack=True)
   if error:
      _, yerr = np.loadtxt(path + '/run_err/lingcat.dat', delimiter=' ', unpack=True)
      for i in range(0, len(yerr)):
         if i % 10 != 0:
            yerr[i] = 0
      ax.errorbar(x/N, y, yerr = yerr, label='Linguistic categories')
   else: 
      ax.plot(x/N, y, label='Linguistic categories')

   ax.legend()
   plt.show()

def overlap_combined(path, N = 100, local = True, error = False):
   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_ylim([0, 1])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('Overlap')

   x, y = np.loadtxt(path + '/run_avg/overlap.dat', delimiter=' ', unpack=True)
   if error:
      _, yerr = np.loadtxt(path + '/run_err/overlap.dat', delimiter=' ', unpack=True)
      for i in range(0, len(yerr)):
         if i % 10 != 0:
            yerr[i] = 0
      ax.errorbar(x/N, y, yerr = yerr, label='Global overlap')
   else: 
      ax.plot(x/N, y, label='Global overlap')

   if local:
      x, y = np.loadtxt( path + '/run_avg/local_overlap.dat', delimiter=' ', unpack=True)
      if error:
         _, yerr = np.loadtxt(path + '/run_err/local_overlap.dat', delimiter=' ', unpack=True)
         for i in range(0, len(yerr)):
            if i % 10 != 0:
               yerr[i] = 0
         ax.errorbar(x/N, y, yerr = yerr, label='Local overlap')
      else: 
         ax.plot(x/N, y, label='Local overlap')
      ax.legend()
   plt.show()

def ht_plot(delta = 100, hmax = 10):
   H = []
   T = []
   GLOR = []
   Z = float('nan')
   for d in os.listdir('study1/linear_network'):
      m = re.match(r'h=([0-9\.]+)t=([0-9\.]+)', d)
      if m != None:
         h = float(m.group(1))
         t = float(m.group(2))
         H.append(h)
         T.append(t)

         average('study1/linear_network/' + d, local = True, runs = 5)
         _, g = np.loadtxt('study1/linear_network/' + d + '/run_avg/GLOR.dat', delimiter=' ', unpack=True)
         GLOR.append(g[-1])

         if h == 0 and t == 0:
            Z = g[-1]

         idxs = [i for i in range(len(H)) if H[i] == ((1- h * t) / (1 - t)) and T[i] == 1 - t]
         if idxs != []:
            GLOR[i] += g[-1]
            GLOR[i] /= 2
         else:
            H.append((1- h * t) / (1 - t))
            T.append(1 - t)
            GLOR.append(g[-1])

   for r in np.linspace(0, 1, delta):
      H.append(r)
      T.append(0)
      GLOR.append(Z)

      H.append(1)
      T.append(r)
      GLOR.append(Z)

   t, h = np.mgrid[0:1:100j, 0:hmax:100j]
   grid = interpolate.griddata((T, H), GLOR, (t, h), method='linear')
   for i in range(delta):
      for j in range(delta):
         if h[i, j] * t[i, j] > 1:
            grid[i, j] = float('nan')
         if h[i, j] < 1:
            grid[i, j] = float('nan')


   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xlabel('t')
   ax.set_ylabel('h')
   ax.set_xlim([0.1, 0.9])
   ax.set_ylim([1, 10])

   cf = ax.contourf(t, h, grid)
   fig.colorbar(cf, ax=ax)

   T2 = [T[i] for i in range(len(T)) if H[i] * T[i] <= 1 and H[i] > 1]
   H2 = [H[i] for i in range(len(H)) if H[i] * T[i] <= 1 and H[i] > 1]
   ax.scatter(T2, H2, marker='x', c='black')
   plt.show()

def F_comparison(N = 100):
   fig = plt.figure()
   ax = fig.add_subplot(111)
   #ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('GLOR')
   T, o = np.loadtxt('study1/linear_network/overlap.dat', delimiter=' ', unpack=True)
   _, lo = np.loadtxt('study1/linear_network/local_overlap.dat', delimiter=' ', unpack=True)
   ax.plot(T/N, o/lo, label=u'F = ∞')
   F = 30
   while F > 5:
      try:
         T, o = np.loadtxt('study1/linear_network_' + str(F) + '/overlap.dat', delimiter=' ', unpack=True)
         _, lo = np.loadtxt('study1/linear_network_' + str(F) + '/local_overlap.dat', delimiter=' ', unpack=True)

         _, o2 = np.loadtxt('study1/linear_network2_' + str(F) + '/overlap.dat', delimiter=' ', unpack=True)
         _, lo2 = np.loadtxt('study1/linear_network2_' + str(F) + '/local_overlap.dat', delimiter=' ', unpack=True)

         o = 0.5 * np.array(o + o2) 
         lo = 0.5 * np.array(lo + lo2) 
         ax.plot(T/N, o/lo, label='F = ' + str(F))
      except:
         pass
      F = F / 2

   ax.legend()
   plt.show()

def K_comparison(N = 100):
   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('GLOR')

   for d in os.listdir('study1/ring_lattice'):
      average('study1/ring_lattice/' + d, local = True, runs = 5)
      m = re.match(r'K=([0-9])', d)
      if m != None:
         k = int(m.group(1))

         # T, o = np.loadtxt('study1/ring_lattice/' + d + '/run_avg/overlap.dat', delimiter=' ', unpack=True)
         # _, oerr = np.loadtxt('study1/ring_lattice/' + d + '/run_err/overlap.dat', delimiter=' ', unpack=True)
         T, lo = np.loadtxt('study1/ring_lattice/' + d + '/run_avg/local_overlap.dat', delimiter=' ', unpack=True)
         _, loerr = np.loadtxt('study1/ring_lattice/' + d + '/run_err/local_overlap.dat', delimiter=' ', unpack=True)
         # _, g = np.loadtxt('study1/ring_lattice/' + d + '/run_avg/GLOR.dat', delimiter=' ', unpack=True)
         # _, gerr = np.loadtxt('study1/ring_lattice/' + d + '/run_err/GLOR.dat', delimiter=' ', unpack=True)
         for i in range(0, len(T)):
            if i % 10 != 0:
               # oerr[i] = 0
               loerr[i] = 0
               # gerr[i] = 0
         # ax.errorbar(T / N, g, yerr=gerr, label = 'K = %d' % k)
         # ax.errorbar(T / N, o, yerr=oerr, label = 'K = %d' % k)
         ax.errorbar(T / N, lo, yerr=loerr, label = 'K = %d' % k)
   ax.legend()
   plt.show()

def beta_comparison():
   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xscale('log')
   ax.set_xlim([10e-5, 1])
   ax.set_xlabel('β')
   ax.set_ylabel('GLOR')

   glor = []
   path = []
   clustering = []
   beta = []

   gerr = []
   perr = []
   cerr = []

   for dir in os.listdir('study1/small_world_network'):
      m = re.match(r"beta_([0-9\.]+)", dir)
      try:
         if m != None:
            average('study1/small_world_network/' + dir,  local = True, matrix = True)
            _, g = np.loadtxt('study1/small_world_network/' + dir + '/run_avg/GLOR.dat', delimiter=' ', unpack=True)
            _, g_e = np.loadtxt('study1/small_world_network/' + dir + '/run_err/GLOR.dat', delimiter=' ', unpack=True)
            c = np.loadtxt('study1/small_world_network/' + dir + '/run_avg/clustering.dat', unpack=True)
            c_e= np.loadtxt('study1/small_world_network/' + dir + '/run_err/clustering.dat', unpack=True)
            p = np.loadtxt('study1/small_world_network/' + dir + '/run_avg/path_length.dat', unpack=True)
            p_e = np.loadtxt('study1/small_world_network/' + dir + '/run_err/path_length.dat', unpack=True)
            glor.append(g[-1])
            gerr.append(g_e[-1])
            clustering.append(c)
            cerr.append(c_e)
            path.append(p)
            perr.append(p_e)
            beta.append(float(m.groups(1)[0]))
      except Exception as e:
         print(e)

   beta, glor, path, clustering = zip(*sorted(zip(beta, glor, path, clustering)))

   ax.errorbar(beta, glor, yerr= gerr, label='GLOR')
   ax.errorbar(beta, path / path[0], yerr=perr/path[0], label='L/L(0)')
   ax.errorbar(beta, clustering / clustering[0], yerr=cerr/clustering[0], label='C/C(0)')
   ax.legend()
   plt.show()

   print stats.linregress(path, glor), stats.linregress(clustering, glor)

def contact():
   average('study2/non_uniform_A', local = True, lingcat = True, split = True)

   fig, [ax1, ax2] = plt.subplots(1, 2)
   ax1.set_xlabel('Games per player')
   ax2.set_xlabel('Games per player')
   ax1.set_ylabel('Linguistic categories')
   ax2.set_ylabel('Linguistic categories')
   ax1.set_title('Pre-contact')
   ax2.set_title('Post-contact')

   x, y = np.loadtxt('study2/non_uniform_A/run_avg/lingcat.dat', delimiter=' ', unpack=True)
   _, yerr = np.loadtxt('study2/non_uniform_A/run_err/lingcat.dat', delimiter=' ', unpack=True)
   for i in range(0, len(yerr)):
      if i % 10 != 0:
         yerr[i] = 0

   x1 = x[:len(x)/2]
   x2 = x[len(x)/2:]
   y1 = y[:len(y)/2]
   y2 = y[len(y)/2:]

   yerr1 = yerr[:len(yerr)/2]
   yerr2 = yerr[len(yerr)/2:]
   ax1.errorbar(x1 / 100, y1, yerr = yerr1)
   ax2.errorbar(x2 / 100, y2, yerr = yerr1)

   ax1.set_xscale('log')
   ax1.set_xlim([10e0, 10e6])
   ax1.set_ylim([0, 80])
   ax2.set_xscale('log')
   ax2.set_xlim([10e0, 10e6])
   ax2.set_ylim([0, 80]) 

   plt.tight_layout()
   plt.show()


   fig, [ax1, ax2] = plt.subplots(1, 2)
   ax1.set_xlabel('Games per player')
   ax2.set_xlabel('Games per player')
   ax1.set_ylabel('Overlap')
   ax2.set_ylabel('Overlap')
   ax1.set_title('Pre-contact')
   ax2.set_title('Post-contact')

   x, y = np.loadtxt('study2/non_uniform_A/run_avg/local_overlap.dat', delimiter=' ', unpack=True)
   _, yerr = np.loadtxt('study2/non_uniform_A/run_err/local_overlap.dat', delimiter=' ', unpack=True)
   for i in range(0, len(yerr)):
      if i % 10 != 0:
         yerr[i] = 0

   x1 = x[:len(x)/2]
   x2 = x[len(x)/2:]
   y1 = y[:len(y)/2]
   y2 = y[len(y)/2:]

   yerr1 = yerr[:len(yerr)/2]
   yerr2 = yerr[len(yerr)/2:]
   ax1.errorbar(x1 / 100, y1, yerr = yerr1)
   ax2.errorbar(x2 / 100, y2, yerr = yerr1)

   ax1.set_xscale('log')
   ax1.set_xlim([10e0, 10e6])
   ax1.set_ylim([0, 1])
   ax2.set_xscale('log')
   ax2.set_xlim([10e0, 10e6])
   ax2.set_ylim([0, 1]) 

   plt.tight_layout()
   plt.show()

# average('study0', lingcat = True, percat = True)
# category_combined('study0', error = True)
# overlap_combined('study0', error = True, local = False)
# average('study1/linear_network/h=0t=0', local = True, lingcat = True, percat = True, runs = 10)
# category_combined('study1/linear_network/h=0t=0', error = True)
# overlap_combined('study1/linear_network/h=0t=0', local = True, error = True)

# ht_plot()

# K_comparison()

beta_comparison()

# contact()

# average('study3/live_model/l1=0.150000l2=0.150000', local = True)
# overlap_combined('study3/live_model/l1=0.150000l2=0.150000', error = True, local = True)