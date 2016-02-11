def ints(): return map(int, raw_input().split())

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
    c_order = list(ints())
