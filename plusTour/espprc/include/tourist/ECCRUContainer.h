#ifndef TOURIST_ECCRU_CONTAINER_H_
#define TOURIST_ECCRU_CONTAINER_H_

#include <iostream>
#include <vector>

#include "tourist/ECCRU.h"

using namespace std;

class ECCRUContainer {
 public:
  explicit ECCRUContainer();

  int GetNumOfECCRUs() { return eccrus_.size(); }
  bool AllocateResource(int start_time, int end_time, double resource_ammount,
                        int eccru_id, int *cost);
  bool AllocateResource(vector<int> resource_consumption, int eccru_id,
                        int *cost);
  void AddECCRU(ECCRU eccru) { eccrus_.push_back(eccru); }

 private:
  std::vector<ECCRU> eccrus_;
};

#endif