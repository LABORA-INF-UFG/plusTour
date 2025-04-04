#ifndef TOURIST_CITY_CONTAINER_H_
#define TOURIST_CITY_CONTAINER_H_

#include <iostream>
#include <vector>

#include "tourist/City.h"

class CityContainer {
 public:
  explicit CityContainer();
  virtual ~CityContainer() {}

  int GetNumOfCities() { return cities_.size(); }
  City* GetCityByPos(int pos) { return cities_[pos]; }
  void AddCity(City* c) { cities_.push_back(c); }

 public:
  std::vector<City*> cities_;
};

#endif