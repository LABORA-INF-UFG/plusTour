#ifndef TOURIST_CITY_H_
#define TOURIST_CITY_H_

#include <iostream>
#include <vector>

#include "tourist/POIContainer.h"
#include "tourist/UserContainer.h"

using namespace std;

class City {
 public:
  explicit City(string name, UserContainer* u_container,
                POIContainer* p_container);

  string GetName() { return name_; }
  POIContainer* GetPOIContainer() { return p_container_; }
  UserContainer* GetUserContainer() { return u_container_; }

 private:
  string name_;
  POIContainer* p_container_;
  UserContainer* u_container_;
};

#endif