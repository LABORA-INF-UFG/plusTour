#include "tourist/POI.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

POI::POI(int id, std::string name, double lat, double lng, std::string category,
         int popularity, int v_index) {
  id_ = id;
  name_ = name;
  lat_ = lat;
  lng_ = lng;
  category_ = category;
  popularity_ = popularity;
  v_index_ = v_index;
  avg_visit_duration_ = 0;
  num_visits_ = 0;
}
