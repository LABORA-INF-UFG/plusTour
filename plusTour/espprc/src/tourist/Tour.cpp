#include "tourist/Tour.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "tourist/Visit.h"

using namespace std;

Tour::Tour(int seq_id) {
  seq_id_ = seq_id;
  duration_ = 0;
}

double Tour::GetDuration(POIContainer* poc) {
  if (duration_ != 0) return duration_;

  duration_ = 0;

  for (int i = 0; i < visits_.size(); i++) {
    Visit* v = visits_[i];
    int poiID = v->GetPOIId();
    if (i == visits_.size() - 1) {
      duration_ += v->GetDuration();
    } else if (i + 1 < visits_.size()) {
      duration_ += v->GetDuration();
      Visit* vNext = visits_[i + 1];
      int poiIDNext = vNext->GetPOIId();
      duration_ += poc->GetTimeBetweenPOIs(poiID, poiIDNext, "walking");
    }
  }

  return duration_;
}

void Tour::PrintTour() {
  cout << "{";
  for (int i = 0; i < visits_.size(); i++) {
    Visit* v = visits_[i];
    int poiID = v->GetPOIId();
    cout << poiID;

    if (i == visits_.size() - 1)
      cout << "}" << endl;
    else
      cout << ", ";
  }
}

double Tour::GetProfit(POIContainer* poc, map<string, double> interest,
                       double eta) {
  double profit = 0;

  for (int i = 0; i < visits_.size(); i++) {
    Visit* v = visits_[i];
    int poiID = v->GetPOIId();
    POI* p = poc->GetPOIById(poiID);
    profit +=
        (eta * interest[p->GetCategory()]) + ((1 - eta) * p->GetPopularity());
  }

  return profit;
}