#include <algorithm>
#include <cmath>
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

namespace SAFunc {
void makeState(int* state, int len, function<bool(int*)> func);
bool EstimatedCost(int facilityCount, int customerCount, int facility[][2],
                   int customerDemand[], int** customerCost,
                   int* facilityState);
size_t calcCost(int facilityCount, int customerCount, int facility[][2],
                int* customerDemand, int** customerCost, int* facilityState,
                int* customerState);
};  // namespace SAFunc

void SAFunc::makeState(int* state, int len, function<bool(int*)> func) {
  default_random_engine random(time(NULL));
  uniform_int_distribution<int> disInt(0, len - 1);
  uniform_int_distribution<int> disCount(1, 3);
  uniform_int_distribution<int> disSelect(1, 3);
  uniform_int_distribution<int> disFire(1, 10000);
  for (int i = 0; i < len; i++) {  // 全开状态
    state[i] = 1;
  }
  func(state);
  float T = 99;
  while (T > 0.0001) {
    for (int i = 0; i < 8; i++) {
      int newState[len];
      memcpy(newState, state, len * sizeof(int));
      for (int j = 0; j < disCount(random); j++) {  // 随机变换1-3个
        if (disSelect(random) > 2) {
          int index = disInt(random);
          newState[index] = (newState[index] == 0 ? 1 : 0);
        } else {
          int a = disInt(random);
          int b = disInt(random);
          int temp = newState[a];
          newState[a] = newState[b];
          newState[b] = temp;
        }
      }

      size_t lastCost = minCost;
      if (func(newState)) {
        if (lastCost > minCost) {  // 替换
          memcpy(state, newState, len * sizeof(int));
        } else {
          float deltaE = lastCost - minCost;
          // float p = 1.0f / (float)(1 + exp((-deltaE) / (float)T));
          float p = exp((-deltaE) / (float)T);
          if ((disFire(random) / 10000.0f) < p) {  // 接受恶化解
            memcpy(state, newState, len * sizeof(int));
          }
        }
      }
    }
    T = 0.95 * T;
  }
}

bool SAFunc::EstimatedCost(int facilityCount, int customerCount,
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

  // 是否继续变换
  if (((float)currentCost / (float)minCost) < 1.2) {
    // cout << (float)currentCost / (float)minCost << endl;
    default_random_engine random(time(NULL));
    uniform_int_distribution<int> disInt(0, customerCount - 1);
    uniform_int_distribution<int> disCounts(1, 3);
    uniform_int_distribution<int> disSelect(1, 3);
    uniform_int_distribution<int> disFire(1, 10000);

    // 多重变换
    float T = 99;
    while (T > 0.0001) {
      for (int i = 0; i < 5; i++) {
        int neiState[customerCount];
        memcpy(neiState, customerState, customerCount * sizeof(int));
        for (int j = 0; j < disCounts(random); j++) {
          int a = disInt(random);
          int b = disInt(random);
          if (disSelect(random) > 2) {  // 添加变换
            neiState[a] = neiState[b];
          } else {  // 交叉变换
            int temp = neiState[a];
            neiState[a] = neiState[b];
            neiState[b] = temp;
          }
        }

        size_t newCost =
            calcCost(facilityCount, customerCount, facility, customerDemand,
                     customerCost, facilityState, neiState);
        if (newCost != size_t(-1)) {
          if (currentCost > newCost) {  // 接受最优解
            currentCost = newCost;
            memcpy(customerState, neiState, customerCount * sizeof(int));
          } else {
            float deltaE = newCost - currentCost;
            // float p = 1.0f / (float)(1 + exp((-deltaE) / (float)T));
            float p = exp((-deltaE) / (float)T);
            // cout << p << endl;
            if ((disFire(random) / 10000.0f) < p) {  // 接受恶化解
              currentCost = newCost;
              memcpy(customerState, neiState, customerCount * sizeof(int));
            }
          }
        }
      }
      T = 0.97 * T;
    }
    // cout << tmp << endl;
  }

  if (currentCost < minCost) {
    // cout << currentCost << endl;
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }

  return true;
}

size_t SAFunc::calcCost(int facilityCount, int customerCount, int facility[][2],
                        int* customerDemand, int** customerCost,
                        int* facilityState, int* customerState) {
  int totalCap = 0;
  size_t currentCost = 0;
  int currentCap[facilityCount];
  for (int i = 0; i < facilityCount; i++) {
    currentCap[i] = facilityState[i] * facility[i][0];
    currentCost += facilityState[i] * facility[i][1];
    totalCap += currentCap[i];
  }
  if (totalCap < totalCustomerDemand) {
    return -1;
  }
  for (int i = 0; i < customerCount; i++) {
    int facilityIndex = customerState[i];
    currentCost += customerCost[i][facilityIndex];
    currentCap[facilityIndex] -= customerDemand[i];
    if (currentCap[facilityIndex] < 0) {
      return -1;
    }
  }
  return currentCost;
}

void SA(int facilityCount, int customerCount, int facility[][2],
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

  SAFunc::makeState(facilityState, facilityCount,
                    [&](int* facilityState) -> bool {
                      return SAFunc::EstimatedCost(facilityCount, customerCount,
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
  delete[] minFacilityState;
  delete[] minCustomerState;
}
