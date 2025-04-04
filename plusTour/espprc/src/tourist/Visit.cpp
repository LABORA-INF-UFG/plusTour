#include "tourist/Visit.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

Visit::Visit(int poi_id, double upd_time) {
  poi_id_ = poi_id;
  arrival_time_ = upd_time;
  departure_time_ = upd_time;
  duration_ = arrival_time_ - departure_time_;
  photos_taken_ = 1;
}

Visit::Visit(int poi_id, double arriv, double dep, double dur) {
  poi_id_ = poi_id;
  arrival_time_ = arriv;
  departure_time_ = dep;
  duration_ = dur;
}