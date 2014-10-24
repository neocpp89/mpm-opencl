#!/usr/bin/env python
import pylab
import sys
from matplotlib.collections import PolyCollection
import numpy as np
import matplotlib
import msh

if (len(sys.argv) < 2):
    print 'usage:', sys.argv[0], 'MESHFILE'
    exit(0)

meshdict = msh.read_mesh(sys.argv[1])
print 'Read mesh file:', sys.argv[1]
#delete elements that are not tris (2) or quads (3)
meshdict['elements'] = [x for i,x in enumerate(meshdict['elements']) if (meshdict['element_types'][i] == 2 or meshdict['element_types'][i] == 3)]
meshdict['element_types'] = [x for x in meshdict['element_types'] if (x == 2 or x == 3)]
meshdict['num_elements'] = len(meshdict['elements'])
#print meshdict
bb = msh.get_bounding_box(meshdict)
print 'Bounding box:', bb
adj_list = msh.create_adjacency_list(meshdict)
print 'Created adjacency list.'
#print adj_list
color_list = msh.color_elements_sdl(meshdict, adj_list)
#print color_list
print 'Mesh colored with', 1+max(color_list), 'colors ('+str(len(meshdict['elements']))+' elements) using SDL ordering.'
for color in list(set(color_list)):
    print color_list.count(color), "elements colored by color", color

#for profiling coloring
#sys.exit(0)

#draw element colors
ax = pylab.gca()
cmap = matplotlib.cm.get_cmap('spectral', 1+max(color_list))
cmaplist = [cmap(i) for i in range(cmap.N)]
for etype in set(meshdict['element_types']):
    elements = [(color_list[i], x) for i,x in enumerate(meshdict['elements']) if meshdict['element_types'][i] == etype]
    verts = np.zeros((len(elements[0][1]), len(elements), 2))
    z = np.ones((len(elements),3))
    for j,ce in enumerate(elements):
        color = ce[0]
        elem = ce[1]
        for i,node in enumerate(elem):
            verts[i][j][0] = meshdict['nodes'][node][0]
            verts[i][j][1] = meshdict['nodes'][node][1]
        z[j,0] = cmaplist[color][0]
        z[j,1] = cmaplist[color][1]
        z[j,2] = cmaplist[color][2]
    verts = np.swapaxes(verts, 0, 1)
    polys = PolyCollection(verts, facecolors=z, edgecolor='black', zorder=2)
    ax.add_collection(polys)
    ax.autoscale_view()

# draw elements
#for elem in meshdict['elements']:
#    xe = [meshdict['nodes'][elem[-1]][0]]
#    ye = [meshdict['nodes'][elem[-1]][1]]
#    for node in elem:
#        xe.append(meshdict['nodes'][node][0])
#        ye.append(meshdict['nodes'][node][1])
#    pylab.plot(xe, ye, color='red')

#draw nodes
x = [n[0] for n in meshdict['nodes']]
y = [n[1] for n in meshdict['nodes']]
pylab.plot(x, y, color='gray', marker='o', linewidth=0)

#draw centroids
x_avg = []
y_avg = []
for elem in meshdict['elements']:
    xe = 0
    ye = 0
    for node in elem:
        xe += meshdict['nodes'][node][0]
        ye += meshdict['nodes'][node][1]
    x_avg.append(xe / len(elem))
    y_avg.append(ye / len(elem))
pylab.plot(x_avg, y_avg, color='white', marker='.', linewidth=0, markeredgewidth=0)
pylab.axis('equal')
pylab.axis([bb['xmin'],bb['xmax'],bb['ymin'],bb['ymax']])
pylab.gca().set_aspect('equal', 'box')
ax2 = pylab.gcf().add_axes([0.92, 0.1, 0.03, 0.8])
cb = matplotlib.colorbar.ColorbarBase(ax2, cmap=cmap, spacing='proportional')
ax2.set_yticks([])
ax2.set_xticks([])
pylab.show()

