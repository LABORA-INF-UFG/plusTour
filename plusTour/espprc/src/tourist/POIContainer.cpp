#include "tourist/POIContainer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "tourist/POI.h"

using namespace std;

POIContainer::POIContainer() {
  num_of_pois_ = 0;
  pois_[0] = new POI(0, "Invalid POI", 0, 0, "", 0, 0);
  index_list_.push_back(0);
  max_id_ = 0;
}

void POIContainer::AddPoi(POI* p) {
  //pois_.push_back(p);
  pois_[p->GetId()] = p;
  num_of_pois_++;

  index_list_.push_back(p->GetId());
  if (p->GetId() > max_id_){
      max_id_ = p->GetId();
  }
}

double POIContainer::GetTimeBetweenPOIs(int p1_id, int p2_id,
                                        string travelMethod) {
  if (travelMethod == "walking") {
    return distance_[p1_id][p2_id];
  }
  return -1.0;
}

void POIContainer::SetDistanceBetweenPOIs(int p1_id, int p2_id, double value) {
  if (distance_.size() == 0) {
    distance_.resize(max_id_ + 1);
    for (int i = 0; i <= max_id_; i++) {
      distance_[i].resize(max_id_ + 1);
    }
  }

  distance_[p1_id][p2_id] = value;
  distance_[p2_id][p1_id] = value;
}