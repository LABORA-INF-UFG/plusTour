#ifndef TOURIST_SERVICE_H_
#define TOURIST_SERVICE_H_

#include <iostream>
#include <vector>

using namespace std;

class Service {
 public:
  explicit Service(double bandwidth_requirement, double delay_requirement,
                   double processing_requirement);

  double GetBandwidthRequirement() { return bandwidth_requirement_; }
  double GetDelayRequirement() { return delay_requirement_; }
  double GetProcessingRequirement() { return processing_requirement_; }

 private:
  double bandwidth_requirement_;
  double delay_requirement_;
  double processing_requirement_;
};

#endif