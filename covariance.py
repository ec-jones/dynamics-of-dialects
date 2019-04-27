import re
import os
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
from sklearn.mixture import GaussianMixture
from scipy import optimize


def read(line):
   return float(line[:-1].split(' ')[1])

def contact_env():
   origA = []
   origB = []
   A = []
   B = []
   for i in range(0, 10):
      run = 'run_%d' % i
      path = 'study2/non_uniform_A/' + run + '/'
      try:
         f_A = []
         f_B = []
         k = 0
         with open('study2/' + dir0 + '/split.dat') as f:
            for line in f:
               if k == 0:
                  origA.append(float(line[3:-1]))
               elif k == 21:
                  origB.append(float(line[3:-1]))
               elif 1 <= k <= 20:
                  f_A.append(read(line))
               elif 22 <= k <= 41:
                  f_B.append(read(line))
               k += 1
         if f_A != [] and f_B != []:
            A.append(np.array(f_A))
            B.append(np.array(f_B))
      except:
         pass

   return origA, origB, A, B

def split():
   origA = []
   origB = []
   A = []
   B = []
   for i in range(0, 10):
      run = 'run_%d' % i
      path = 'study2/non_uniform_A/' + run + '/'
      try:
         f_A = []
         f_B = []
         j = 0
         with open(path + 'split.dat') as f:
            for line in f:
               if j == 0:
                  origA.append(float(line[3:-1]))
               elif j == 21:
                  origB.append(float(line[3:-1]))
               elif 1 <= j <= 20:
                  f_A.append(read(line))
               elif 22 <= j<= 41:
                  f_B.append(read(line))
               j += 1
         if f_A != [] and f_B != []:
            A.append(np.array(f_A))
            B.append(np.array(f_B))
      except:
         pass
   return origA, origB, A, B

def local_overlap():
   local_overlap = []
   for dir in os.listdir('study2'):
      m = re.match(r"contact_[0-9]+", dir)
      try:
         if m != None:
           local_overlap.append(np.loadtxt('study2/' + dir + '/local_overlap_env.dat', delimiter=' '))
      except:
         pass
   local_overlap_mean = np.mean(local_overlap, axis = 0)
   xs1 = local_overlap_mean[0:len(local_overlap_mean) / 2,0]
   ys1 = local_overlap_mean[0:len(local_overlap_mean) / 2,1]
   xs2 = local_overlap_mean[len(local_overlap_mean) / 2:,0]
   ys2 = local_overlap_mean[len(local_overlap_mean) / 2:,1]

   fig, [ax1, ax2] = plt.subplots(1, 2)
   ax1.set_xscale('log')
   ax1.set_ylim([0, 1])
   ax1.plot(xs1 / 100, ys1)
   ax1.set_title('Pre contact')
   ax1.set_xlabel('Games per player')
   ax1.set_ylabel('Local overlap')

   ax2.set_xscale('log')
   ax2.set_ylim([0, 1])
   ax2.plot(xs2 / 100, ys2)
   ax2.set_title('Post contact')
   ax2.set_xlabel('Games per player')
   ax2.set_ylabel('Local overlap')
   plt.show()

def lingcat():
   lingcat = []
   for dir in os.listdir('study2'):
      m = re.match(r"contact_[0-9]+", dir)
      try:
         if m != None:
           lingcat.append(np.loadtxt('study2/' + dir + '/lingcat.dat', delimiter=' '))
      except:
         pass
   lingcat_mean = np.mean(lingcat, axis = 0)
   xs1 = lingcat_mean[0:len(lingcat_mean) / 2,0]
   ys1 = lingcat_mean[0:len(lingcat_mean) / 2,1]
   xs2 = lingcat_mean[len(lingcat_mean) / 2:,0]
   ys2 = lingcat_mean[len(lingcat_mean) / 2:,1]

   fig, [ax1, ax2] = plt.subplots(1, 2)
   ax1.set_xscale('log')
   ax1.set_ylim([0, 20])
   ax1.plot(xs1 / 100, ys1)
   ax1.set_title('Pre contact')
   ax1.set_xlabel('Games per player')
   ax1.set_ylabel('Linguistic category')

   ax2.set_xscale('log')
   ax2.set_ylim([0, 20])
   ax2.plot(xs2 / 100, ys2)
   ax2.set_title('Post contact')
   ax2.set_xlabel('Games per player')
   ax2.set_ylabel('Linguistic category')
   plt.show()

origA, origB, A, B = split()

M = np.zeros(20)
for i in range(0, 20):
   if i >= 1:
      if i <= 18:
         M[i] = (np.mean(A, axis=0)[i-1]+ np.mean(A, axis=0)[i+1])/2 
      else:
         M[i] = np.mean(A, axis=0)[i-1]
   else:
      M[i] = np.mean(A, axis=0)[i+1]

N = np.zeros(20)
for i in range(0, 20):
   if i >= 1:
      if i <= 18:
         N[i] = (np.mean(B, axis=0)[i-1]+ np.mean(A, axis=0)[i+1])/2 
      else:
         N[i] = np.mean(B, axis=0)[i-1]
   else:
      N[i] = np.mean(B, axis=0)[i+1]

print stats.pearsonr(np.array([np.mean(A, axis=0)[4:-5] ,np.mean(B, axis=0)[4:-5]]).flatten(), np.array([M[4:-5] ,N[4:-5]]).flatten())
print stats.ttest_1samp(origB, 0.5)
print(np.mean(origA))
print(np.mean(origB))
W = np.linspace(0, 1, 20)
fig, ax = plt.subplots(1, 1)
ax.plot(W, np.mean(A, axis=0), label = 'Overlap between A and C')
ax.plot(W, np.mean(B, axis=0), label = 'Overlap between B and C')
ax.plot(W, np.mean(A, axis=0) + np.mean(B, axis=0), label = ' Total')
ax.set_ylabel('Overlap')
ax.set_xlabel('Stimuli')
ax.set_ylim([0, 1.1])
ax.legend()
plt.show()

# local_overlap()


# origA, origB, A, B = split()

# edges = np.array((np.array(A)[:,[0,-1]],np.array(B)[:,[0,-1]])).flatten()
# inner = np.array((np.array(A)[:,[1,-2]],np.array(B)[:,[1,-2]])).flatten()
# print np.mean(edges)
# print np.mean(inner)
# print stats.ttest_ind(edges, inner, equal_var=False)

# fig, ax = plt.subplots(1, 1)
# ax.hist(origA, bins = 30)
# ax.hist(origB, label='B', bins = 30)
# ax.set_xlabel('Range of the perceptual space')
# ax.set_xlim([0, 1])
# ax.set_ylabel('Frequency')
# plt.show()

# print stats.ttest_ind(origA, origB, equal_var=False)

"""
mu = 0.5 * (np.mean(np.array(A), axis=0) + np.mean(np.array(B), axis=0))
cov = 0.5 * (np.cov(np.array(A), rowvar=False) + np.cov(np.array(B), rowvar = False))

print(np.max(cov))

mu = 0.5 * (np.mean(np.array(A), axis=0) + np.mean(np.array(B), axis=0))
cov = 0.5 * (np.cov(np.array(A), rowvar=False) + np.cov(np.array(B), rowvar = False))

print(mu)
fig, ax = plt.subplots()

# This is wrong
i = 0
for row in cov:
   ax.plot(np.roll(row, i))
   i += 1   

plt.show()

#print(np.var(mu)) # 0.003  so each band is equally likeley

In natural languages one would expect words that are close in perceptual proximty to avhe similary origins,
 however this model not only doesn't ahve this effect, but ... covariant analysis ....

 """