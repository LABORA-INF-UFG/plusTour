#include <bitset>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <filesystem>

#include "tourist/City.h"
#include "tourist/CityContainer.h"
#include "tourist/ECCRU.h"
#include "tourist/ECCRUContainer.h"
#include "tourist/MultiTourSolver.h"
#include "tourist/POI.h"
#include "tourist/POIContainer.h"
#include "tourist/SPPRCSolver.h"
#include "tourist/Tour.h"
#include "tourist/TourContainer.h"
#include "tourist/User.h"
#include "tourist/UserContainer.h"
#include "tourist/Visit.h"

using namespace std;

double recall_total;
double precision_total;
int processados;

string to_lower(string s) {
  for (int i = 0; i < s.length(); i++) s[i] = tolower(s[i]);
  return s;
}

void process(string filename) {
  ifstream i_file;
  ofstream o_file;
  string buffer;
  char escape;
  int num_of_pois = 0;
  string filename_lower = to_lower(filename);

  string photo_id;
  string user_id;
  string date_taken;
  string poi_id;
  string poi_theme;
  int poi_freq;
  string seq_id;
  map<string, int> popularity;

  i_file.open(
      ("../Datasets/Model/" + filename + "/userVisits-" + filename + ".csv")
          .c_str());
  o_file.open(
      ("../Datasets/Model/" + filename + "/UserVisits" + filename + ".in")
          .c_str());

  getline(i_file, buffer);

  while (getline(i_file, photo_id, ';')) {
    i_file >> escape;
    getline(i_file, user_id, '"');
    i_file >> escape;
    getline(i_file, date_taken, ';');
    getline(i_file, poi_id, ';');
    i_file >> escape;
    getline(i_file, poi_theme, '"');
    i_file >> escape;
    i_file >> poi_freq;
    i_file >> escape;
    getline(i_file, seq_id);

    o_file << user_id << " " << date_taken << " " << poi_id << " " << seq_id
           << endl;
    popularity[poi_id] = poi_freq;
  }

  string id;
  string name;
  string lat;
  string lng;
  string category;

  i_file.close();
  o_file.close();
  i_file.open(
      ("../Datasets/Model/" + filename + "/POI-" + filename + ".csv").c_str());
  o_file.open(
      ("../Datasets/Model/" + filename + "/POI" + filename + ".in").c_str());

  getline(i_file, buffer);

  while (getline(i_file, id, ';')) {
    getline(i_file, name, ';');
    getline(i_file, lat, ';');
    getline(i_file, lng, ';');
    getline(i_file, category);
    num_of_pois++;
    o_file << id << " " << name << " " << lat << " " << lng << " "
           << popularity[id] << " " << category << endl;
  }

  int from;
  int to;
  string distance;
  int profit;
  string cost[num_of_pois + 1][num_of_pois + 1];

  for (int i = 0; i <= num_of_pois; i++) {
    for (int j = 0; j <= num_of_pois; j++) {
      cost[i][j] = "1000000";
    }
  }

  i_file.close();
  o_file.close();
  i_file.open(("../Datasets/Model/" + filename + "/costProfCat-" + filename +
               "POI-all.csv")
                  .c_str());
  o_file.open(("../Datasets/Model/" + filename + "/CostProft" + filename + ".in")
                  .c_str());

  getline(i_file, buffer);

  while (i_file >> escape) {
    i_file >> from;
    i_file >> escape;
    i_file >> escape;
    i_file >> escape;
    i_file >> to;
    i_file >> escape;
    i_file >> escape;
    i_file >> escape;
    getline(i_file, distance, '"');
    i_file >> escape;
    i_file >> escape;
    i_file >> profit;
    i_file >> escape;
    i_file >> escape;
    i_file >> escape;
    i_file >> category;

    cost[from][to] = distance;
  }

  for (int i = 1; i <= num_of_pois; i++) {
    for (int j = 1; j <= num_of_pois; j++) {
      if (i == j) continue;

      o_file << i << " " << j << " " << cost[i][j] << endl;
    }
  }

  i_file.close();
  o_file.close();
}

void removeSeqAndUpdate(POIContainer* poic, User* u, Tour* s) {
  for (int k = 0; k < s->GetSeqLength(); k++) {
    Visit* v = s->GetVisit(k);
    POI* p = poic->GetPOIById(v->GetPOIId());
    p->RemoveFromAvgVisitDuration(v->GetDuration());
    //p->SetPopularity(p->GetPopularity() - v->GetPhotosTaken());
    p->SetPopularity(p->GetPopularity() - 1);
  }

  u->UpdateInterests(poic, s->GetSeqId(), 1);
}

void addSeqAndUpdate(POIContainer* poic, User* u, Tour* s) {
  for (int k = 0; k < s->GetSeqLength(); k++) {
    Visit* v = s->GetVisit(k);
    POI* p = poic->GetPOIById(v->GetPOIId());
    p->UpdateAvgVisitDuration(v->GetDuration());
    //p->SetPopularity(p->GetPopularity() + v->GetPhotosTaken());
    p->SetPopularity(p->GetPopularity() + 1);
  }

  u->UpdateInterests(poic, -1, 1);
}

void compare(string city_name, Tour* s1, Tour* s2, POIContainer* poic,
             map<string, double> interest, double eta, ofstream& results) {
  results << s1->GetSeqId() << ";";

  if (s2->GetSeqLength() == 0) {
    results << "\"{";

    for (int i = 0; i < s1->GetSeqLength(); i++) {
      results << s1->GetVisit(i)->GetPOIId();
      if (i != s1->GetSeqLength() - 1) {
        results << ", ";
      }
    }

    results << "}\";";
    results << "No solution;No solution;No solution;No solution;No solution;No "
               "solution;No solution;No solution;No solution;";
  } else {
    results << "\"{";

    vector<int> intersec;
    int intersec_size = 0;
    bitset<100> visited_s1;
    bitset<100> visited_s2;
    intersec.assign(poic->GetNumOfPOIs() + 1, 0);

    for (int i = 0; i < s1->GetSeqLength(); i++) {
      visited_s1[s1->GetVisit(i)->GetPOIId()] = 1;
      results << s1->GetVisit(i)->GetPOIId();
      if (i != s1->GetSeqLength() - 1) {
        results << ", ";
      }
    }

    results << "}\";\"{";

    for (int i = 0; i < s2->GetSeqLength(); i++) {
      visited_s2[s2->GetVisit(i)->GetPOIId()] = 1;
      results << s2->GetVisit(i)->GetPOIId();
      if (i != s2->GetSeqLength() - 1) {
        results << ", ";
      }
    }

    results << "}\";";
    results << s1->GetProfit(poic, interest, eta) << ";";
    results << s2->GetProfit(poic, interest, eta) << ";";
    results << s1->GetDuration(poic) << ";";
    results << s2->GetDuration(poic) << ";";
      
    vector<int> li = poic->GetIndexList();
    for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
      if (visited_s1[li[i]] == visited_s2[li[i]] && visited_s1[li[i]] == 1) intersec_size++;
    }

    double recall = (double)intersec_size / visited_s1.count();
    double precision = (double)intersec_size / visited_s2.count();
    recall_total = (recall_total * processados) + recall;
    precision_total = (precision_total * processados) + precision;
    processados++;
    recall_total /= processados;
    precision_total /= processados;

    results << recall << ";" << recall_total << ";";
    results << precision << ";" << precision_total << ";";
  }
}

void saveTour(POIContainer* poic, User* u, Tour* s) {
  double max_budget = s->GetDuration(poic);
  //int num_of_pois = poic->GetNumOfPOIs();
  int num_of_categories = 4;
  int initial_poi = -1;
  int final_poi = -1;

  for (int i = 0; i < s->GetSeqLength(); i++) {
    Visit* v = s->GetVisit(i);
    if (i == 0) {
      initial_poi = v->GetPOIId();
    }

    if (i == s->GetSeqLength() - 1) {
      final_poi = v->GetPOIId();
    }
  }

  ofstream o_file;
  stringstream ss;
  ss << s->GetSeqId();
  string seq_id = ss.str();
  o_file.open(("rotas/seq" + seq_id + ".dat").c_str());

  o_file << "budget = " << max_budget << ";" << endl;
  o_file << "n = " << poic->GetNumOfPOIs() << ";" << endl;
  o_file << "initial_poi = " << initial_poi << ";" << endl;
  o_file << "final_poi = " << final_poi << ";" << endl;
  o_file << endl;

  map<string, double> interesse_tempo = u->time_based_interest_;

  o_file << "categories = {";
  for (map<string, double>::iterator it = interesse_tempo.begin();
       it != interesse_tempo.end(); it++) {
    it++;
    if (it != interesse_tempo.end()) {
      it--;
      o_file << "\"" << it->first << "\""
             << "," << endl;
    } else {
      it--;
      o_file << "\"" << it->first << "\"" << endl;
    }
  }
  o_file << "};" << endl << endl;

  o_file << "interesse = #[" << endl;
  for (map<string, double>::iterator it = interesse_tempo.begin();
       it != interesse_tempo.end(); it++) {
    it++;
    if (it != interesse_tempo.end()) {
      it--;
      o_file << "\"" << it->first << "\""
             << ": " << it->second << "," << endl;
    } else {
      it--;
      o_file << "\"" << it->first << "\""
             << ": " << it->second << endl;
    }
  }
  o_file << "]#;" << endl << endl;

  o_file << "ttravel = [" << endl;
  vector<int> li = poic->GetIndexList();
  for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
    for (int j = i + 1; j <= poic->GetNumOfPOIs(); j++) {
      if (li[i] != li[j]) {
        if (li[i] == li[poic->GetNumOfPOIs()] - 1 && li[j] == li[poic->GetNumOfPOIs()]) {
          o_file << poic->GetTimeBetweenPOIs(li[i], li[j], "walking") << endl;
        } else {
          o_file << poic->GetTimeBetweenPOIs(li[i], li[j], "walking") << "," << endl;
        }
      }
    }
  }
  o_file << "];" << endl << endl;

  o_file << "tvisit = [" << endl;
  for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
    POI* p = poic->GetPOIById(li[i]);
    double interest = u->GetTimeBasedInterest(p->GetCategory());
    double avg_poi = p->GetAvgVisitDuration();
    double tvisit = interest * avg_poi;
    if (li[i] == li[poic->GetNumOfPOIs()]) {
      o_file << li[i] << ": " << tvisit << endl;
    } else {
      o_file << li[i] << ": " << tvisit << "," << endl;
    }
  }
  o_file << "];" << endl << endl;

  o_file << "poi_cat = [" << endl;
  for (int i = 1; i <= poic->GetNumOfPOIs(); i++) {
    POI* p = poic->GetPOIById(li[i]);
    string category = p->GetCategory();
    if (li[i] == li[poic->GetNumOfPOIs()]) {
      o_file << li[i] << ": "
             << "\"" << category << "\"" << endl;
    } else {
      o_file << li[i] << ": "
             << "\"" << category << "\""
             << "," << endl;
    }
  }

  o_file << "];" << endl << endl;

  o_file.close();
}

void load(CityContainer* c_container, string city) {
  int id;
  string name;
  double lat;
  double lng;
  string category;
  int popularity;
  int p1_id, p2_id;
  double value;
  ifstream file;
  string city_lower = to_lower(city);

  POIContainer* p_container = new POIContainer();
  UserContainer* u_container = new UserContainer();
  City* c = new City(city, u_container, p_container);

  // file.open(("data/" + city_lower + "/POI" + city + ".in").c_str());
  file.open(("../Datasets/Model/" + city + "/pois.in").c_str());

  int index_iterator = 1;
  while (file >> id) {
    file >> name;
    file >> lat;
    file >> lng;
    file >> popularity;
    file >> category;
    p_container->AddPoi(new POI(id, name, lat, lng, category, popularity, index_iterator));
    index_iterator++;
  }

  cout << "Processed pois" << endl;

  file.close();
  // file.open(("data/" + city_lower + "/CostProft" + city + ".in").c_str());
  file.open(("../Datasets/Model/" + city + "/pois_travel_time.in").c_str());

  while (file >> p1_id) {
    file >> p2_id;
    file >> value;
    p_container->SetDistanceBetweenPOIs(p1_id, p2_id, value);
  }

  cout << "Processed distances" << endl;

  file.close();
  // file.open(("data/" + city_lower + "/UserVisits" + city + ".in").c_str());
  file.open(("../Datasets/Model/" + city + "/user_visits.in").c_str());

  string user_id;
  int seq_id = -1;
  double date_taken;
  int poi_id = -1;

  string old_user_id;
  int old_seq_id = -1;
  double old_date_taken;
  int old_poi_id = -1;

  User* u;
  Tour* s;
  Visit* v;

  while (file >> user_id) {
    file >> date_taken;
    file >> poi_id;
    file >> seq_id;

    if (user_id != old_user_id) {
      u = new User(user_id);
      s = new Tour(seq_id);
      v = new Visit(poi_id, date_taken);
      s->AddVisit(v);
      u->AddTour(s);
      u_container->AddUser(u);
    } else if (seq_id != old_seq_id) {
      s = new Tour(seq_id);
      v = new Visit(poi_id, date_taken);
      s->AddVisit(v);
      u->AddTour(s);
    } else if (poi_id != old_poi_id) {
      v = new Visit(poi_id, date_taken);
      s->AddVisit(v);
    } else {
      v->UpdateArrivalDeparture(date_taken);
    }

    old_user_id = user_id;
    old_date_taken = date_taken;
    old_poi_id = poi_id;
    old_seq_id = seq_id;
  }

  file.close();

  cout << "Processed visits" << endl;

  // Processing average by poi

  for (int i = 0; i < u_container->users_.size(); i++) {
    u = u_container->GetUserByPos(i);
    for (int j = 0; j < u->GetNumOfSeqs(); j++) {
      s = u->GetTourByPos(j);
      for (int k = 0; k < s->GetSeqLength(); k++) {
        v = s->GetVisit(k);
        (p_container->GetPOIById(v->GetPOIId()))
            ->UpdateAvgVisitDuration(v->GetDuration());
      }
    }
  }

  cout << "Processed average" << endl;

  c_container->AddCity(c);
    cout << "\n" << endl;
}

int main(int argc, char* argv[]) {
  int num_of_cities = 0;
  vector<string> cities;
    
  ifstream file_of_cities;
  string name_of_the_city;
  file_of_cities.open("file_of_cities.in");
  
  string cty = argv[4];
  if(cty.compare("all")!=0){
    cities.push_back(cty);
    num_of_cities++;
    cout << cty << endl;
  }else{
    while(file_of_cities >> name_of_the_city){
        if(name_of_the_city[0] == '#') continue;
        
        cities.push_back(name_of_the_city);
        num_of_cities++;
        cout << name_of_the_city << endl;
    }
  }
  file_of_cities.close();
  int total = 0;
  double eta;
  int interest_type;
  CityContainer* c_container = new CityContainer();
  ECCRUContainer* eccru_container = new ECCRUContainer();
  eccru_container->AddECCRU(ECCRU(1000, 0));
  eccru_container->AddECCRU(ECCRU(1000, 0));
    

  if (argc != 5) {
    cout
        << "Uso: tourist -rotas <eta_value> <interest_type> <city> or <all>, para gerar as rotas, "
           "ou tourist -solve <eta_value> <interest_type> <city> or <all>, para resolver o problema"
        << endl;
    return -1;
  }

  string argvstr = argv[1];

  if (argvstr.compare("-rotas") == 0 && argvstr.compare("-solve") == 0) {
    cout << argv[1] << endl;
    cout
        << "Uso: tourist -rotas <eta_value> <interest_type> <city> or <all>, para gerar as rotas, "
           "ou tourist -solve <eta_value> <interest_type> <city> or <all>, para resolver o problema"
        << endl;
    return -1;
  }

  eta = atof(argv[2]);
  interest_type = atoi(argv[3]);

  cout << "\n" << endl;
  for (int i = 0; i < num_of_cities; i++) {
    // process(cities[i]);
    load(c_container, cities[i]);
  }
  cout << "\n" << endl;

  srand(1);
  for (int k = 0; k < c_container->GetNumOfCities(); k++) {
    ofstream result_file;
    vector<TourContainer*> generated_solutions;
    City* c = c_container->GetCityByPos(k);
    // if (c->GetName() != "london") continue;
    cout << c->GetName() << endl;
    total = 0;
    recall_total = 0;
    precision_total = 0;
    processados = 0;
    if (eta == 0) {
      result_file.open(("results/espprc/" + c->GetName() +
                        "/results" + c->GetName() + "PT-0.csv")
                           .c_str(),
                       fstream::out);
    } else {
      if (interest_type == 1 && eta == 0.5)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-.5F.csv")
                             .c_str());
      if (interest_type == 1 && eta == 1)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-1F.csv")
                             .c_str());
      if (interest_type == 2 && eta == 0.5)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-.5T.csv")
                             .c_str());
      if (interest_type == 2 && eta == 1)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-1T.csv")
                             .c_str());
      if (interest_type == 3 && eta == 0.5)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-.5U.csv")
                             .c_str());
      if (interest_type == 3 && eta == 1)
        result_file.open(("results/espprc/" + c->GetName() +
                          "/results" + c->GetName() + "PT-1U.csv")
                             .c_str());
    }
    result_file << "seq_id;Rota Real;Rota Gerada;Profit Real;Profit "
                   "Gerado;Budget Real;Budget Gerado;Recall;Recall "
                   "Total;Precision; Precision Total;Tempo;Tempo Total"
                << endl;
    UserContainer* u_container = c->GetUserContainer();
    POIContainer* p_container = c->GetPOIContainer();

    int processed_itineraries = 0;
    //srand((unsigned)time(0));

    vector<string> user_id_;
    map<int, Tour*> seq_map;
      
    while (processed_itineraries < 250) {
      int user_index = (rand() % u_container->GetNumOfUsers());
      User* u = u_container->GetUserByPos(user_index);
      // u->UpdateInterests(p_container, -1, 1);
      if (u->GetNumOfSeqs() <= 1) continue;

      int tour_index = (rand() % u->GetNumOfSeqs());


      Tour* s = u->GetTourByPos(tour_index);
      if (s->GetSeqLength() >= 1) {
        removeSeqAndUpdate(p_container, u, s);
        if (argvstr.compare("-rotas") == 0) {
          saveTour(p_container, u, s);
        }
        if (argvstr.compare("-solve") == 0) {
          cout << "############################################################"
                  "#######"
               << endl;
          cout << "[" << c->GetName() << "] Solving seq " << s->GetSeqId()
               << endl;
          auto start = std::chrono::steady_clock::now();
          TourContainer* tour_c =
              SPPRCSolver::Solve(p_container, u, s, eta, interest_type);
          if (tour_c->GetNumOfTours() != 0) {
            Tour* new_s = tour_c->GetOptimalTour();
            auto end = std::chrono::steady_clock::now();
            auto elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                      start);
            total += elapsed.count();
            cout << "Finished with " << (double)elapsed.count() / 1000000
                 << " seconds" << endl;

            if (new_s->GetProfit() != 0) {
              processed_itineraries++;
              generated_solutions.push_back(tour_c);
              compare(c->GetName(), s, new_s, p_container,
                      u->time_based_interest_, eta, result_file);
              result_file << (double)elapsed.count() / 1000000 << ";";
              result_file << (double)total / 1000000 << endl;
              user_id_.push_back(u->id_);
            }

            /*
            switch (interest_type) {
                    case 1:
                            compare(c->GetName(), s, new_s, p_container,
            u->frequency_based_interest_, eta, result_file); break;

                    case 2:
                            compare(c->GetName(), s, new_s, p_container,
            u->time_based_interest_, eta, result_file); break;

                    case 3:
                            compare(c->GetName(), s, new_s, p_container,
            u->time_based_interest_weighted_, eta, result_file); break;
            }

            result_file << (double) elapsed.count() / 1000000 << ";";
            result_file << (double) total / 1000000 << endl;
            */
          }
        }
        seq_map[s->GetSeqId()] = s;
        addSeqAndUpdate(p_container, u, s);
      }
    }
    result_file.close();
      
    ofstream tour_file;
    ofstream plot_num_of_tours_file;
    
    std::filesystem::create_directories("results/" + c->GetName());

    tour_file.open("results/" + c->GetName() + "/tour_file.in",
                   fstream::out);
      
    plot_num_of_tours_file.open("results/" + c->GetName() + "/plot_num_of_tours_file.in",
                   fstream::out);
      
    tour_file << generated_solutions.size() << endl;
    plot_num_of_tours_file << generated_solutions.size() << endl;
     
    int total_of_tours = 0;
      
    for (int i = 0; i < generated_solutions.size(); i++) {
      tour_file << user_id_[i] << endl;
      plot_num_of_tours_file << user_id_[i] << " ";
      total_of_tours = total_of_tours + generated_solutions[i]->GetNumOfTours();
      tour_file << generated_solutions[i]->GetNumOfTours() << endl;
      plot_num_of_tours_file << generated_solutions[i]->GetNumOfTours() << endl;
      for (int j = 0; j < generated_solutions[i]->GetNumOfTours(); j++) {
        tour_file << generated_solutions[i]->GetTour(j)->GetSeqId() << " ";
          
        Tour* original_tour = seq_map[generated_solutions[i]->GetTour(j)->GetSeqId()];
        for (int m = 0; m < original_tour->GetSeqLength(); m++){
            tour_file << original_tour->GetVisit(m)->GetPOIId();
          
            if (m != original_tour->GetSeqLength() - 1) {
                tour_file << "-";
            }
        }
        tour_file << endl; 
          
        tour_file << generated_solutions[i]->GetTour(j)->GetProfit() << endl;
        for (int k = 0; k < generated_solutions[i]->GetTour(j)->GetSeqLength();
             k++) {
          tour_file
              << generated_solutions[i]->GetTour(j)->GetVisit(k)->GetPOIId();
          // tour_file << " (" <<
          // generated_solutions[i]->GetTour(j)->GetVisit(k)->GetArrivalTime() <<
          // ", "; tour_file <<
          // generated_solutions[i]->GetTour(j)->GetVisit(k)->GetDepartureTime()
          // << ")";
          if (k != generated_solutions[i]->GetTour(j)->GetSeqLength() - 1)
            tour_file << " ";
        }
        tour_file << endl;
        vector<int> allocation_history =
            generated_solutions[i]->GetTour(j)->GetAllocation();
        vector<vector<int> > allocation_history_at_poi =
            generated_solutions[i]->GetTour(j)->GetAllocationAtPoi();
        for (int k = 0; k < 479; k++) {
          if (allocation_history[k] == 340)
            tour_file << 1 << " ";
          else
            tour_file << allocation_history[k] << " ";
        }
        tour_file << allocation_history[479] << endl;
        tour_file << p_container->GetNumOfPOIs() << endl;
        vector<int> li = p_container->GetIndexList();
        for (int l = 1; l <= p_container->GetNumOfPOIs(); l++) {
          for (int k = 0; k < 479; k++) {
            if (allocation_history_at_poi[li[l]][k] == 340)
              tour_file << 1 << " ";
            else
              tour_file << allocation_history_at_poi[li[l]][k] << " ";
          }
          tour_file << allocation_history_at_poi[li[l]][479] << endl;
        }
      }
    }
    plot_num_of_tours_file << total_of_tours << endl;
    plot_num_of_tours_file.close();
          
    ofstream plot_time_fase_1_file;
    plot_time_fase_1_file.open("results/" + c->GetName() + "/plot_time_fase_1_file.in",
                   fstream::out);
    plot_time_fase_1_file << (double)total / 1000000 << endl;
    plot_time_fase_1_file.close();
      
    tour_file.close();
    // MultiTourSolver::Solve(p_container, u_container, generated_solutions);
  }
}
