# Multiagent Simulation of Dialects

## Usage

To run: ./simulation
The parameters of the simulation are set interally so they can run in parallel.

The the main mechanisms of the simulation are contained within agent.c, category.c, name.c and network.c.
main.c contains IO (for recording statistics) as well as a series of organised studies with specific parameters. To run only one
of these studies the study1/2/3 directives can be set.

The scripts statistics.py, study2.py, and study3.py contain the functions and methods used to manipulate the recorded data. However, this is just for internal use; the main body of these files doesn't maintian the function calls used for all the statistic of the encompanying report.

This project makes use of the python-louvain, networkx, numpy, matplotlib, and scipy python libraries, to run these libraries must be installed. If a binary form is to be redistributed, it must contain the following copyrights:

## Copyrights

Copyright © 2009,
Thomas Aynaud <thomas.aynaud@lip6.fr>.
All rights reserved.

Copyright © 2004-2019 NetworkX Developers,
Aric Hagberg <hagberg@lanl.gov>
Dan Schult <dschult@colgate.edu>
Pieter Swart <swart@lanl.gov>.
All rights reserved.

Copyright © 2005-2019,
NumPy Developers.
All rights reserved.

Copyright © 2012-2013,
Matplotlib Development Team.
All Rights Reserved.

Copyright © 2001, 2002,
Enthought, Inc.
All rights reserved.

Copyright © 2003-2013,
SciPy Developers.
All rights reserved.
