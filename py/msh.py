#!/usr/bin/env python
import numpy as np
import operator
import copy

WAITCOMMAND = 0
NODENUM = 1
NODEDEF = 2
ELEMNUM = 3
ELEMDEF = 4
COMMENT = 100

def read_mesh(meshfilename):
    state = WAITCOMMAND
    oldcommand = ''
    meshdict = {'nodes':[], 'elements':[], 'num_nodes':0, 'num_elements':0}
    with open(meshfilename, 'r') as f:
        for line in f:
            line = line.strip()
            if (state == WAITCOMMAND):
                if (line[0] == '$'):
                    oldcommand = line[1:]
                    if (oldcommand == 'Nodes'):
                        state = NODENUM
                    elif (oldcommand == 'Elements'):
                        state = ELEMNUM
                    else:
                        state = COMMENT
            elif (state == COMMENT):
                if (('$End'+oldcommand) == line):
                    state = WAITCOMMAND
            elif (state == NODENUM):
                meshdict['num_nodes'] = int(line)
                meshdict['nodes'] = [0]*meshdict['num_nodes']
                if (meshdict['num_nodes'] > 0):
                    state = NODEDEF
                else:
                    state = WAITCOMMAND
            elif (state == NODEDEF):
                if (line == '$EndNodes'):
                    state = WAITCOMMAND
                else:
                    nlist = map(float, line.split(' '))
                    # make node nums = node index in zero based languages
                    nid = int(nlist[0])-1
                    coords = nlist[1:]
                    meshdict['nodes'][nid] = coords
            elif (state == ELEMNUM):
                meshdict['num_elements'] = int(line)
                meshdict['elements'] = [0]*meshdict['num_elements']
                meshdict['element_types'] = [0]*meshdict['num_elements']
                if (meshdict['num_elements'] > 0):
                    state = ELEMDEF
                else:
                    state = WAITCOMMAND
            elif (state == ELEMDEF):
                if (line == '$EndElements'):
                    state = WAITCOMMAND
                else:
                    elist = map(int, line.split(' '))
                    eid = elist[0]-1
                    etype = elist[1]
                    numtags = elist[2]
                    # make node nums = node index in zero based languages
                    nodenums = map(lambda x: x-1, elist[(3+numtags):])
                    meshdict['elements'][eid] = nodenums
                    meshdict['element_types'][eid] = etype
            else:
                print 'Unknown state'
                state = WAITCOMMAND
    return meshdict

def create_adjacency_list(meshdict):
    adj_list = [0]*meshdict['num_elements']
    n_list = [0]*meshdict['num_nodes']
    # super-duper inefficient way to do this. oh well.

    # figure out which nodes are part of which elements
    for i,e in enumerate(meshdict['elements']):
        eid = i
        for node in e:
            if (n_list[node] == 0):
                n_list[node] = []
            n_list[node].append(eid)

    # remove duplicates on nodelists
    for i,n in enumerate(n_list):
        n_list[i] = list(set(n))

    # for each element, go through the node list and find shared elements
    for i,e in enumerate(meshdict['elements']):
        adj_elems = []
        for node in e:
            adj_elems.extend(n_list[node])
        adj_elems = list(set(adj_elems))
        # remove self!
        adj_elems.remove(i)
        # assign to adjacency list
        adj_list[i] = adj_elems

    return adj_list

#greedy algorithm
def color_elements_greedy(meshdict, adj_list, order=None):
    color_list = [-1]*len(adj_list)
    if (order == None):
        order = enumerate(color_list)
    for i,m in order:
        taken = set([color_list[adj] for adj in adj_list[i]])
        taken.add(-1)
        allcolors = set(range(-1, max(taken)+2))
        #free = sorted(list(allcolors-taken))
        #first free color
        color_list[i] = min(allcolors-taken)
            
    return color_list

#smallest degree last
def color_elements_sdl(meshdict, adj_list):
    # order by degree
    adj_copy = copy.deepcopy(adj_list)
    deg_ordered = []
    processed = [0]*len(adj_list)
    deg = [(i, len(x)) for i,x in enumerate(adj_copy)]
    max_deg = max(deg, key=operator.itemgetter(1))[1]
    deg_list = []
    for x in range(0, max_deg+1):
        deg_list.append([])
    for d in deg:
        deg_list[d[1]].append(d)
    vmin_next = None
    imin_next = 0
    for x in range(0, meshdict['num_elements']):
        # get smallest element
        if (vmin_next is not None):
            imin = imin_next
            vmin = vmin_next
        else:
            for i in range(0, max_deg+1):
                if (deg_list[i] != []):
                    imin = i
                    vmin = deg_list[i][0]
                    break
        current_minimum_degree = vmin[1]
        deg_ordered.append(vmin)
        processed[vmin[0]] = 1

        # remove this vertex
        deg_list[imin].remove(vmin)

        # likewise, reduce degree of neighbors by 1 (due to removal)
        vmin_next = None
        for i in adj_copy[vmin[0]]:
            adj_copy[i].remove(vmin[0])
            deg_list[deg[i][1]].remove(deg[i])
            deg[i] = (i, deg[i][1] - 1)
            deg_list[deg[i][1]].append(deg[i])
#            if (deg[i][1] < current_minimum_degree):
#                current_minimum_degree = deg[i][1]
#                imin_next = deg[i][1]
#                vmin_next = deg[i]
        adj_copy[vmin[0]] = []

    # reverse to put smallest degrees last
    deg_ordered.reverse()

    # color with greedy algorithm
    color_list = color_elements_greedy(meshdict, adj_list, deg_ordered)

    return color_list

#largest degree first
def color_elements_ldf(meshdict, adj_list):
    # order by degree
    adj_copy = copy.deepcopy(adj_list)
    deg_ordered = []
    processed = [0]*len(adj_list)
    deg = [(i, len(x)) for i,x in enumerate(adj_copy)]
    while (min(processed) == 0):
        # get largest element
        vmax = max(deg, key=operator.itemgetter(1))
        deg_ordered.append(vmax)
        processed[vmax[0]] = 1

        # hackish, but we 'remove' it by setting the degree to something small
        deg[vmax[0]] = (vmax[0], -1)

        # likewise, reduce degree of neighbors by 1 (due to removal)
        for i in adj_copy[vmax[0]]:
            adj_copy[i].remove(vmax[0])
            deg[i] = (i, deg[i][1] - 1)
        adj_copy[vmax[0]] = []

    # color with greedy algorithm
    color_list = color_elements_greedy(meshdict, adj_list, deg_ordered)

    return color_list

def get_bounding_box(meshdict):
    bb = {'xmin':0, 'ymin':0, 'xmax':0, 'ymax':0}
    bb['xmin'] = meshdict['nodes'][0][0]
    bb['ymin'] = meshdict['nodes'][0][1]
    bb['xmax'] = meshdict['nodes'][0][0]
    bb['ymax'] = meshdict['nodes'][0][1]
    for nc in meshdict['nodes']:
        if (nc[0] < bb['xmin']):
            bb['xmin'] = nc[0]
        if (nc[1] < bb['ymin']):
            bb['ymin'] = nc[1]
        if (nc[0] > bb['xmax']):
            bb['xmax'] = nc[0]
        if (nc[1] > bb['ymax']):
            bb['ymax'] = nc[1]
    return bb
