#!/usr/bin/env python
# -*- coding: utf-8 -*- 

from __future__ import unicode_literals
import numpy as np
import matplotlib.pyplot as plt

def category_combined(path, N = 100):
   x, y = np.loadtxt(path + '/percat.dat', delimiter=' ', unpack=True)

   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('Categories')
   ax.plot(x/N, y, label='Perceptual categories')

   x, y = np.loadtxt( path + '/lingcat.dat', delimiter=' ', unpack=True)
   ax.plot(x/N, y, label='Linguistic categories')
   ax.legend()
   plt.show()

def overlap_combined(path, N = 100):
   x, y = np.loadtxt(path + '/overlap.dat', delimiter=' ', unpack=True)

   fig = plt.figure()
   ax = fig.add_subplot(111)
   #ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_ylim([0, 1])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('Overlap')
   ax.plot(x/N, y, label='Global overlap')

   x, y = np.loadtxt( path + '/local_overlap.dat', delimiter=' ', unpack=True)
   ax.plot(x/N, y, label='Local overlap')
   ax.legend()
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
   #ax.set_xscale('log')
   ax.set_xlim([10e0, 10e6])
   ax.set_xlabel('Games per player')
   ax.set_ylabel('GLOR')
   K = 2
   while K < 20:
      try:
         T, o = np.loadtxt('study1/ring_lattice_' + str(K) + '/overlap.dat', delimiter=' ', unpack=True)
         _, lo = np.loadtxt('study1/ring_lattice_' + str(K) + '/local_overlap.dat', delimiter=' ', unpack=True)

         ax.plot(T/N, o/lo, label='F = ' + str(K))
      except:
         pass
      K += 2

   ax.legend()
   plt.show()

def beta_comparison():
   fig = plt.figure()
   ax = fig.add_subplot(111)
   ax.set_xlabel('β')
   ax.set_ylabel('GLOR')
   glor = []
   for i in range(0, 11):
      b = i / 10.0
      _, o = np.loadtxt('study1/beta_' + str(b) + '/overlap.dat', delimiter=' ', unpack=True)
      _, lo = np.loadtxt('study1/beta_' + str(b) + '/local_overlap.dat', delimiter=' ', unpack=True)
      o = np.mean(o[-3:])
      lo = np.mean(lo[-3])
      glor.append(o/lo)

   print(glor)

   beta = np.linspace(0, 1, 11)
   ax.scatter  (beta, glor)
   plt.show()


#category_combined('study1/linear_network')
#overlap_combined('study1/linear_network')
# F_comparison()
# K_comparison()
beta_comparison()