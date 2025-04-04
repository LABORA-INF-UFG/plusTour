#include "tourist/ECCRUContainer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "tourist/ECCRU.h"

using namespace std;

ECCRUContainer::ECCRUContainer() { eccrus_.push_back(ECCRU(0, 0)); }

/*bool ECCRUContainer::AllocateResource(int start_time, int end_time, double
resource_ammount, vector<int> allocation_order, vector<pair<int, double> >
&allocation_eccru) {

        for (int i = 0; i < allocation_order.size() && resource_ammount > 0;
i++) { int eccru_id = allocation_order[i]; double allocation_ammount =
eccrus_[eccru_id].AllocateResource(start_time, end_time, resource_ammount);
                allocation_eccru.push_back(make_pair(eccru_id,
allocation_ammount)); resource_ammount -= allocation_ammount;
        }

        if (resource_ammount == 0)
                return true;
        else
                return false;
} */

bool ECCRUContainer::AllocateResource(int start_time, int end_time,
                                      double resource_ammount, int eccru_id,
                                      int* cost) {
  return eccrus_[eccru_id].AllocateResource(start_time, end_time,
                                            resource_ammount, cost);
}

bool ECCRUContainer::AllocateResource(vector<int> resource_consumption,
                                      int eccru_id, int* cost) {
  cout << "\n" << endl;
  return eccrus_[eccru_id].AllocateResource(resource_consumption, cost);
}
