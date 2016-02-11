#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

int rows, cols, drones, turns, payload;
int types;
vector<int> weights;
int warehouses;

struct Coords {
  int x;
  int y;
};

vector<Coords> warehouses_coords;
vector<vector<int> > warehouses_types;

int num_orders;

struct Order {
  Coords coords;
  vector<int> products;
};

vector<Order> orders;

int time(Coords from, Coords to) {
  int d1 = (from.x - to.x);
  d1 = d1 * d1;
  int d2 = (from.y - to.y);
  d2 = d2 * d2;
  float root = sqrt(d1 + d2);
  return int(ceil(root)) + 1;
}

enum operaton_id {
  op_load,
  op_deliver,
};

struct Operation {
  int drone_id;
  operaton_id operaton;
  int wh_cust_id;
  int prod_type;
  int num_its;
};

vector<int> order_to_types(const Order& ord) {
  vector<int> ret(types, 0);
  for (int i = 0; i < ord.products.size(); ++i) {
    ret[ord.products[i]] += 1;
  }
  return ret;
}

vector<Operation> serve(int drone_id, int wh_id, int order_id) {
  vector<Operation> ret;

  vector<int> order = order_to_types(orders[order_id]);
  for (int i = 0; i < types; ++i) {
    while (order[i] > 0) {
      int take = min(order[i], int(payload / weights[i]));
      ret.push_back({drone_id, op_load, wh_id, i, take});
      ret.push_back({drone_id, op_deliver, order_id, i, take});
      order[i] -= take;
    }
  }
  return ret;
}

vector<Operation> serve_all() {
  vector<Operation> ret;
  for (int i = 0; i < num_orders; ++i) {
    vector<Operation> cur = serve(0, 0, i);
    ret.insert(ret.end(), cur.begin(), cur.end());
  }
  return ret;
}

vector<int> find_times(const vector<Operation>& ops) {
  vector<int> ret;
  int cur_time = 0;
  Coords cur_cds = {0, 0};

  for (int i = 0; i < ops.size(); ++i) {
    ret.push_back(cur_time);
    Coords target_cds;
    if (ops[i].operaton == op_load) {
      target_cds = warehouses_coords[ops[i].wh_cust_id];
    }
    else {
      target_cds = orders[ops[i].wh_cust_id].coords;

    }
    cur_time += time(cur_cds, target_cds);
    cur_cds = target_cds;
  }
  return ret;
}

void print_intermediate(const vector<Operation>& ops, const vector<int>& times) {
  assert(ops.size() == times.size());
  assert(ops.size() % 2 == 0);

  cout << ops.size() / 2 << endl;

  for (int i = 0; i < ops.size(); ++i) {
    assert((i % 2 == 0) == (ops[i].operaton == op_load));
    cout << ops[i].wh_cust_id << " "
	 << times[i];
    for (int j = 0; j < types; ++j) {
      if (ops[i].prod_type == j) {
	cout << " " << ops[i].num_its;
      }
      else {
	cout << " " << 0;
      }
    }
  }
}

int main() {
  cin >> rows >> cols >> drones >>  turns >> payload;
  cin >> types;
  weights.reserve(types);
  for (int i = 0; i < types; ++i) {
    int wght;
    cin >> wght;
    weights.push_back(wght);
  }
  cin >> warehouses;

  for (int i = 0; i < warehouses; ++i) {
    Coords cds;
    cin >> cds.x >> cds.y;
    warehouses_coords.push_back(cds);

    vector<int> products;
    products.reserve(types);
    for (int j = 0; j < types; ++j) {
      int prod;
      cin >> prod;
      products.push_back(prod);
    }
    warehouses_types.push_back(products);
  }

  cin >> num_orders;
  for (int i = 0; i < num_orders; ++i) {
    Order order;
    cin >> order.coords.x >> order.coords.y;

    int order_size;
    cin >> order_size;
    for (int j = 0; j < order_size; ++j) {
      int type;
      cin >> type;
      assert(type < types);
      order.products.push_back(type);
    }
    assert(order.products.size() == order_size);
    orders.push_back(order);
  }
  assert(weights.size() == types);
  assert(warehouses_coords.size() == warehouses);
  assert(warehouses_types.size() == warehouses);
  assert(orders.size() == num_orders);

  vector<Operation> ops = serve_all();
  vector<int> times = find_times(ops);
  print_intermediate(ops, times);
}
