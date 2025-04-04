#include "tourist/City.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

City::City(string name, UserContainer* u_container, POIContainer* p_container) {
  name_ = name;
  u_container_ = u_container;
  p_container_ = p_container;
}
