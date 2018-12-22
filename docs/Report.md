# The Capacitated Facility Location Problem

固定容量的设施选址问题

## 题目

固定容量的设施选址问题是简单工厂选址问题的推广。

假设每个设施可以生产有限数量的产品，而每个设施有着不同的代价。每个客户有着不同的需求，而不同的设施对于满足不同客户的需求也有着不同的代价。这个问题就在于如何分配设施和客户，使得总代价最低。

CFLP是一个NP难问题，最有效的方法还是拉格朗日松弛法或者矩阵列生成法，把这个问题转换成一个最小值的优化问题来解决，但是涉及到数学定理和公式比较多，这里就使用两种近似算法来求这个问题的近似最优解。



## 求解

### 贪婪

首先从最简单的策略开始，先假设设施的开设与否已经定下来了

那么问题就转换成：如何分配客户使得总花费最低？

最显而易见的方法就是把客户分配给其花费最小的设施，如果这个设施容量已经满了，那么就退而求次。

这样，我们就得到了一个简易的贪婪的方法：

1. 首先分配好开设和不开设的设施`facilityState`，用`0`表示关闭，`1`表示开启

2. 然后计算这些设施的总容量，如果总容量小于总需求，那么这个设施组合必然无效，可以直接寻找下一个设施组合，并且定义一个数组`currentCap`表示当前设施的现有容量
3. 对于每一个客户，优先分配**满足需求**并且**花费最小**的设施。
4. 最后得到的这组设施组合的**最小花费**

```cpp
void greedyFunc::EstimatedCost(int facilityCount, int customerCount, int facility[][2],
                   int customerDemand[], int** customerCost,
                   int* facilityState) {
  int totalCap = 0;
  size_t currentCost = 0;
  int currentCap[facilityCount];// 当前容量
  for (int i = 0; i < facilityCount; i++) {
    currentCap[i] = facilityState[i] * facility[i][0];
    currentCost += facilityState[i] * facility[i][1]; // 计算花费
    totalCap += currentCap[i]; // 计算总容量
  }
  if (totalCap < totalCustomerDemand) { // 总容量小于总需求
    return;
  }
  int customerState[customerCount];
  memset(customerState, 0, customerCount * sizeof(int));
  for (int i = 0; i < customerCount; i++) {
    int minIndex = -1;
    for (int j = 0; j < facilityCount; j++) {
      if (currentCap[j] - customerDemand[i] > 0) { // 当前设施是否满足需求
        if (minIndex == -1) {
          minIndex = j;
        } else if (customerCost[i][minIndex] > customerCost[i][j]) { // 是否花费最小
          minIndex = j;
        }
      }
    }
    if (minIndex == -1) { // 找不到满足的设施
      return;
    } else { // 更新当前花费和容量
      currentCost += customerCost[i][minIndex];
      currentCap[minIndex] -= customerDemand[i];
      customerState[i] = minIndex;
    }
  }
  if (currentCost < minCost) { // 是否比当前最优解好
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }
}
```

那么问题就来了， 如果找一个比较好的设施开放组合呢？

#### 遍历

最简单的方法就是遍历所有可能的组合

```c++
void greedyFunc::makeState(int* state, int current, int num, function<void(int*)> func) {
  if (current == num) {
    func(state);
  } else {
    state[current] = 0;
    makeState(state, current + 1, num, func);
    state[current] = 1;
    makeState(state, current + 1, num, func);
  }
}

void greedy(int facilityCount, int customerCount, int facility[][2],
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
  // 生产状态并估计花费
  greedyFunc::makeState(facilityState, 0, facilityCount, [&](int* facilityState) -> void {
    greedyFunc::EstimatedCost(facilityCount, customerCount, facility, customerDemand,
    customerCost, facilityState);
  });
  // 输出结果
}
```

这里使用递归的方法，遍历所有的状态，用贪婪法求解所有的组合的较优解，然后选取最优的一个作为最后的解。

遍历所有的状态尽管可以找到比较好的解，但是状态空间随着设施数量的增加也会有着爆炸式的增长，在`i5-6200u`的性能的机器上，可以得到如下的结果

| 设施数 | 状态空间            | 花费时间 |
| ------ | ------------------- | -------- |
| 10     | $2^{10}=1024$       | 约0.03s  |
| 20     | $2^{20}=1048576$    | 约7s     |
| 30     | $2^{30}=1073741824$ | 约2h     |

2个小时来求解一个问题这个花费是不可接受





