# The Capacitated Facility Location Problem

固定容量的设施选址问题

## 题目

固定容量的设施选址问题是简单工厂选址问题的推广。

假设每个设施可以生产有限数量的产品，而每个设施有着不同的代价。每个客户有着不同的需求，而不同的设施对于满足不同客户的需求也有着不同的代价。这个问题就在于如何分配设施和客户，使得总代价最低。

CFLP 是一个 NP 难问题，最有效的方法还是拉格朗日松弛法或者矩阵列生成法，把这个问题转换成一个最小值的优化问题来解决，但是涉及到数学定理和公式比较多，这里就使用三种近似算法来求这个问题的近似最优解。

## 求解

对于这个问题，为了求解到更好的解，这里把**设施的开关**和**客户的分配**分为两个部分进行处理

第一是因为这两部分的关联其实不大，分开处理可以避免把计算资源浪费在不能满足需求的状态里

第二是因为分开以后求解过程会更为清晰，可以分别对于两种状态做不同的优化处理，在结果和效率之间取得更好的平衡

### 贪婪策略

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

使用贪婪法，对于一个固定的设施组合，可以在$O(nm)$的时间复杂度之内找到较优解，`n`和`m`分别为设施数和客户数

那么问题就来了， 如果找一个比较好的设施开放组合呢？

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

求解`P1-P24`：[计算结果](https://github.com/ZhenlyChen/CapacitatedFacilityLocation/blob/master/output_greedy.txt)

这种方法对于前 24 个解可以得到平均`9067.292`的花费，大多数解都在`7000-11000`之间。

一共花费了`84s`的时间，前面的`1-12`个问题都可以在毫秒级的时间求解，而后面 12 个设施的问题每个则需要`6-7s`的时间求解。

遍历所有的状态尽管可以找到比较好的解，但是状态空间随着设施数量的增加也会有着爆炸式的增长，在`i5-6200u`的性能的机器上，可以得到如下的结果：

| 设施数 | 状态空间            | 花费时间 |
| ------ | ------------------- | -------- |
| 10     | $2^{10}=1024$       | 约 0.03s |
| 20     | $2^{20}=1048576$    | 约 7s    |
| 30     | $2^{30}=1073741824$ | 约 2h    |

2 个小时来求解一个问题这个花费是不可接受，因此这里对于`P25`之后的解就不贴出来了。因此这不算一个成功的算法，因为花费的时间实在太久了。

### 随机搜索算法

为了解决状态爆炸的问题，这里对于寻找组合做了一个改进，改为随机寻找状态而不是遍历所有状态，那样就可以在有限的状态数下面寻找较优解。

```c++
void randomGreedyFunc::makeState(int* state, int len, function<bool(int*)> func) {
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
```

随机将设施组合的一个状态从`0`变成`1`或者从`1`变成`0`，一直寻找状态，直到连续`1440`个解的花费都大于当前最小花费就停止下来，这样，就可以尽量找到最优的状态。

虽然这种方法并没有遍历所有状态，但是也可以遍历尽量多的状态，直到很长一段时间内找不到更优解才停下来，这样，就**基于贪婪策略得到一个随机搜索算法**。

求解`P1-P71`：[计算结果](https://github.com/ZhenlyChen/CapacitatedFacilityLocation/blob/master/output_randomGreedy.txt)

和上面的遍历所有状态的方法相比，对于`P1-P24`，可以得到平均`9387.792`的花费，结果仅仅比遍历所有状态差`3.53%`，这个差别是可以接受的。而每一个问题仅仅需要大约`0.01s`的时间就可以得到，时间上比上面的方法优了 100 倍以上，在**效率和结果**上得到一个很好的平衡。

从所有结果上看：

| 平均花费   | 14810.9    |
| ---------- | ---------- |
| 最大值     | 41198      |
| 最小值     | 5834       |
| 总花费     | 1051574    |
| 总时间花费 | 4.0769843s |

对于设施数量为 10 和 20 的数据，花费一般在`7000-11000`之间，而设施数量为`30`的数据，花费一般在`13000-33000`之间。

### 爬山法

随机搜索毕竟是基于随机的，但是很多时候未必随机到一个比较好的结果，因此这里就要引入一些启发式的元素。爬山法假设当前解和周围的解是有变化规律的，这里尽管规律不太明显，但是还是有一定的规律，比如某个设施性价比可能比较高，因此开启这个设施的所有状态可以看作一个周围状态，这些状态可能有着比其他状态更好的解。

其基本思路是：

首先选择一个解作为种子解，**每次寻找与这个解相近的解，如果相近的解中有代价更小的解，则把这个解作为种子解。而如果周围的解都比该解的代价大，则表示已经到达了局部极小值点，搜索停止。**

这里以**设施全开**作为**种子解**，然后通过切换状态和交换状态这两种变换寻找邻域的状态

```c++
void hillClimbingFunc::makeState(int* state, int len, function<bool(int*)> func) {
  default_random_engine random(time(NULL));
  uniform_int_distribution<int> disInt(0, len - 1);
  uniform_int_distribution<int> disCount(1, 3);
  uniform_int_distribution<int> disSelect(1, 3);
  for (int i = 0; i < len; i++) {  // 全开状态
    state[i] = 1;
  }
  func(state);
  int changeTime = 18 * len;
  for (int i = 0; i < changeTime; i++) {
    int newState[len];
    memcpy(newState, state, len * sizeof(int));
    for (int j = 0; j < disCount(random); j++) {  // 随机变换1-3个
      if (disSelect(random) > 2) { // 切换变换
        int index = disInt(random);
        newState[index] = (newState[index] == 0 ? 1 : 0);
      } else { // 交换变换
        int a = disInt(random);
        int b = disInt(random);
        int temp = newState[a];
        newState[a] = newState[b];
        newState[b] = temp;
      }
    }

    size_t lastCost = minCost;
    if (func(newState) && lastCost > minCost) {
      memcpy(state, newState, len * sizeof(int));
      i = i > 200 ? i - 200 : 0;
    }
  }
}
```

不仅仅设施状态使用爬山法，对于客户的选择也可以使用爬山法进行进一步的求解，一般可以得到比贪婪策略更加优的解。

客户的选择首先**基于贪婪策略**寻找出一个**种子解**，

判断这个解和最优解的差在`15%`之内，如果超过这个值，那么这个解继续优化找到更有解的概率也就更小，也就是说则是一个没有**潜力**的解，可以直接跳过这个过程。

对于有潜力的解，通过添加一个客户到另一个设施或者交换两个客户的设施来寻找更优解，这里寻找的次数基于客户的数量，当客户的状态空间比较大的时候，可以采用更多的变换寻找更优解

```c++
bool hillClimbingFunc::EstimatedCost(int facilityCount, int customerCount,
                                     int facility[][2], int customerDemand[],
                                     int** customerCost, int* facilityState) {

  // ... 贪婪策略，和上文代码一样，这里省略了

  // 是否继续变换
  if (((float)currentCost / (float)minCost) < 1.15) {
    // cout << (float)currentCost / (float)minCost << endl;
    default_random_engine random(time(NULL));
    uniform_int_distribution<int> disInt(0, customerCount - 1);
    uniform_int_distribution<int> disCounts(1, 3);
    uniform_int_distribution<int> disSelect(1, 3);

    int neiCount = customerCount * 18;
    // 交叉变换
    for (int i = 0; i < neiCount; i++) {
      int neiState[customerCount];
      memcpy(neiState, customerState, customerCount * sizeof(int));
      for (int j = 0; j < disCounts(random); j++) { // 多重变换
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
      if (newCost != size_t(-1) && currentCost > newCost) {
        currentCost = newCost;
        memcpy(customerState, neiState, customerCount * sizeof(int));
        i = i > 200 ? i - 200 : 0;
      }
    }
  }

  if (currentCost < minCost) { // 找到更优解
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }

  return true;
}
```

这样，在设施状态和客户状态上都使用了启发式的爬山法寻找更优解，理论上对于随机搜索的贪婪策略，可以找到更优解，因为爬山法对于客户选择设施的过程上有了更大的灵活性。

求解`P1-P71`：[计算结果](https://github.com/ZhenlyChen/CapacitatedFacilityLocation/blob/master/output_hillClimbing.txt)

从所有结果上看：

| 平均花费   | 13410.94     |
| ---------- | ------------ |
| 最大值     | 38851        |
| 最小值     | 5214         |
| 总花费     | 952177       |
| 总时间花费 | 159.4030965s |

从结果上看，爬山法比随机搜索的结果要优`9.45%`，但是所花费的时间是随机搜索的近`40`倍。

如果在可以接受的时间范围内进一步寻找更优解，或者结果的优先级比高的情况下，爬山法无疑是一个很好的选择。

### 模拟退火算法

爬山法每次会往最优的方向去寻找解，在很多情况下会陷入局部最优解。对于这种问题，可以使用模拟退火算法来解决。在模拟退火算法中，有一定的概率可以结构恶化解，而这个概率也会随着温度的降低变低，避免了在一开始就陷入了局部最优解。

对于邻域的搜索和爬山法一样，只是加入了温度这个变量，一直寻找更优解直到温度下降到一个阈值。

每次得到结果之后，如果当前解更优，那么就接受，

否则计算$e^{\frac{-E}{T}}$是否大于一个(0,1)的随机值，如果是那就接受这个恶化解，这个概率会随着温度的降低而降低，而最后还是会趋向与最优解。

```c++

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
```

对于客户对设施的分配，同样采用模拟退火算法。

```c++

bool SAFunc::EstimatedCost(int facilityCount, int customerCount,
                           int facility[][2], int customerDemand[],
                           int** customerCost, int* facilityState) {

  // ... 贪婪策略，和上文代码一样，这里省略了

  // 是否继续变换
  if (((float)currentCost / (float)minCost) < 1.2) {
    // cout << (float)currentCost / (float)minCost << endl;
    default_random_engine random(time(NULL));
    uniform_int_distribution<int> disInt(0, customerCount - 1);
    uniform_int_distribution<int> disCounts(1, 3);
    uniform_int_distribution<int> disSelect(1, 3);
    uniform_int_distribution<int> disFire(1, 10000);

    // 模拟退火算法
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
            float p = exp((-deltaE) / (float)T);
            if ((disFire(random) / 10000.0f) < p) {  // 接受恶化解
              currentCost = newCost;
              memcpy(customerState, neiState, customerCount * sizeof(int));
            }
          }
        }
      }
      T = 0.97 * T;
    }
  }

  if (currentCost < minCost) {
    minCost = currentCost;
    memcpy(minFacilityState, facilityState, facilityCount * sizeof(int));
    memcpy(minCustomerState, customerState, customerCount * sizeof(int));
  }

  return true;
}
```

这里采用的初始温度 T 为`99`，每次降温的因子为`0.97`，最后温度小于`0.0001`停止计算。

求解`P1-P71`：[计算结果](https://github.com/ZhenlyChen/CapacitatedFacilityLocation/blob/master/output_SA.txt)

从所有结果上看：

| 平均花费   | 14614.82     |
| ---------- | ------------ |
| 最大值     | 40089        |
| 最小值     | 5869         |
| 总花费     | 1037652      |
| 总时间花费 | 192.1969315s |

总体上，模拟退火比随机搜索优`1.32%`，但是花费的时间无疑也是更多。

按道理，这种做法可以取得更好的结果，但是大部分结果还不如爬山法，而且花费的时间也更多了。

不过模拟退火算法在少数情况下可以找到一些爬山法并不能发现的更优解，但是这个更优也就少`100-200`左右的花费。

从效率和结果上看，模拟退火并没有比较好的结果，可能是和退火的参数有关，也有可能是这种情况下并不适用模拟退火，爬山法的效率和结果反而更好。

## 数据对比

各种算法的特点和对比其实在上面已经一一详细说过了，这里将其集中起来做一个数据对比。

| 算法     | 贪婪(遍历)       | 随机搜索 | 爬山     | 模拟退火 |
| -------- | ---------------- | -------- | -------- | -------- |
| 平均花费 | 9067.292(P1-P24) | 14810.9  | 13410.94 | 14614.82 |
| 最大花费 | 11790(P1-P24)    | 41198    | 38851    | 40089    |
| 最小花费 | 7092(P1-P24)     | 5834     | 5214     | 5869     |
| 时间花费 | 约 3 天          | 约 4s    | 约 159s  | 约 192s  |

`P1-P71`详细花费数据（一次测试）

| 数据集 | 随机搜索 | 爬山法 | 模拟退火 |
| ------ | -------- | ------ | -------- |
| 1      | 9142     | 8948   | 8897     |
| 2      | 8104     | 8037   | 7975     |
| 3      | 9824     | 9598   | 9618     |
| 4      | 11261    | 11084  | 10727    |
| 5      | 9348     | 9167   | 9027     |
| 6      | 8061     | 7968   | 7916     |
| 7      | 9990     | 9620   | 9675     |
| 8      | 11790    | 11420  | 11275    |
| 9      | 8598     | 8484   | 8497     |
| 10     | 7617     | 7617   | 7695     |
| 11     | 9064     | 8975   | 9083     |
| 12     | 10301    | 10175  | 10211    |
| 13     | 8842     | 8426   | 8850     |
| 14     | 7378     | 7150   | 7521     |
| 15     | 9550     | 8892   | 9432     |
| 16     | 11150    | 10492  | 11376    |
| 17     | 9145     | 8483   | 8612     |
| 18     | 7779     | 7152   | 7427     |
| 19     | 9763     | 8952   | 9443     |
| 20     | 11563    | 10707  | 11015    |
| 21     | 8960     | 8068   | 8634     |
| 22     | 7659     | 7092   | 7465     |
| 23     | 9422     | 9046   | 9247     |
| 24     | 10996    | 10299  | 10779    |
| 25     | 15146    | 12010  | 14837    |
| 26     | 13039    | 10957  | 12786    |
| 27     | 16039    | 12617  | 15775    |
| 28     | 19039    | 14455  | 17473    |
| 29     | 17020    | 13422  | 15134    |
| 30     | 15002    | 11734  | 13894    |
| 31     | 18802    | 13991  | 17022    |
| 32     | 22320    | 16516  | 20798    |
| 33     | 14563    | 12053  | 15006    |
| 34     | 12540    | 10807  | 12855    |
| 35     | 15340    | 12426  | 15762    |
| 36     | 18140    | 13976  | 18221    |
| 37     | 13454    | 11283  | 13999    |
| 38     | 12089    | 10562  | 12738    |
| 39     | 14289    | 11860  | 15464    |
| 40     | 16489    | 13193  | 17660    |
| 41     | 6810     | 6665   | 6829     |
| 42     | 5924     | 5790   | 6399     |
| 43     | 6016     | 5214   | 6527     |
| 44     | 7558     | 7524   | 7470     |
| 45     | 6640     | 6251   | 6591     |
| 46     | 6566     | 5794   | 6577     |
| 47     | 6634     | 6563   | 6744     |
| 48     | 5834     | 5660   | 6062     |
| 49     | 6154     | 5321   | 5869     |
| 50     | 9379     | 8927   | 9070     |
| 51     | 7996     | 7556   | 8088     |
| 52     | 9588     | 9588   | 9642     |
| 53     | 9785     | 8666   | 9496     |
| 54     | 9702     | 9295   | 9503     |
| 55     | 8764     | 8064   | 8566     |
| 56     | 22031    | 21448  | 22579    |
| 57     | 27770    | 27032  | 27800    |
| 58     | 41198    | 38851  | 40089    |
| 59     | 31594    | 29074  | 30867    |
| 60     | 21976    | 20605  | 22080    |
| 61     | 26476    | 24821  | 26287    |
| 62     | 35541    | 33646  | 35187    |
| 63     | 28276    | 25364  | 28537    |
| 64     | 21976    | 20530  | 21573    |
| 65     | 26342    | 24855  | 26337    |
| 66     | 34042    | 31857  | 34098    |
| 67     | 28444    | 25100  | 26743    |
| 68     | 21634    | 20601  | 21867    |
| 69     | 26529    | 24577  | 26654    |
| 70     | 36493    | 33013  | 35297    |
| 71     | 29284    | 26211  | 28403    |
