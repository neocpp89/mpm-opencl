#!/usr/bin/env python
import pylab
import sys
from matplotlib.collections import PolyCollection
import numpy as np
import matplotlib
import msh

if (len(sys.argv) < 3):
    print 'usage:', sys.argv[0], 'MESHFILE OUTFILE'
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

outfile = sys.argv[2]
with open(outfile, 'w') as f:
    for i,node in enumerate(meshdict['nodes']):
        s = 'N,'+str(i)+","+",".join(map(lambda x: x.hex(),node))+'\n'
        f.write(s)

    element_id = 0
    for color in list(set(color_list)):
        elems = filter(lambda T: color_list[T[0]] == color, enumerate(meshdict['elements']))
        # print elems
        for element in elems:
            original_id = element[0]
            element_nodes = element[1]
            s = 'E,'+str(element_id)+","+str(color)+","+",".join(map(str, element_nodes))
            # only output gradients if this is a triangle
            if meshdict['element_types'][original_id] == 2:
                s = s+','
                A = meshdict['nodes'][element_nodes[0]]
                B = meshdict['nodes'][element_nodes[1]]
                C = meshdict['nodes'][element_nodes[2]]
                detJ = (B[0] - A[0])*(C[1] - A[1]) - (B[1] - A[1])*(C[0] - A[0])
                dNAdx = (B[1] - C[1]) / detJ
                dNAdy = (C[0] - B[0]) / detJ
                dNBdx = (C[1] - A[1]) / detJ
                dNBdy = (A[0] - C[0]) / detJ
                dNCdx = (A[1] - B[1]) / detJ
                dNCdy = (B[0] - A[0]) / detJ
                s = s + dNAdx.hex()+','
                s = s + dNAdy.hex()+','
                s = s + dNBdx.hex()+','
                s = s + dNBdy.hex()+','
                s = s + dNCdx.hex()+','
                s = s + dNCdy.hex()+','
                s = s + detJ.hex()
            s = s+'\n'
            f.write(s)
            element_id += 1

#for profiling coloring
#sys.exit(0)

'''
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
cb.set_ticks([0,1])
#ax2.set_yticks([])
#ax2.set_xticks([])
pylab.show()
'''
