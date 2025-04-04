#ifndef TOURIST_TOUR_H_
#define TOURIST_TOUR_H_

#include <iostream>
#include <map>
#include <vector>

#include "tourist/POIContainer.h"
#include "tourist/Visit.h"

class Tour {
 public:
  explicit Tour(int sed_id);
  virtual ~Tour() {}

  int GetSeqId() { return seq_id_; }
  int GetSeqLength() { return visits_.size(); }
  void AddVisit(Visit* vis) { visits_.push_back(vis); }
  Visit* GetVisit(int pos) { return visits_[pos]; }
  double GetDuration(POIContainer* poic);
  double GetProfit(POIContainer* poic, map<string, double> interest,
                   double eta);
  double GetProfit() { return profit_; }
  void SetProfit(double profit) { profit_ = profit; }
  vector<int> GetAllocation() { return allocation_in_time_; }
  vector<vector<int> > GetAllocationAtPoi() {
    return allocation_in_time_at_poi_;
  }
  void SetAllocation(vector<int> v) { allocation_in_time_ = v; }
  void SetAllocationAtPoi(vector<vector<int> > v) {
    allocation_in_time_at_poi_ = v;
  }
  void PrintTour();

 private:
  int seq_id_;
  std::vector<Visit*> visits_;
  double duration_;
  double profit_;
  vector<int> allocation_in_time_;
  vector<vector<int> > allocation_in_time_at_poi_;
};

#endif