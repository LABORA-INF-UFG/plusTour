#include "tourist/ECCRU.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

ECCRU::ECCRU(double processing_capacity, double cost) {
  processing_capacity_ = processing_capacity;
  cost_ = 1;
  processing_capacity_at_time_.assign(1441, processing_capacity_);
  num_allocation_at_time_.assign(1441, 0);
}

/*double ECCRU::AllocateResource(int start_time, int end_time, double
resource_ammount) { double ammount_alocated = 100; double
resource_ammount_alocated = resource_ammount;

        for (int i = start_time; i <= end_time; i++) {

                if ((processing_capacity_at_time_[i] - resource_ammount) < 0) {
                        ammount_alocated = min(ammount_alocated,
processing_capacity_at_time_[i] / resource_ammount); resource_ammount_alocated =
resource_ammount * ammount_alocated;
                }
        }

        for (int i = start_time; i <= end_time; i++) {
                processing_capacity_at_time_[i] -= resource_ammount_alocated;
        }

        return resource_ammount_alocated;
}*/

bool ECCRU::AllocateResource(int start_time, int end_time,
                             double resource_ammount, int* cost) {
  *cost = 0;

  for (int i = start_time; i <= end_time; i++) {
    if ((processing_capacity_at_time_[i] - resource_ammount) < 0) {
      return false;
    }
  }

  for (int i = start_time; i <= end_time; i++) {
    processing_capacity_at_time_[i] -= resource_ammount;
    num_allocation_at_time_[i]++;
    *cost += num_allocation_at_time_[i] * cost_;
  }

  return true;
}

bool ECCRU::AllocateResource(vector<int> resource_consumption, int* cost) {
  *cost = 0;
  for (int i = 0; i <= 1440; i++) {
    if ((processing_capacity_at_time_[i] - resource_consumption[i]) < 0) {
      return false;
    }
  }


  for (int i = 0; i <= 1440; i++) {
    if (resource_consumption[i] != 0) {
      processing_capacity_at_time_[i] -= resource_consumption[i];
      num_allocation_at_time_[i]++;
      *cost += (num_allocation_at_time_[i] * cost_);
    }
  }

  return true;
}

void ECCRU::DeallocateResource(int start_time, int end_time,
                               double resource_ammount) {
  for (int i = start_time; i <= end_time; i++) {
    processing_capacity_at_time_[i] += resource_ammount;
  }
}

void ECCRU::ClearAllocation() {
  processing_capacity_at_time_.assign(1441, processing_capacity_);
}
