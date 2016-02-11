import math

def ints(): return map(int, raw_input().split())

def dist(p, q):
    x0, y0 = p
    x1, y1 = q
    ss = (x1-x0)**2 + (y1-y0)**2
    ret = math.ceil(ss ** 0.5)
    return int(ret)

def wdist(p, q):
    return dist(w_loc[p], w_loc[q])

X, Y, D, DEADLINE, MAXLOAD = ints()

P, = ints()
WEIGHT = ints()

W, = ints()
w_loc = [None] * W
w_stock = [None] * W
for w in range(W):
    w_loc[w] = tuple(ints())
    w_stock[w] = list(ints())

C, = ints()
c_loc = [None] * C
c_num = [None] * C
c_order = [None] * C
for c in range(C):
    c_loc[c] = tuple(ints())
    c_num[c], = ints()
    c_order[c] = list(ints())

################################################################################
### READ IN DEMANDS

DEM = []
for w in range(W):
    DEM.append([])
D, = ints()
while True:
    try:
        data = ints()
    except:
        break
    w = data[0]
    time = data[1]
    prods = data[2:]
    DEM[w].append((time, prods))
for w in range(W):
    DEM[w].sort()

################################################################################
## generate distribution tree
distances = []
for w in range(W):
    cost = sum(dist(w_loc[w], v) for v in w_loc)
    distances.append((cost, w))
distances.sort()

root = distances[0][1]

parent = [None]*W
child = []
for w in range(W):
    child.append([])
for w in range(W):
    if w == root:
        parent[w] = None
    else:
        parent[w] = root
        child[root].append(w)

################################################################################
## compute surpluses
## copy stock
local_surplus = w_stock[::]
for w in range(W):
    local_surplus[w] = local_surplus[w][::]
## subtract demands
for w, dem in enumerate(DEM):
    for de in dem:
        for p in de[1]:
            local_surplus[w][p] -= 1

tree_surplus = [None]*W

def compute_surplus(w):
    ret = local_surplus[w][::]
    for v in child[w]:
        cs = compute_surplus(v)
        for p in range(P):
            ret[p] += cs[p]
    tree_surplus[w] = ret
    return ret

compute_surplus(root)

################################################################################
## move stock around
def update_tree(w, p, num):
    tree_surplus[w][p] += num
    if parent[w] is not None:
        update_tree(parent[w], p, num)

def delivered(w, p, num):
    local[w][p] += num
    update_tree(w, p, num)

def out_of(w, p):
    delivered(w, p, -1)

def in_to(w, p):
    if w is not None:
        delivered(w, p, 1)

local = w_stock[::]
for w in range(W):
    local[w] = local[w][::]

ALL_MOVES = []

MOVEMENT = []
for w in range(W):
    L = []
    for v in range(W):
        L.append([])
    MOVEMENT.append(L)

def add_move(w, v, p, t):
    if v is not None:
        MOVEMENT[w][v].append((t, p))
        ALL_MOVES.append((t,p,w,v))

def move(w, v, p, t):
    if local[w][p] == 0:
        source = None
        best = 0
        # todo: how to decide which child?
        for c in child[w]:
            if tree_surplus[c][p] > best:
                best = tree_surplus[c][p]
                source = c
        if best == 0:
            source = parent[w]
        move(source, w, p, t-dist(w_loc[source], w_loc[w]))
    assert local[w][p] > 0
    out_of(w, p)
    in_to(v, p)
    add_move(w,v,p,t)

for w, dem in enumerate(DEM):
    for de in dem:
        t, ps = de
        for p in ps:
            move(w, None, p, t)

for i in range(W):
    for j in range(W):
        MOVEMENT[i][j].sort(reverse = True)

################################################################################
# greedily assign drones
drones = []
for d in range(D):
    drones = [(0, 0, d)] * D ## pairs free time, warehouse id, drone_id

ALL_MOVES.sort()

DRONE_COMMANDS = []

def arrival_time(d, w):
    return d[0] + wdist(d[1], w)

def drone_move(drone, p, w, v):
    free, pos, did = drone
    items = [p]
    weight = WEIGHT[p]
    tasks = MOVEMENT[w][v]
    for i in range(len(tasks)-1, -1, -1):
        task = tasks[i]
        t, pp = task
        if p == pp:
            tasks.remove(task)
        else:
            if weight + WEIGHT[pp] <= MAXLOAD:
                tasks.remove(task)
                items.append(pp)
    for p in items:
        DRONE_COMMANDS.append((did, "L", w, p, 1))
    for p in items:
        DRONE_COMMANDS.append((did, "U", v, p, 1))
    finish_time = free + wdist(w,v) + 2
    return (finish_time, v, did)

for move in ALL_MOVES:
    t,p,w,v = move
    best_time = 10**1000
    best_drone = None
    for drone in drones:
        free, pos, did = drone
        time = arrival_time(drone, w)
        if time < best_time:
            best_drone = drone
            best_time = time
    assert best_drone is not None
    drones.remove(best_drone)
    new_drone = drone_move(best_drone, p, w, v)
    drones.append(new_drone)


for comm in DRONE_COMMANDS:
    print ' '.join(map(str, comm))








#def pretty_print():
#    grid = []
#    for x in range(X):
#        grid.append(['.'] * Y)
#
#    for x in range(X):
#        for y in range(Y):
#            if (x,y) in w_loc:
#                grid[x][y] = 'w'
#            if (x,y) in c_loc:
#                grid[x][y] = 'o'
#    s = '\n'.join(''.join(row) for row in grid)
