#!/usr/bin/env python
import pylab
import sys
from matplotlib.collections import PolyCollection
import numpy as np
import matplotlib
import msh

import csv

if (len(sys.argv) < 2):
    print 'usage:', sys.argv[0], 'MT2FILE'
    exit(0)

infile = sys.argv[1]
print 'Read mt2 file:', infile

with open(infile) as f:
    reader = csv.reader(f)
    rows = map(lambda r: r, reader)

    nodes_raw = map(lambda r: r[1:], filter(lambda r: r[0] == 'N', rows))
    node_ids = map(lambda r: int(r[0]), nodes_raw)
    nodes = [0]*(1+max(node_ids))
    for n in nodes_raw:
        # drop the z coordinate in 2D
        nodes[int(n[0])] = map(lambda x: float.fromhex(x), n[1:3])
    
    elements_raw = map(lambda r: r[1:], filter(lambda r: r[0] == 'E', rows))
    element_colors = map(lambda r: int(r[1]), elements_raw)
    elements = []
    for color in range(0, 1+max(element_colors)):
        elements.append([])
    for e in elements_raw:
        elements[int(e[1])].append(map(int, e[2:5]))

    ax = pylab.gca()
    cmap = matplotlib.cm.get_cmap('spectral', 1+max(element_colors))
    cmaplist = [cmap(i) for i in range(cmap.N)]

    # we are only plotting triangles here.
    verts = np.zeros((3, 1+len(elements_raw), 2))
    z = np.ones((1+len(elements_raw),3))

    j = 0
    for color,same_color_elements in enumerate(elements):
        print len(same_color_elements)
        for elem in same_color_elements:
            i = 0
            for node in elem:
                # print i, j, node, elem
                verts[i][j][0] = nodes[node][0]
                verts[i][j][1] = nodes[node][1]
                i += 1
            z[j,0] = cmaplist[color][0]
            z[j,1] = cmaplist[color][1]
            z[j,2] = cmaplist[color][2]
            j += 1

    verts = np.swapaxes(verts, 0, 1)
    polys = PolyCollection(verts, facecolors=z, edgecolor='black', zorder=2)
    ax.add_collection(polys)
    ax.autoscale_view()

#draw nodes
x = [n[0] for n in nodes]
y = [n[1] for n in nodes]
pylab.plot(x, y, color='gray', marker='o', linewidth=0)

bb = {}
bb['xmin'] = min(x) 
bb['xmax'] = max(x) 
bb['ymin'] = min(y) 
bb['ymax'] = max(y) 

#draw centroids
x_avg = []
y_avg = []

for elem in reduce(lambda x,y: x+y, elements):
    xe = 0
    ye = 0
    for node in elem:
        xe += nodes[node][0]
        ye += nodes[node][1]
    x_avg.append(xe / len(elem))
    y_avg.append(ye / len(elem))
pylab.plot(x_avg, y_avg, color='white', marker='.', linewidth=0, markeredgewidth=0)
pylab.axis('equal')
pylab.axis([bb['xmin'],bb['xmax'],bb['ymin'],bb['ymax']])
pylab.gca().set_aspect('equal', 'box')
ax2 = pylab.gcf().add_axes([0.92, 0.1, 0.03, 0.8])
cb = matplotlib.colorbar.ColorbarBase(ax2, cmap=cmap, spacing='proportional')
cb.set_ticks([0, 1])
#ax2.set_yticks([])
#ax2.set_xticks([])
pylab.show()

