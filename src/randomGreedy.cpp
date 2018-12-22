#include <algorithm>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

static size_t minCost;
static int* minFacilityState;
static int* minCustomerState;
static int totalCustomerDemand;

namespace randomGreedyFunc {
void makeState(int* state, int len, function<bool(int*)> func);
bool EstimatedCost(int facilityCount, int customerCount, int facility[][2],
                   int customerDemand[], int** customerCost,
                   int* facilityState);
};  // namespace randomGreedyFunc

void randomGreedyFunc::makeState(int* state, int len,
                                 function<bool(int*)> func) {
  int count = 0;
  default_random_engine random(time(NULL));
  uniform_int_distribution<int> disInt(0, len - 1);
  while (true) {
    int i = disInt(random);
    state[i] = state[i] == 0 ? 1 : 0;
    size_t lastCost = minCost;
    bool success = func(state);
    if (success) {
      if (lastCost < minCost) {
        count = 0;
      } else {
        count++;
      }
    }
    if (count > 1440) {
      break;
    }
  }
}

bool randomGreedyFunc::EstimatedCost(int facilityCount, int customerCount,
                                     int facility[][2], int customerDemand[],
                                     int** customerCost, int* facilityState) {
  int totalCap = 0;
  size_t currentCost = 0;
  int currentCap[facilityCount];
  for (int i = 0; i < facilityCount; i++) {
    currentCap[i] = facilityState[i] * facility[i][0];
    currentCost += facilityState[i] * facility[i][1];
    totalCap += currentCap[i];
  }
  if (totalCap < totalCustomerDemand) {
    return false;
  }
  int customerState[customerCount];
  memset(customerState, 0, customerCount * sizeof(int));
  for (int i = 0; i < customerCount; i++) {
    int minIndex = -1;
    for (int j = 0; j < facilityCount; j++) {
      if (currentCap[j] - customerDemand[i] > 0) {
        if (minIndex == -1) {
          minIndex = j;
        } else if (customerCost[i][minIndex] > customerCost[i][j]) {
          minIndex = j;
        }
      }
    }
    if (minIndex == -1) {
      return false;
    } else {
      currentCost += customerCost[i][minIndex];
      currentCap[minIndex] -= customerDemand[i];
      customerState[i] = minIndex;
    }
  }
  if (currentCost < minCost) {
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }
  return true;
}

void randomGreedy(int facilityCount, int customerCount, int facility[][2],
                  int customerDemand[], int** customerCost) {
  minCost = -1;
  minFacilityState = new int[facilityCount];
  minCustomerState = new int[customerCount];

  int* facilityState = new int[facilityCount];
  memset(facilityState, 0, facilityCount * sizeof(int));

  // 总需求
  int totalCustomerDemand = 0;
  for (int i = 0; i < customerCount; i++) {
    totalCustomerDemand += customerDemand[i];
  }

  randomGreedyFunc::makeState(
      facilityState, facilityCount, [&](int* facilityState) -> bool {
        return randomGreedyFunc::EstimatedCost(facilityCount, customerCount,
                                               facility, customerDemand,
                                               customerCost, facilityState);
      });

  cout << minCost << endl;
  for (int i = 0; i < facilityCount; i++) {
    cout << minFacilityState[i] << " ";
  }
  cout << endl;
  for (int i = 0; i < customerCount; i++) {
    cout << minCustomerState[i] << " ";
  }
  cout << endl;
  delete minFacilityState;
  delete minCustomerState;
}
