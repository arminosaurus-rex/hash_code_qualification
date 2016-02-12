#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <cstdlib>

using namespace std;

typedef pair<int, int> PII;
typedef pair<int, PII> PIPII;

vector<string> output;

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

int compute_distance(int r1, int c1, int r2, int c2){
  return ceil(sqrt((r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2)));
}

int distance_warehouse_customer(int whs, int cust){
  return compute_distance(row_warehouse[whs], col_warehouse[whs], row_customer[cust], col_customer[cust]);
}

int distance_warehouse_warehouse(int whs1, int whs2){
  return compute_distance(row_warehouse[whs1], col_warehouse[whs1], row_warehouse[whs2], col_warehouse[whs2]);
}
int my_min(int a, int b){
  return a<b? a:b;
}

int three_min(int a, int b, int c){
  return my_min(a, my_min(b, c));
}



int main(){

  scanf("%d %d %d %d %d ", &num_rows, &num_cols, &num_drones, &deadline, &max_load);

  // Products
  scanf("%d ", &num_products);
  for(int i=0; i<num_products; i++) scanf("%d ", &weight_products[i]);

  // Warehouses
  scanf("%d ", &num_warehouses);
  for(int whs=0; whs < num_warehouses; whs++){
    scanf("%d %d ", &row_warehouse[whs], &col_warehouse[whs]);
    for(int j=0; j < num_products; j++){
      scanf("%d ", &availability_warehouses[whs][j]);
    }
  }


  // Orders
  scanf("%d ", &num_customers);
  for(int cust=0; cust < num_customers; cust++){
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
    position_drones.push_back(make_pair(i % num_warehouses, distance_warehouse_warehouse(0, i % num_warehouses)));
  }

  int current_time = 0;
  while(current_time <= deadline){

    //printf("%d\n", current_time);

    // Set current time to be the minimum over all the times where the drones are available
    int drone_time = position_drones[0].second;
    for(int i=1; i < num_drones; i++) drone_time = my_min(drone_time, position_drones[i].second);
    current_time = drone_time;

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
          int max_elements = three_min(availability_warehouses[current_warehouse][prod],
                                (int)((max_load - current_load) / weight_products[prod]),
                              overview_orders_customers[cust][prod]);


          if(max_elements == 0) continue;
          if(max_elements < 0) printf("Error\n");

          current_load += max_elements * weight_products[prod];
          load_operations += 1;
        }

        if(current_load > 0){
          printf(" %d %d %lf \n", current_load, max_load, (double)((1.0 * current_load) / (1.0 * max_load)));
          // Includes each load and delivery
          int time_to_deliver = 2 * load_operations + distance_warehouse_customer(current_warehouse, cust);
          // How long does the job take, who is the customer and which drone is used.
          int factor = time_to_deliver * num_orders_customer[cust];
          possible_jobs.push_back(make_pair(factor, make_pair(cust, drn)));
        }
      }
    }

    if(possible_jobs.empty()){
      //printf("We are here?\n");
      break;
    }
    sort(possible_jobs.begin(), possible_jobs.end());

    //int choose = my_min(2, possible_jobs.size());
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
      if(i > 0 && orders_customer[nxt_customer][i-1] == prod) continue;
      if(overview_orders_customers[nxt_customer][prod] == 0) continue;

      int max_elements = three_min(availability_warehouses[current_warehouse][prod],
                            (int)((max_load - current_load) / weight_products[prod]),
                          overview_orders_customers[nxt_customer][prod]);
      if(max_elements == 0) continue;

      current_load += max_elements * weight_products[prod];
      //cout << current_warehouse << " " << prod << " " << availability_warehouses[current_warehouse][prod] << endl;
      string s = to_string(usd_drone) + " L " + to_string(position_drones[usd_drone].first) + " "  + to_string(prod) + " " + to_string(max_elements);
      output.push_back(s);
      //printf("%d L %d %d %d\n", usd_drone, position_drones[usd_drone].first, prod, max_elements);
      // Update availability of objects and fulfilled works
      availability_warehouses[current_warehouse][prod] -= max_elements;
      num_orders_customer[nxt_customer] -= max_elements;
      overview_orders_customers[nxt_customer][prod] -= max_elements;
      delivery_commands.push_back(make_pair(prod, max_elements));
    }


    // All Loading done, now set delivery
    for(int i=0; i<delivery_commands.size(); i++){
        //printf("%d D %d %d %d\n", usd_drone, nxt_customer, delivery_commands[i].first, delivery_commands[i].second);
        string s = to_string(usd_drone) + " D "  + to_string(nxt_customer) + " " + to_string(delivery_commands[i].first) + " " + to_string(delivery_commands[i].second);
        output.push_back(s);
    }


    int mx = 0;
    int nwhs = 0;
    for(int i=0; i<num_warehouses; i++){
        int sum = 0;
        for(int j=0; j<num_products; j++){
          sum += availability_warehouses[i][j];
        }
        if(sum > mx){mx = sum; nwhs = i;}
    }

    // Send the drone to a random warehouse.
    int next_warehouse = nwhs; //current_warehouse;
    int travel_time = distance_warehouse_customer(next_warehouse, nxt_customer);
    int new_time = position_drones[usd_drone].second + nxt_job.first + travel_time;
    position_drones[usd_drone] = make_pair(next_warehouse, new_time);

  }

/*
  int not_served = 0;
  for(int i=0; i<num_customers; i++){
    if(num_orders_customer[i] > 0) not_served += 1;
    printf("%d ", num_orders_customer[i]);
  }
  printf("\n");

  printf("Not served: %d\n", not_served);
*/

  //printf("%d\n", output.size());


  cout << output.size() << endl;
  for(int i=0; i<output.size(); i++){
    cout << output[i] << endl;
  }
  return 0;
}
