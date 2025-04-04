#ifndef TOURIST_USER_CONTAINER_H_
#define TOURIST_USER_CONTAINER_H_

#include <iostream>
#include <vector>

#include "tourist/User.h"

class UserContainer {
 public:
  explicit UserContainer();
  virtual ~UserContainer() {}

  int GetNumOfUsers() { return users_.size(); }
  User* GetUserByPos(int pos) { return users_[pos]; }
  void AddUser(User* u) { users_.push_back(u); }

 public:
  std::vector<User*> users_;
};

#endif