/*#ifndef TOURIST_SPPRC_RESOURCE_SOLVER_H_
#define TOURIST_SPPRC_RESOURCE_SOLVER_H_

#include "tourist/ECCRUContainer.h"
#include "tourist/POIContainer.h"
#include "tourist/Tour.h"
#include "tourist/User.h"

class SPPRCResourceSolver {
 public:
  explicit SPPRCResourceSolver();
  virtual ~SPPRCResourceSolver() {}

  static Tour* Solve(POIContainer* poic, ECCRUContainer* eccru_c, User* u, Tour*
s, double eta, int interest_type);
};

#endif*/