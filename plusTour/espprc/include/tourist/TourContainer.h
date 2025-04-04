#ifndef TOURIST_TOUR_CONTAINER_H_
#define TOURIST_TOUR_CONTAINER_H_

#include <iostream>
#include <vector>

#include "tourist/TourContainer.h"
#include "tourist/Tour.h"

using namespace std;

class TourContainer {
 public:
  explicit TourContainer();

  int GetNumOfTours() { return tours_.size(); }
  Tour* GetOptimalTour() { return tours_[optimal_index_]; }
  void AddTour(Tour* tour);
  Tour* GetTour(int tour_index) { return tours_[tour_index]; }

 private:
  std::vector<Tour*> tours_;
  int optimal_index_;
};

#endif
