#include "tourist/Service.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

Service::Service(double bandwidth_requirement, double delay_requirement,
                 double processing_requirement) {
  bandwidth_requirement_ = bandwidth_requirement;
  delay_requirement_ = delay_requirement;
  processing_requirement_ = processing_requirement;
}
