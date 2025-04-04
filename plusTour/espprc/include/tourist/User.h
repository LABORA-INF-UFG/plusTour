#ifndef TOURIST_USER_H_
#define TOURIST_USER_H_

#include <iostream>
#include <map>
#include <vector>

#include "tourist/POIContainer.h"
#include "tourist/Tour.h"

class User {
 public:
  explicit User(std::string id);
  virtual ~User() {}

  std::string GetId() { return id_; }
  void AddTour(Tour* s) { tours_.push_back(s); }
  int GetNumOfSeqs() { return tours_.size(); }
  Tour* GetTourByPos(int pos) { return tours_[pos]; }
  double GetFrequencyBasedInterest(std::string category) {
    return frequency_based_interest_[category];
  }
  double GetTimeBasedInterest(std::string category) {
    return time_based_interest_[category];
  }
  double GetTimeBasedWeightedInterest(std::string category) {
    return time_based_interest_weighted_[category];
  };
  double GetTimeToSpendFrequencyBased(POI* p);
  double GetTimeToSpendTimeBased(POI* p);
  double GetTimeToSpendTimeBasedWeighted(POI*);
  void UpdateInterests(POIContainer* poi_container, int seqID, double alpha);

 private:
  void ResetInterests(POIContainer* poi_container);

 public:
  std::string id_;
  std::vector<Tour*> tours_;
  std::map<std::string, double> frequency_based_interest_;
  std::map<std::string, double> time_based_interest_;
  std::map<std::string, double> time_based_interest_weighted_;
};

#endif