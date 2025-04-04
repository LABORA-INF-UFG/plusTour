#ifndef TOURIST_SPPRC_SOLVER_H_
#define TOURIST_SPPRC_SOLVER_H_

#include "tourist/POIContainer.h"
#include "tourist/Tour.h"
#include "tourist/TourContainer.h"
#include "tourist/User.h"

class SPPRCSolver {
 public:
  explicit SPPRCSolver();
  virtual ~SPPRCSolver() {}

  static TourContainer* Solve(POIContainer* poic, User* u, Tour* s, double eta,
                              int interest_type);
};

#endif
