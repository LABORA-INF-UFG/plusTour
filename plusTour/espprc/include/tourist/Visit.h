#ifndef TOURIST_VISIT_H_
#define TOURIST_VISIT_H_

#include <iostream>
#include <vector>

class Visit {
 public:
  explicit Visit(int poi_id, double upd_time);
  explicit Visit(int poi_id, double arriv, double dep, double dur);
  virtual ~Visit() {}

  int GetPOIId() { return poi_id_; }
  double GetArrivalTime() { return arrival_time_; }
  double GetDepartureTime() { return departure_time_; }
  double GetDuration() { return duration_; }
  int GetPhotosTaken() { return photos_taken_; }
  void UpdateArrivalDeparture(double upd_time) {
    arrival_time_ = std::min(upd_time, arrival_time_);
    departure_time_ = std::max(upd_time, departure_time_);
    duration_ = departure_time_ - arrival_time_;
    photos_taken_++;
  }

 private:
  int poi_id_;
  double arrival_time_;
  double departure_time_;
  double duration_;
  int photos_taken_;
};

#endif