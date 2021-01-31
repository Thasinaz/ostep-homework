#!/usr/bin/env python

# import the library
from zplot import *
import sys

# describe the drawing surface
ctype = 'svg' if len(sys.argv) < 2 else sys.argv[1]
c = canvas(ctype, title='tlb', dimensions=['3in', '2.4in'])

# load some data
t = table(file='tlb.data')

# make a drawable region for a graph
d = drawable(canvas=c, xrange=[1,8192], yrange=[0,16], xscale='log2',
             coord=['0.5in','0.4in'], dimensions=['2.3in','1.7in'])

# make some axes
axis(drawable=d, title='TLB Size Measurement', xtitle='Number Of Pages',
     ytitle='Time Per Accesses(ns)', xauto=[1, 8192,8191])

# plot the points
p = plotter()
p.points(drawable=d, table=t, xfield='x', yfield='y', style='triangle',
         linecolor='red', fill=True, fillcolor='red')

# finally, output the graph to a file
c.render()
