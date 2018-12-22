# Capacitated Facility Location Problem



## Usage

### Algorithm

Choose a algorithm in `main.cpp`

```c++
// greed(facilityCount, customerCount, facility, customerDemand, customerCost);
hillClimbing(facilityCount, customerCount, facility, customerDemand, customerCost);
// randomGreedy(facilityCount, customerCount, facility, customerDemand, customerCost);
// SA(facilityCount, customerCount, facility, customerDemand, customerCost);
```

### Compile

```bash
$ make
```

### Test

```bash
$ ./test/test.ps1
```

### Result

[Greedy](./output_greedy.txt)

[Random](./output_randomGreedy.txt)

[hillClimbing](./output_hillClimbing.txt)

[SA](./output_SAtxt)

