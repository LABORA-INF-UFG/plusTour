#ifndef TOURIST_POI_H_
#define TOURIST_POI_H_

#include <iostream>
#include <vector>

#include "tourist/ECCRU.h"

class POI {
 public:
  explicit POI(int id, std::string name, double lat, double lng,
               std::string category, int popularity, int v_index);

  int GetId() { return id_; }
  std::string GetName() { return name_; }
  double GetLat() { return lat_; }
  double GetLng() { return lng_; }
  std::string GetCategory() { return category_; }
  int GetPopularity() { return popularity_; }
  int GetIndex() { return v_index_; }
  void SetPopularity(int popularity) { popularity_ = popularity; }
  double GetAvgVisitDuration() { return avg_visit_duration_; }
  double GetAvgVisitDurationByPopularity() {
    return (avg_visit_duration_ * num_visits_) / popularity_;
  }
  void UpdateAvgVisitDuration(double visit_duration) {
    avg_visit_duration_ =
        (((avg_visit_duration_ * num_visits_) + visit_duration) /
         ++num_visits_);
  }
  void RemoveFromAvgVisitDuration(double visit_duration) {
    avg_visit_duration_ =
        (((avg_visit_duration_ * num_visits_) - visit_duration) /
         --num_visits_);
  }
  int GetNumOfVisits() { return num_visits_; }

 private:
  int id_;
  std::string name_;
  double lat_;
  double lng_;
  std::string category_;
  int popularity_;
  int v_index_;
  double avg_visit_duration_;
  int num_visits_;
};

#endif