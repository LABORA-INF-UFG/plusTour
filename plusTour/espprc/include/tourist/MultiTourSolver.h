#ifndef TOURIST_MULTI_TOUR_SOLVER_H_
#define TOURIST_MULTI_TOUR_SOLVER_H_

#include "tourist/POIContainer.h"
#include "tourist/Tour.h"
#include "tourist/TourContainer.h"
#include "tourist/User.h"
#include "tourist/UserContainer.h"

class MultiTourSolver {
 public:
  explicit MultiTourSolver();
  virtual ~MultiTourSolver() {}

  static void Solve(POIContainer* poic, UserContainer* uc,
                    vector<TourContainer*> generated_solutions);
};

#endif
