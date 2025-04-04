
#include "tourist/MultiTourSolver.h"

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

#include "tourist/ECCRU.h"
#include "tourist/ECCRUContainer.h"
#include "tourist/POI.h"
#include "tourist/POIContainer.h"
#include "tourist/Tour.h"
#include "tourist/TourContainer.h"
#include "tourist/User.h"
#include "tourist/UserContainer.h"
#include "tourist/Visit.h"

#define EPS 0.0001

using namespace boost;

POIContainer* multi_poic_global;
UserContainer* userc_global;
ECCRUContainer* eccru_container_global;
int multi_t = 0;

struct MultiVertexProperty {
  MultiVertexProperty(int id = 0, double profit = 0,
                      vector<int> allocation_in_time = vector<int>(),
                      Tour* tour = NULL)
      : id_(id), profit_(profit), allocation_in_time_(allocation_in_time) {
    tour_ = tour;
  }

  int id_;
  double profit_;
  vector<int> allocation_in_time_;
  Tour* tour_;
};

struct MultiEdgeProperty {
  MultiEdgeProperty(int num = 0, int eccru_id = 0)
      : num_(num), eccru_id_(eccru_id) {}
  int num_;
  int eccru_id_;
};

typedef adjacency_list<vecS, vecS, directedS, MultiVertexProperty,
                       MultiEdgeProperty>
    ItineraryGraph;

struct MultiResourceContainer {
  MultiResourceContainer(double profit = 0, int time_created = 0,
                         ECCRUContainer eccru_container = ECCRUContainer())
      : profit_(profit), time_created_(time_created) {
    bw_resource_.assign(multi_poic_global->GetNumOfPOIs() + 1, 150);
    eccru_container_ = eccru_container;
    allocation_cost_ = 0;
  }

  MultiResourceContainer& operator=(const MultiResourceContainer& other) {
    if (this == &other) return *this;
    this->~MultiResourceContainer();
    new (this) MultiResourceContainer(other);
    return *this;
  }

  double profit_;
  ECCRUContainer eccru_container_;
  vector<Tour*> tours_;
  std::vector<double> bw_resource_;
  std::vector<int> allocation_history_;
  int time_created_;
  int allocation_cost_;
};

bool operator==(const MultiResourceContainer& res_cont_1,
                const MultiResourceContainer& res_cont_2) {
  return res_cont_1.profit_ == res_cont_2.profit_ &&
         res_cont_1.allocation_cost_ == res_cont_2.allocation_cost_ &&
         res_cont_1.time_created_ == res_cont_2.time_created_;
}

bool operator<(const MultiResourceContainer& res_cont_1,
               const MultiResourceContainer& res_cont_2) {
  if (!(abs(res_cont_1.profit_ - res_cont_2.profit_) < EPS)) {
    return res_cont_1.profit_ < res_cont_2.profit_;
  } else if (!(abs(res_cont_1.allocation_cost_ - res_cont_2.allocation_cost_) <
               EPS)) {
    return res_cont_1.allocation_cost_ < res_cont_2.allocation_cost_;
  } else {
    return res_cont_1.time_created_ <= res_cont_2.time_created_;
  }
}

class MultiExtensionFunction {
 public:
  inline bool operator()(
      const ItineraryGraph& g, MultiResourceContainer& new_cont,
      const MultiResourceContainer& old_cont,
      graph_traits<ItineraryGraph>::edge_descriptor ed) const {
    const MultiEdgeProperty& edge_prop = get(edge_bundle, g)[ed];
    const MultiVertexProperty& vert_prop_t =
        get(vertex_bundle, g)[target(ed, g)];
    const MultiVertexProperty& vert_prop_s =
        get(vertex_bundle, g)[source(ed, g)];

    new_cont.time_created_ = multi_t++;
    // new_cont.path.push_back(vert_prop_t.poi_id);
    // new_cont.travel_spent.push_back(edge_prop.cost);
    // new_cont.path_spent.push_back(vert_prop_t.time_to_spend);
    // new_cont.budget_spent = old_cont.budget_spent + edge_prop.cost +
    // vert_prop_t.time_to_spend;
    //
    new_cont.profit_ = old_cont.profit_ + vert_prop_t.profit_;

    new_cont.allocation_cost_ = old_cont.allocation_cost_;

    new_cont.allocation_history_.push_back(edge_prop.eccru_id_);
    int cost = 0;
    bool ok = new_cont.eccru_container_.AllocateResource(
        vert_prop_t.allocation_in_time_, edge_prop.eccru_id_, &cost);
    new_cont.allocation_cost_ += cost;
    return ok;
  }
};

class DominanceFunction {
 public:
  inline bool operator()(const MultiResourceContainer& res_cont_1,
                         const MultiResourceContainer& res_cont_2) const {
    return res_cont_1.profit_ >= res_cont_2.profit_ &&
           res_cont_1.allocation_cost_ <= res_cont_2.allocation_cost_;
  }
};

void MultiTourSolver::Solve(POIContainer* poic, UserContainer* uc,
                            vector<TourContainer*> generated_solutions) {
  multi_t = 1;
  multi_poic_global = poic;
  userc_global = uc;

  ItineraryGraph g;

  ECCRUContainer eccru_c_nr = ECCRUContainer();
  eccru_c_nr.AddECCRU(ECCRU(1000, 0));
  eccru_c_nr.AddECCRU(ECCRU(1000, 0));

  int edge_num = 0;
  int itinerary_num = 1;

  cout << "Num of users: " << generated_solutions.size() << endl;

  for (int i = 0; i < generated_solutions.size(); i++) {
    cout << "Iineraries for user :" << i + 1 << endl;
    for (int j = 0; j < generated_solutions[i]->GetNumOfTours(); j++) {
      cout << itinerary_num++ << ": ";
      generated_solutions[i]->GetTour(j)->PrintTour();
    }
  }

  itinerary_num = 1;
  vector<int> null_alocation;
  null_alocation.assign(1441, 0);

  add_vertex(MultiVertexProperty(0, 0, null_alocation, NULL), g);

  for (int i = 0; i < generated_solutions.size(); i++) {
    for (int j = 0; j < generated_solutions[i]->GetNumOfTours(); j++) {
      Tour* tour = generated_solutions[i]->GetTour(j);
      cout << "Adding vertex " << itinerary_num << endl;
      tour->PrintTour();
      add_vertex(MultiVertexProperty(itinerary_num++, tour->GetProfit(),
                                     tour->GetAllocation(), tour),
                 g);
    }
  }

  add_vertex(MultiVertexProperty(itinerary_num, 0, null_alocation, NULL), g);

  vector<int> past_itineraries;
  vector<int> next_itineraries;

  for (int i = 0, cur_it = 1; i < generated_solutions.size(); i++) {
    past_itineraries = next_itineraries;
    next_itineraries.clear();
    for (int j = 0; j < generated_solutions[i]->GetNumOfTours(); j++) {
      next_itineraries.push_back(cur_it);
      if (i == 0) {
        for (int k = 1; k <= 2; k++) {
          add_edge(0, cur_it, MultiEdgeProperty(edge_num++, k), g);
        }
        cur_it++;
      } else {
        for (int l = 0; l < past_itineraries.size(); l++) {
          for (int k = 1; k <= 2; k++) {
            add_edge(past_itineraries[l], cur_it,
                     MultiEdgeProperty(edge_num++, k), g);
          }
        }
        cur_it++;
      }
    }
  }

  for (int l = 0; l < next_itineraries.size(); l++) {
    for (int k = 1; k <= 2; k++) {
      cout << "connecting " << next_itineraries[l] << " with " << itinerary_num
           << endl;
      add_edge(next_itineraries[l], itinerary_num,
               MultiEdgeProperty(edge_num++, k), g);
    }
  }

  graph_traits<ItineraryGraph>::vertex_descriptor sv = 0;
  graph_traits<ItineraryGraph>::vertex_descriptor tv = itinerary_num;

  std::vector<std::vector<graph_traits<ItineraryGraph>::edge_descriptor> >
      opt_solutions;
  std::vector<MultiResourceContainer> pareto_opt_rcs;

  r_c_shortest_paths(
      g, get(&MultiVertexProperty::id_, g), get(&MultiEdgeProperty::num_, g),
      sv, tv, opt_solutions, pareto_opt_rcs,
      MultiResourceContainer(0, 0, eccru_c_nr), MultiExtensionFunction(),
      DominanceFunction(),
      std::allocator<
          r_c_shortest_paths_label<ItineraryGraph, MultiResourceContainer> >(),
      default_r_c_shortest_paths_visitor());

  double best_profit = -1;
  int best_index = -1;

  cout << "Num sols: " << static_cast<int>(opt_solutions.size()) << endl;

  for (int i = 0; i < static_cast<int>(opt_solutions.size()); i++) {
    if (best_profit < pareto_opt_rcs[i].profit_) {
      best_profit = pareto_opt_rcs[i].profit_;
      best_index = i;
    }

    cout << "Profit sol " << i << ": " << pareto_opt_rcs[i].profit_ << endl;
  }
  /*for(int j = static_cast<int>(opt_solutions[i].size()) - 2, o = 0; j >= 0;
  j--, o++) { POI* p = poic->GetPOIById(source(opt_solutions[i][j], g)); cout <<
  p->GetId() << " {"; for (int k = 0; k <
  pareto_opt_rcs[i].allocation_history[o].size(); k++) { cout <<
  pareto_opt_rcs[i].allocation_history[o][k].first << " : " <<
  pareto_opt_rcs[i].allocation_history[o][k].second << ", ";
          }
          cout << "} -> ";

          if (j == 0) {
                  POI* p = poic->GetPOIById(target(opt_solutions[i][j], g));
                  o++;
                  cout << p->GetId() << " {";
                  for (int k = 0; k <
  pareto_opt_rcs[i].allocation_history[o].size(); k++) { cout <<
  pareto_opt_rcs[i].allocation_history[o][k].first << " : " <<
  pareto_opt_rcs[i].allocation_history[o][k].second << ", ";
                  }
                  cout << "}";
          }
  }
  cout << endl;*/
  /*
                  for(int j = static_cast<int>(opt_solutions[i].size()) - 2, o =
     0; j >= 0; j--, o++) { POI* p =
     poic->GetPOIById(source(opt_solutions[i][j], g)); cout << p->GetId() << "
     {"; for (int k = 0; k < pareto_opt_rcs[i].allocation_history.size(); k++) {
                                  cout <<
     pareto_opt_rcs[i].allocation_history[k] << " : 1000, ";
                          }
                          cout << "} -> ";

                          if (j == 0) {
                                  POI* p =
     poic->GetPOIById(target(opt_solutions[i][j], g)); o++; cout << p->GetId()
     << " {"; for (int k = 0; k < pareto_opt_rcs[i].allocation_history.size();
     k++) { cout << pareto_opt_rcs[i].allocation_history[k] << " : 1000, ";
                                  }
                                  cout << "}";
                          }
                  }
                  cout << endl;
          }

          if (best_index != -1) {
                  for(int j = static_cast<int>(opt_solutions[best_index].size())
     - 2; j >= 0; j--) { POI* p =
     poic->GetPOIById(source(opt_solutions[best_index][j], g)); double spent =
     global_time_to_spend[p->GetId()]; Visit* new_v = new
     Visit(source(opt_solutions[best_index][j], g), 0, spent, spent);
                          new_s->AddVisit(new_v);
                  }

                  POI* p = poic->GetPOIById(lastPOIId);
                  double spent = global_time_to_spend[p->GetId()];
                  Visit* new_v = new Visit(lastPOIId, 0, spent, spent);
                  new_s->AddVisit(new_v);
          }*/

  // return new_s;
}
