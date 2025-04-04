#include "tourist/TourContainer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "tourist/Tour.h"

using namespace std;

TourContainer::TourContainer() { optimal_index_ = 0; }

void TourContainer::AddTour(Tour* tour) {
  tours_.push_back(tour);
  optimal_index_ = tours_[tours_.size() - 1]->GetProfit() >
                           tours_[optimal_index_]->GetProfit()
                       ? tours_.size() - 1
                       : optimal_index_;
}
