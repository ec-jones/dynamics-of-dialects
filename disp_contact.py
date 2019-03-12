import os
import numpy as np
import matplotlib.pyplot as plt
import re

lingcat = []
local_match_env = []
match_env = []
for dir in os.listdir('data'):
	if re.match(r'contact[0-9]', dir):
		lingcat.append(np.loadtxt('data/' + dir + '/lingcat.dat', delimiter=' '))
		local_match_env.append(np.loadtxt('data/' + dir + '/local_match_env.dat', delimiter=' '))
		match_env.append(np.loadtxt('data/' + dir + '/match_env.dat', delimiter=' '))

lingcat_mean = np.mean(lingcat, axis = 0)
xs1 = lingcat_mean[0:len(lingcat_mean) / 2,0]
ys1 = lingcat_mean[0:len(lingcat_mean) / 2,1]
xs2 = lingcat_mean[len(lingcat_mean) / 2:,0]
ys2 = lingcat_mean[len(lingcat_mean) / 2:,1]

fig, [ax1, ax2] = plt.subplots(1, 2)
ax1.set_xscale('log')
ax1.set_ylim([0, 20])
ax1.plot(xs1 / 100, ys1)

ax2.set_xscale('log')
ax2.set_ylim([0, 20])
ax2.plot(xs2 / 100, ys2)
plt.show()

local_match_mean = np.mean(local_match_env, axis = 0)
xs1 = local_match_mean[0:len(local_match_mean) / 2,0]
ys1 = local_match_mean[0:len(local_match_mean) / 2,1]
xs2 = local_match_mean[len(local_match_mean) / 2:,0]
ys2 = local_match_mean[len(local_match_mean) / 2:,1]

fig, [ax1, ax2] = plt.subplots(1, 2)
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.set_ylim([0.1, 1])
ax1.plot(xs1 / 100, ys1)

ax2.set_xscale('log')
ax2.set_yscale('log')
ax2.set_ylim([0.1, 1])
ax2.plot(xs2 / 100, ys2)
plt.show()

match_mean = np.mean(match_env, axis = 0)
xs1 = match_mean[0:len(match_mean) / 2,0]
ys1 = match_mean[0:len(match_mean) / 2,1]
xs2 = match_mean[len(match_mean) / 2:,0]
ys2 = match_mean[len(match_mean) / 2:,1]

fig, [ax1, ax2] = plt.subplots(1, 2)
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.set_ylim([0.1, 1])
ax1.plot(xs1 / 100, ys1)

ax2.set_xscale('log')
ax2.set_yscale('log')
ax2.set_ylim([0.1, 1])
ax2.plot(xs2 / 100, ys2)
plt.show()