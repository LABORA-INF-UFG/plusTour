#include "tourist/User.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "tourist/Tour.h"

using namespace std;

User::User(string id) { id_ = id; }

void User::UpdateInterests(POIContainer* poi_container, int seq_id,
                           double alpha) {
  ResetInterests(poi_container);
  for (int i = 0; i < tours_.size(); i++) {
    Tour* s = tours_[i];
    if (s->GetSeqId() == seq_id) continue;
    for (int j = 0; j < s->GetSeqLength(); j++) {
      Visit* v = s->GetVisit(j);
      POI* p = poi_container->GetPOIById(v->GetPOIId());

      if (p->GetAvgVisitDuration() != 0) {
        time_based_interest_[p->GetCategory()] +=
            (((double)v->GetDuration()) / p->GetAvgVisitDuration());
      }
      frequency_based_interest_[p->GetCategory()]++;
    }
  }

  // map<string, double>::iterator it;

  // for (it = time_based_interest_.begin(); it != time_based_interest_.end();
  // it++) 	time_based_interest_weighted_[it->first] = it->second;

  /*for (int i = 0, processed = 0; i < tours_.size(); i++) {
          Tour *s = tours_[i];
          if (s->GetSeqId() == seq_id) continue;
          processed++;
          for (int j = 0;  j < s->GetSeqLength(); j++) {
                  Visit* v = s->GetVisit(j);
                  POI* p = poi_container->GetPOIById(v->GetPOIId());
                  if (p->GetAvgVisitDuration() != 0) {
                          double recom_time =
  GetTimeToSpendTimeBasedWeighted(p); double actual_time = v->GetDuration();
                          double error = (recom_time - actual_time) /
  p->GetAvgVisitDuration(); time_based_interest_weighted_[p->GetCategory()] -=
  alpha * (processed / (tours_.size() - 1)) * error;
                  }
          }
  }*/
}

void User::ResetInterests(POIContainer* poi_container) {
  vector<int> li = poi_container->GetIndexList();
  for (int i = 1; i <= poi_container->GetNumOfPOIs(); i++) {
    POI* p = poi_container->GetPOIById(li[i]);
    frequency_based_interest_[p->GetCategory()] = 0;
    time_based_interest_[p->GetCategory()] = 0;
    time_based_interest_weighted_[p->GetCategory()] = 0;
  }
}

double User::GetTimeToSpendFrequencyBased(POI* p) {
  return GetFrequencyBasedInterest(p->GetCategory()) * p->GetAvgVisitDuration();
}

double User::GetTimeToSpendTimeBased(POI* p) {
  return GetTimeBasedInterest(p->GetCategory()) * p->GetAvgVisitDuration();
}

double User::GetTimeToSpendTimeBasedWeighted(POI* p) {
  return GetTimeBasedWeightedInterest(p->GetCategory()) *
         p->GetAvgVisitDuration();
}