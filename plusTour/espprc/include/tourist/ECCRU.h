#ifndef TOURIST_ECCRU_H_
#define TOURIST_ECCRU_H_

#include <iostream>
#include <vector>

using namespace std;

class ECCRU {
 public:
  explicit ECCRU(double processing_capacity, double cost);

  double GetProcessingCapacity() { return processing_capacity_; }
  double GetCost() { return cost_; }
  bool AllocateResource(int start_time, int end_time, double resource_ammount,
                        int* cost);
  bool AllocateResource(vector<int> resource_consumption, int* cost);
  void DeallocateResource(int start_time, int end_time,
                          double resource_ammount);
  void ClearAllocation();

 private:
  double processing_capacity_;
  double cost_;
  vector<double> processing_capacity_at_time_;
  vector<int> num_allocation_at_time_;
};

#endif