#include "tourist/SPPRCSolver.h"

#include <algorithm>
#include <bitset>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/r_c_shortest_paths.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "tourist/POI.h"
#include "tourist/POIContainer.h"
#include "tourist/Tour.h"
#include "tourist/TourContainer.h"
#include "tourist/User.h"
#include "tourist/UserContainer.h"
#include "tourist/Visit.h"

#define EPS 0.0001

using namespace boost;

double budget_max;
POIContainer* poic_global;
User* user_global;
Tour* seq_global;
int firstPOIIndex;
int lastPOIIndex;
int num_of_vertex;
double eta_;
std::bitset<100> vertex_visited;
int t = 0;
int interest_type_global;
std::map<std::string, double> global_interest;
std::map<int, double> global_time_to_spend;

struct VertexProperty {
  VertexProperty(int pid = 0, double ttspend = 0)
      : poi_id(pid), time_to_spend(ttspend) {}
  int poi_id;
  double time_to_spend;
};

struct EdgeProperty {
  EdgeProperty(int n = 0, double c = 0) : num(n), cost(c) {}
  int num;
  double cost;
};

struct Visitor {
  template <class Label, class Graph>
  void on_label_popped(const Label& l, const Graph&) {}
  template <class Label, class Graph>
  void on_label_feasible(const Label&, const Graph&) {}
  template <class Label, class Graph>
  void on_label_not_feasible(const Label&, const Graph&) {}
  template <class Label, class Graph>
  void on_label_dominated(const Label& l, const Graph&) {}
  template <class Label, class Graph>
  void on_label_not_dominated(const Label&, const Graph&) {}
  template <class Queue, class Graph>
  bool on_enter_loop(const Queue& queue, const Graph& graph) {
    return true;
  }
};

typedef adjacency_list<vecS, vecS, directedS, VertexProperty, EdgeProperty>
    Graph;

struct ResourceContainer {
  ResourceContainer(double bs = 0, double p = 0, int tc = 0)
      : budget_spent(bs), profit(p), time_created(tc) {}

  ResourceContainer& operator=(const ResourceContainer& other) {
    if (this == &other) return *this;
    this->~ResourceContainer();
    new (this) ResourceContainer(other);
    return *this;
  }

  double budget_spent;
  double profit;
  std::map<int, int> visited;
  std::vector<int> path;
  std::vector<double> path_spent;
  std::vector<double> travel_spent;
  int time_created;
};

bool operator==(const ResourceContainer& res_cont_1,
                const ResourceContainer& res_cont_2) {
  bool check = true;

  for (int i = 0; i < min(res_cont_1.path.size(), res_cont_2.path.size());
       i++) {
    if (res_cont_1.path[i] != res_cont_2.path[i]) {
      check = false;
      break;
    }
  }

  return res_cont_1.budget_spent == res_cont_2.budget_spent &&
         res_cont_1.profit == res_cont_2.profit &&
         res_cont_1.path.size() == res_cont_2.path.size() && check;
}

bool operator<(const ResourceContainer& res_cont_1,
               const ResourceContainer& res_cont_2) {
  if (!(abs(res_cont_1.budget_spent - res_cont_2.budget_spent) < EPS)) {
    return res_cont_1.budget_spent < res_cont_2.budget_spent;
  } else if (!(abs(res_cont_1.profit - res_cont_2.profit) < EPS)) {
    return res_cont_1.profit < res_cont_2.profit;
  } else {
    return res_cont_1.time_created <= res_cont_2.time_created;
  }
}

class ExtensionFunction {
 public:
  inline bool operator()(const Graph& g, ResourceContainer& new_cont,
                         const ResourceContainer& old_cont,
                         graph_traits<Graph>::edge_descriptor ed) const {
    const EdgeProperty& edge_prop = get(edge_bundle, g)[ed];
    const VertexProperty& vert_prop_t = get(vertex_bundle, g)[target(ed, g)];
    const VertexProperty& vert_prop_s = get(vertex_bundle, g)[source(ed, g)];

    if (new_cont.visited[vert_prop_t.poi_id] == 1) {
      return false;
    }

    new_cont.time_created = t++;

    new_cont.path.push_back(vert_prop_t.poi_id);

    /*for (int i = 0; i < old_cont.travel_spent.size(); i++)
            new_cont.travel_spent.push_back(old_cont.travel_spent[i]);*/

    new_cont.travel_spent.push_back(edge_prop.cost);
    new_cont.path_spent.push_back(vert_prop_t.time_to_spend);
    new_cont.budget_spent =
        old_cont.budget_spent + edge_prop.cost + vert_prop_t.time_to_spend;

    vector<int> li = poic_global->GetIndexList();
    new_cont.profit =
        old_cont.profit +
        ((eta_ * global_interest[(poic_global->GetPOIById(li[vert_prop_t.poi_id]))
                                     ->GetCategory()]) +
         ((1 - eta_) *
          (poic_global->GetPOIById(li[vert_prop_t.poi_id]))->GetPopularity()));

    new_cont.visited[vert_prop_t.poi_id] = 1;

    if (new_cont.budget_spent <= budget_max) {
      return true;
    } else {
      return false;
    }
  }
};

class DominanceFunction {
 public:
  inline bool operator()(const ResourceContainer& res_cont_1,
                         const ResourceContainer& res_cont_2) const {
    return res_cont_1.profit >= res_cont_2.profit &&
           res_cont_1.budget_spent <= res_cont_2.budget_spent;
  }
};

TourContainer* SPPRCSolver::Solve(POIContainer* poic, User* u, Tour* s,
                                  double eta, int interest_type) {
  TourContainer* tour_c = new TourContainer();
  t = 1;
  poic_global = poic;
  user_global = u;
  seq_global = s;
  //num_of_vertex = poic->GetNumOfPOIs();
  eta_ = eta;
  budget_max = s->GetDuration(poic);
  double time_to_spend_origin = 0;
  double profit_origin = 0;
  interest_type_global = interest_type;
  Graph g;

  switch (interest_type) {
    case 1:
      global_interest = u->frequency_based_interest_;
      break;

    case 2:
      global_interest = u->time_based_interest_;
      break;

    case 3:
      global_interest = u->time_based_interest_weighted_;
      break;
  }
    
  vector<int> li = poic->GetIndexList();
  for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
    POI* p = poic->GetPOIById(li[i]);
    global_time_to_spend[p->GetId()] =
        global_interest[p->GetCategory()] * p->GetAvgVisitDuration();
    cout << p->GetId() << ": " << global_time_to_spend[p->GetId()] << endl;
  }

  for (int i = 0; i < s->GetSeqLength(); i++) {
    Visit* v = s->GetVisit(i);
    if (i == 0) {
      firstPOIIndex = poic->GetPOIById(v->GetPOIId())->GetIndex();
    }

    if (i == s->GetSeqLength() - 1) {
      lastPOIIndex = poic->GetPOIById(v->GetPOIId())->GetIndex();
    }
  }

  add_vertex(VertexProperty(0, 1000000), g);

  for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
    POI* p = poic->GetPOIById(li[i]);
    string category = p->GetCategory();
    double time_to_spend = global_time_to_spend[p->GetId()];

    add_vertex(VertexProperty(i, time_to_spend), g);

    if (p->GetId() == li[firstPOIIndex]) {
      time_to_spend_origin = global_time_to_spend[p->GetId()];
      profit_origin =
          ((eta_ * global_interest[p->GetCategory()]) +
           ((1 - eta_) *
            (poic_global->GetPOIById(li[firstPOIIndex]))->GetPopularity()));
    }
  }

  if (time_to_spend_origin > budget_max) return tour_c;

  add_edge(0, firstPOIIndex, EdgeProperty(1, 0), g);

  for (int i = 1, edge_num = 1; i <= poic->GetNumOfPOIs(); i++) {
    vertex_visited[i] = 0;
    for (int j = 1; j <= poic->GetNumOfPOIs(); j++) {
      if (i == j) continue;
      add_edge(
          i, j,
          EdgeProperty(edge_num++, poic->GetTimeBetweenPOIs(li[i], li[j], "walking")),
          g);
    }
  }

  graph_traits<Graph>::vertex_descriptor sv = 0;
  graph_traits<Graph>::vertex_descriptor tv = lastPOIIndex;

  std::vector<std::vector<graph_traits<Graph>::edge_descriptor> > opt_solutions;
  std::vector<ResourceContainer> pareto_opt_rcs;

  r_c_shortest_paths(
      g, get(&VertexProperty::poi_id, g), get(&EdgeProperty::num, g), sv, tv,
      opt_solutions, pareto_opt_rcs, ResourceContainer(0, 0, 1),
      ExtensionFunction(), DominanceFunction(),
      std::allocator<r_c_shortest_paths_label<Graph, ResourceContainer> >(),
      Visitor());

  double best_profit = -1;
  int best_index = -1;

  cout << "Num sols: " << static_cast<int>(opt_solutions.size()) << endl;
  cout << "Budget MAX: " << budget_max << endl;
  if (budget_max / 60 / 60 > 8) {
    cout << "Budget above limit. Please run again!" << endl;
    exit(1);
  }

  for (int i = 0; i < static_cast<int>(opt_solutions.size()); i++) {
    if (pareto_opt_rcs[i].path.size() < 1) continue;
    Tour* new_s = new Tour(s->GetSeqId());
    vector<int> allocation_in_time;
    vector<vector<int> > allocation_in_time_at_poi;

    allocation_in_time_at_poi.assign(poic->GetMaxId() + 1, vector<int>());
    for (int j = 0; j <= poic->GetNumOfPOIs(); j++)
      allocation_in_time_at_poi[li[j]].assign(480, 0);

    allocation_in_time.assign(480, 0);
    new_s->SetProfit(pareto_opt_rcs[i].profit);
    double aux = 0;

    for (int j = 0; j < pareto_opt_rcs[i].path.size(); j++) {
      POI* p = poic->GetPOIById(li[pareto_opt_rcs[i].path[j]]);
      double spent = global_time_to_spend[p->GetId()];
      Visit* new_v =
          new Visit(p->GetId(), floor(aux), ceil(aux + spent), ceil(spent));
      new_s->AddVisit(new_v);

      for (int k = floor(aux / 60); k <= ceil((aux + spent) / 60); k++) {
        allocation_in_time[k] = 340;
        allocation_in_time_at_poi[p->GetId()][k] = 340;
      }

      aux += spent;
      if (j + 1 < pareto_opt_rcs[i].travel_spent.size()) {
        aux += pareto_opt_rcs[i].travel_spent[j + 1];
      }
    }

    new_s->SetAllocation(allocation_in_time);
    new_s->SetAllocationAtPoi(allocation_in_time_at_poi);
    tour_c->AddTour(new_s);
  }

  return tour_c;
}