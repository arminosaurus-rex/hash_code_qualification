#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <cstdlib>

using namespace std;

typedef pair<int, int> PII;
typedef pair<int, PII> PIPII;

const int max_number_products = 2000; // See redundancy.in
const int max_number_warehouses = 16; // See redundacy.in
const int max_number_customers = 1250; // busy day.in

int num_rows, num_cols, num_drones, deadline, max_load;
int num_products;
int weight_products[max_number_products];

int num_warehouses;
int row_warehouse[max_number_warehouses];
int col_warehouse[max_number_warehouses];
int availability_warehouses[max_number_warehouses][max_number_products];

int num_customers;
int row_customer[max_number_customers];
int col_customer[max_number_customers];
int num_orders_customer[max_number_customers];
vector<int> orders_customer[max_number_customers];
int overview_orders_customers[max_number_customers][max_number_products];


vector<PII> customer_distance_to_warehouse;
vector<PII> position_drones;
vector<PIPII> possible_jobs;
vector<PII> delivery_commands;

int distance_warehouse_customer(int whs, int cust){
  return compute_distance(row_warehouse[whs], col_warehouse[whs], row_customer[cust], col_customer[cust]);
}

int compute_distance(int r1, int c1, int r2, int c2){
  return ceil(sqrt((r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2)));
}

int my_min(int a, int b){
  return a<b? a:b;
}



int main(){

  scanf("%d %d %d %d %d ", &num_rows, &num_cols, &num_drones, &deadline, &max_load);

  // Products
  scanf("%d ", &num_products);
  for(int i=0; i<num_products; i++) scanf("%d ", &weight_products[i]);

  // Warehouses
  scanf("%d ", &num_warehouses);
  for(int i=0; i < num_warehouses; i++) scanf("%d %d ", &row_warehouse[i], &col_warehouse[i]);
  for(int whs=0; whs < num_warehouses; whs++){
    for(int i=0; i < num_products; i++){
      scanf("%d ", &availability_warehouses[whs][i]);
    }
  }

  // Orders
  scanf("%d ", &num_customers);
  for(int cust=0; cusy < num_customers; cust++){
    scanf("%d %d ", &row_customer[cust], &col_customer[cust]);
    scanf("%d ", &num_orders_customer[cust]);
    for(int i=0; i<num_orders_customer[cust]; i++){
      int prod;
      scanf("%d ", &prod);
      orders_customer[cust].push_back(prod);
      overview_orders_customers[cust][prod] += 1;
    }
  }

  for(int cust=0; cust < num_customers; cust++) sort(orders_customer[cust].begin(), orders_customer[cust].end());

  // Greedy strategy: Choose the order that is satisfieable the quickest.
  for(int i=0; i<num_drones; i++){
    // Initially all drones are at warehouse zero and their initial time is 0
    position_drones.push_back(make_pair(0, 0));
  }

  int current_time = 0;
  while(current_time <= deadline){

    // Set current time to be the minimum over all the times where the drones are available
    int drone_time = position_drones[0].second;
    for(int i=1; i < num_drones; i++) drone_time = my_min(drone_time, position_drones[i].second);

    possible_jobs.clear();
    for(int cust=0; cust<num_customers; cust++){
      // Compute the fastest way to satisfy a job using only one drone
      if(num_orders_customer[cust] == 0) continue;

      // Drones can do a partial job only
      for(int drn=0; drn < num_drones; drn ++){
        if(position_drones[drn].second > current_time) continue;

        int current_load = 0;
        int current_warehouse = position_drones[drn].first;

        int load_operations = 0;
        for(int i=0; i<orders_customer[cust].size(); i++){
          int prod = orders_customer[cust][i];
          // Make sure we do not look at the same product several times
          if(i > 0 && orders_customer[cust][i-1] == prod) continue;
          // Have already satisfied the order on this
          if(overview_orders_customers[cust][prod] == 0) continue;
          // Get the maximum number of elements we can load
          int max_elements = (int)((max_load - current_load) / weight_products[prod]);
          if(availability_warehouses[current_warehouse][prod] > 0 && current_load + weight_products[prod] <= max_load){
            current_load += my_min(max_elements, availability_warehouses[current_warehouse][prod]) * weight_products[prod];
            load_operations += 1;
          }
        }

        if(current_load > 0){
          // Includes each load and delivery
          int time_to_deliver = 2 * load_operations + distance_warehouse_customer(current_warehouse, cust);
          // How long does the job take, who is the customer and which drone is used.
          possible_jobs.push_back(make_pair(time_to_deliver, make_pair(cust, drn)));
        }
      }
    }

    if(possible_jobs.empty()) break;
    sort(possible_jobs.begin(), possible_jobs.end());

    PIPII nxt_job = possible_jobs[0];

    // Assemble the current job and send the drone off to some random warehouse
    int nxt_customer = nxt_job.second.first;
    int usd_drone = nxt_job.second.second;

    // Print which things are being loaded
    int current_load = 0;
    int current_warehouse = position_drones[usd_drone].first;

    delivery_commands.clear();

    for(int i=0; i<orders_customer[nxt_customer].size(); i++){
      int prod = orders_customer[nxt_customer][i];
      if(overview_orders_customers[nxt_customer][prod] == 0) continue;
      if(availability_warehouses[current_warehouse][prod] > 0 && current_load + weight_products[prod] <= max_load){
        current_load += weight_products[prod];
        printf("%d L %d", usd_drone, position_drones[usd_drone].first);
        int max_elements = my_min((int)((max_load - current_load) / weight_products[prod]), availability_warehouses[current_warehouse][prod]);
        printf(" %d %d\n", prod, max_elements);
        // Update availability of objects and fulfilled works
        availability_warehouses[current_warehouse][prod] -= max_elements;
        num_orders_customer[nxt_customer] -= max_elements;
        overview_orders_customers[nxt_customer][prod] -= max_elements;
        delivery_commands.push_back(make_pair(prod, max_elements));
      }
    }

    // All Loading done, now set delivery
    for(int i=0; i<delivery_commands.size(); i++){
        printf("%d D %d %d %d\n", usd_drone, nxt_customer, delivery_commands[i].first, delivery_commands[i].second);
    }

    // Send the drone to a random warehouse.
    int next_warehouse = rand() % num_warehouses;
    int travel_time = distance_warehouse_customer(next_warehouse, nxt_customer);
    int new_time = position_drones[usd_drone] + nxt_job.first + travel_time;
    position_drones[usd_drone] = make_pair(next_warehouse, new_time);

  }
  return 0;
}
