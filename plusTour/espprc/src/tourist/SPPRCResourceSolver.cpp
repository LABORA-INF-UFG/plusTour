/*#include "tourist/SPPRCResourceSolver.h"

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
#include "tourist/User.h"
#include "tourist/UserContainer.h"
#include "tourist/Visit.h"

#define EPS 0.0001

using namespace boost;

int min_cost = std::numeric_limits<int>::max();
double budget_max;
POIContainer* poic_global;
User* user_global;
Tour* seq_global;
ECCRUContainer* eccru_container_global;
int firstPOIId;
int lastPOIId;
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
        EdgeProperty(int n = 0, double c = 0, int eid = 0)
        : num(n), cost(c), eccru_id(eid) {	}
        int num;
        double cost;
        int eccru_id;
};

struct Visitor {
        template<class Label, class Graph>
        void on_label_popped( const Label& l, const Graph& ) {}
        template<class Label, class Graph>
        void on_label_feasible( const Label&, const Graph& ) {}
        template<class Label, class Graph>
        void on_label_not_feasible( const Label&, const Graph& ) {}
        template<class Label, class Graph>
        void on_label_dominated( const Label& l, const Graph& ) {}
        template<class Label, class Graph>
        void on_label_not_dominated( const Label&, const Graph& ) {}
        template<class Queue, class Graph>
        bool on_enter_loop(const Queue& queue, const Graph& graph) {return
true;}
};

typedef adjacency_list<vecS,
                                                                                         vecS,
                                                                                         directedS,
                                                                                         VertexProperty,
                                                                                         EdgeProperty>
        Graph;

struct ResourceContainer {
        ResourceContainer(double bs = 0,
                                          double p = 0,
                                          int tc = 0,
                                          ECCRUContainer eccru_container =
ECCRUContainer()) : budget_spent( bs ), profit( p ), time_created(tc) {
                bw_resource.assign(poic_global->GetNumOfPOIs() + 1, 150);
                eccru_container_r = eccru_container;
                allocation_cost = 0;
        }

        ResourceContainer& operator=(const ResourceContainer& other) {
                if(this == &other)
                        return *this;
                this->~ResourceContainer();
                new(this) ResourceContainer(other);
                return *this;
        }

        double budget_spent;
        double profit;
        std::map<int, int> visited;
        std::vector<int> path;
        std::vector<double> path_spent;
        std::vector<double> travel_spent;
        ECCRUContainer eccru_container_r;
        std::vector<double> bw_resource;
        std::vector<int> allocation_history;
        int time_created;
        int allocation_cost;
};

bool operator==(const ResourceContainer& res_cont_1,
                                const ResourceContainer& res_cont_2) {
        bool check = true;
        bool check2 = true;

        for (int i = 0; i < min(res_cont_1.path.size(), res_cont_2.path.size());
i++) { if (res_cont_1.path[i] != res_cont_2.path[i]) { check = false; break;
                }
        }

        /*for (int i = 0; i < min(res_cont_1.allocation_history.size(),
res_cont_2.allocation_history.size()); i++) { if
(res_cont_1.allocation_history[i].size() ==
res_cont_2.allocation_history[i].size()) { for (int j = 0; j <
min(res_cont_1.allocation_history[i].size(),
res_cont_2.allocation_history[i].size()); j++) { if
(res_cont_1.allocation_history[i][j].first !=
res_cont_2.allocation_history[i][j].first ||
                                        res_cont_1.allocation_history[i][j].second
!= res_cont_2.allocation_history[i][j].second) { check2 = false; break;
                                }
                        }
                } else {
                        check2 = false;
                        break;
                }
        }*/
/*
return 	res_cont_1.budget_spent == res_cont_2.budget_spent &&
                res_cont_1.profit == res_cont_2.profit &&
                res_cont_1.path.size() == res_cont_2.path.size() &&
                check &&
                check2;
}

bool operator<(	const ResourceContainer& res_cont_1,
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
inline bool operator()(	const Graph& g,
                                                ResourceContainer& new_cont,
                                                const ResourceContainer&
old_cont, graph_traits <Graph>::edge_descriptor ed) const {

        const EdgeProperty& edge_prop = get(edge_bundle, g)[ed];
        const VertexProperty& vert_prop_t = get(vertex_bundle, g)[target( ed, g
)]; const VertexProperty& vert_prop_s = get(vertex_bundle, g)[source( ed, g )];

        if (new_cont.visited[vert_prop_t.poi_id] == 1) {
                return false;
        }

        new_cont.time_created = t++;
        new_cont.path.push_back(vert_prop_t.poi_id);
        new_cont.travel_spent.push_back(edge_prop.cost);
        new_cont.path_spent.push_back(vert_prop_t.time_to_spend);
        new_cont.budget_spent = old_cont.budget_spent + edge_prop.cost +
vert_prop_t.time_to_spend;

        new_cont.profit = old_cont.profit + ((eta_ *
global_interest[(poic_global->GetPOIById(vert_prop_t.poi_id))->GetCategory()]) +
                                        ((1 - eta_) *
(poic_global->GetPOIById(vert_prop_t.poi_id))->GetPopularity()));

        new_cont.visited[vert_prop_t.poi_id] = 1;
        new_cont.allocation_cost = old_cont.allocation_cost;


        /*if (new_cont.visited[vert_prop_s.poi_id] != 1) {
                new_cont.visited[vert_prop_s.poi_id] = 1;
                new_cont.budget_spent += vert_prop_s.time_to_spend;
                new_cont.profit = old_cont.profit + ((eta_ *
global_interest[(poic_global->GetPOIById(vert_prop_s.poi_id))->GetCategory()]) +
                                                ((1 - eta_) *
(poic_global->GetPOIById(vert_prop_t.poi_id))->GetPopularity()));
        }*/
/*
if (new_cont.budget_spent > budget_max) {
        return false;
}

new_cont.bw_resource[vert_prop_t.poi_id] -= 150;

if (new_cont.bw_resource[vert_prop_t.poi_id] < 0)
        return false;

new_cont.allocation_history.push_back(edge_prop.eccru_id);
int cost;
bool ok = new_cont.eccru_container_r.AllocateResource( (old_cont.budget_spent +
edge_prop.cost) / 60, new_cont.budget_spent / 60, 1000, edge_prop.eccru_id,
&cost);
}
};

class DominanceFunction {
public:
inline bool operator()(	const ResourceContainer& res_cont_1,
                                        const ResourceContainer& res_cont_2)
const {

return 	res_cont_1.profit >= res_cont_2.profit &&
                res_cont_1.budget_spent <= res_cont_2.budget_spent &&
                res_cont_1.allocation_cost <= res_cont_2.allocation_cost;
}

};


Tour* SPPRCResourceSolver::Solve(POIContainer* poic, ECCRUContainer* eccru_c,
User* u, Tour * s, double eta, int interest_type) { Tour* new_s = new
Tour(s->GetSeqId()); t = 1; poic_global = poic; user_global = u; seq_global = s;
num_of_vertex = poic->GetNumOfPOIs();
eta_ = eta;
budget_max = s->GetDuration(poic);
double time_to_spend_origin = 0;
double profit_origin = 0;
interest_type_global = interest_type;
Graph g;
std::vector<std::vector<int> > eccrus_permutations;
std::vector<int> aux_permutations;
ECCRUContainer eccru_c_nr = ECCRUContainer();
eccru_c_nr.AddECCRU(ECCRU(1000, 0));
eccru_c_nr.AddECCRU(ECCRU(1000, 0));
int edge_num = 0;

for (int i = 1; i < eccru_c->GetNumOfECCRUs(); i++) {
aux_permutations.push_back(i);
}
do {
eccrus_permutations.push_back(aux_permutations);
} while (std::next_permutation(aux_permutations.begin(),
aux_permutations.end()));

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

for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
POI* p = poic->GetPOIById(i);
global_time_to_spend[p->GetId()] = global_interest[p->GetCategory()] *
p->GetAvgVisitDuration();
}

for (int i = 0; i < s->GetSeqLength(); i++) {
Visit* v = s->GetVisit(i);
if (i == 0) {
        firstPOIId = v->GetPOIId();
}

if (i == s->GetSeqLength() - 1) {
        lastPOIId = v->GetPOIId();
}
}

add_vertex(VertexProperty(0, 1000000), g);

for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
POI* p = poic->GetPOIById(i);
string category = p->GetCategory();
double time_to_spend = global_time_to_spend[p->GetId()];

if (p->GetId() == firstPOIId) {
        time_to_spend_origin = global_time_to_spend[p->GetId()];
        profit_origin = ((eta_ * global_interest[p->GetCategory()]) +
                                        ((1 - eta_) *
(poic_global->GetPOIById(firstPOIId))->GetPopularity()));
}

add_vertex(VertexProperty(i, time_to_spend), g);
}

if (time_to_spend_origin > budget_max)
return new_s;

for (int k = 0; k <= 2; k++) {
add_edge(0, firstPOIId, EdgeProperty(edge_num++, 0, k), g);
}

for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
vertex_visited[i] = 0;
for (int j = 1; j <= poic->GetNumOfPOIs(); j++) {
        if (i == j) continue;
        for (int k = 1; k <= 2; k++) {
                add_edge(i, j, EdgeProperty(edge_num++,
poic->GetTimeBetweenPOIs(i, j, "walking"), k), g);
                /*for (int l = 0; l < eccrus_permutations[k].size(); l++)
                        cout << eccrus_permutations[k][l] << " ";
                cout << endl;*//*
			}
		}
	}

	graph_traits<Graph>::vertex_descriptor sv = 0;
	graph_traits<Graph>::vertex_descriptor tv = lastPOIId;
	
	std::vector
		<std::vector
			<graph_traits<Graph>::edge_descriptor> > 
				opt_solutions;
	std::vector<ResourceContainer> pareto_opt_rcs;

	r_c_shortest_paths
	( 	g, 
		get( &VertexProperty::poi_id, g), 
		get( &EdgeProperty::num, g), 
		sv, 
		tv, 
		opt_solutions, 
		pareto_opt_rcs, 
		ResourceContainer(0, 0, 1, eccru_c_nr), 
		ExtensionFunction(), 
		DominanceFunction(), 
		std::allocator
			<r_c_shortest_paths_label
				<Graph, ResourceContainer> >(), 
					Visitor());
	
	double best_profit = -1;
	int best_index = -1;

	cout << "Num sols: " << static_cast<int>(opt_solutions.size()) << endl;

	for(int i = 0; i < static_cast<int>(opt_solutions.size()); i++) {
		if (best_profit < pareto_opt_rcs[i].profit) {
			best_profit = pareto_opt_rcs[i].profit;
			best_index = i;
		}

		cout << "Profit sol " << i << ": " << pareto_opt_rcs[i].profit << endl;
		/*for(int j = static_cast<int>(opt_solutions[i].size()) - 2, o = 0; j >= 0; j--, o++) {
			POI* p = poic->GetPOIById(source(opt_solutions[i][j], g));
			cout << p->GetId() << " {";
			for (int k = 0; k < pareto_opt_rcs[i].allocation_history[o].size(); k++) {
				cout << pareto_opt_rcs[i].allocation_history[o][k].first << " : " << pareto_opt_rcs[i].allocation_history[o][k].second << ", ";
			}
			cout << "} -> ";

			if (j == 0) {
				POI* p = poic->GetPOIById(target(opt_solutions[i][j], g));
				o++;
				cout << p->GetId() << " {";
				for (int k = 0; k < pareto_opt_rcs[i].allocation_history[o].size(); k++) {
					cout << pareto_opt_rcs[i].allocation_history[o][k].first << " : " << pareto_opt_rcs[i].allocation_history[o][k].second << ", ";
				}
				cout << "}";
			}
		}
		cout << endl;*/
/*
                for(int j = static_cast<int>(opt_solutions[i].size()) - 2, o =
   0; j >= 0; j--, o++) { POI* p = poic->GetPOIById(source(opt_solutions[i][j],
   g)); cout << p->GetId() << " {"; for (int k = 0; k <
   pareto_opt_rcs[i].allocation_history.size(); k++) { cout <<
   pareto_opt_rcs[i].allocation_history[k] << " : 1000, ";
                        }
                        cout << "} -> ";

                        if (j == 0) {
                                POI* p =
   poic->GetPOIById(target(opt_solutions[i][j], g)); o++; cout << p->GetId() <<
   " {"; for (int k = 0; k < pareto_opt_rcs[i].allocation_history.size(); k++) {
                                        cout <<
   pareto_opt_rcs[i].allocation_history[k] << " : 1000, ";
                                }
                                cout << "}";
                        }
                }
                cout << endl;
        }

        if (best_index != -1) {
                for(int j = static_cast<int>(opt_solutions[best_index].size()) -
   2; j >= 0; j--) { POI* p =
   poic->GetPOIById(source(opt_solutions[best_index][j], g)); double spent =
   global_time_to_spend[p->GetId()]; Visit* new_v = new
   Visit(source(opt_solutions[best_index][j], g), 0, spent, spent);
                        new_s->AddVisit(new_v);
                }

                POI* p = poic->GetPOIById(lastPOIId);
                double spent = global_time_to_spend[p->GetId()];
                Visit* new_v = new Visit(lastPOIId, 0, spent, spent);
                new_s->AddVisit(new_v);
        }

        return new_s;
*/