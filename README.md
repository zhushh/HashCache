# HashCache

测试kv代码

## 测试环境

linux机器：Linux VM_79_85_centos 3.10.107

```
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                8
On-line CPU(s) list:   0-7
Thread(s) per core:    1
Core(s) per socket:    8
Socket(s):             1
NUMA node(s):          1
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 79
Model name:            Intel(R) Xeon(R) CPU E5-26xx v4
Stepping:              1
CPU MHz:               2394.446
BogoMIPS:              4788.89
Hypervisor vendor:     KVM
Virtualization type:   full
L1d cache:             32K
L1i cache:             32K
L2 cache:              4096K
NUMA node0 CPU(s):     0-7
```

以下数据按照 `100:1` 的读写操作随机生成

使用mutex锁实现版本测试结果：

| 数据量 | 桶个数N    | slot大小M   | 同时运行线程数P   | 运行耗时 |
| ---- | --------  | ------ | ------------ | ---- |
| 1千万 | 1023     | 100    | 10   | 8380ms |
| 1千万 | 1023     | 100    | 50   | 8660ms |
| 1千万 | 1023     | 100    | 100   | 8700ms |
| 1千万 | 10000     | 10    | 10   | 1980ms |
| 1千万 | 10000     | 10    | 50   | 2050ms |
| 1千万 | 10000     | 10    | 100   | 2060ms |
| 1千万 | 32767     | 10    | 10   | 1950ms |
| 1千万 | 32767     | 10    | 50   | 2070ms |
| 1千万 | 32767     | 10    | 100   | 2040ms |


使用cas实现测试：

| 数据量 | 桶个数N    | slot大小M   | 同时运行线程数P   | 运行耗时 |
| ---- | --------  | ------ | ------------ | ---- |
| 1千万 | 1023     | 100    | 10   | 7330ms |
| 1千万 | 1023     | 100    | 50   | 8350ms |
| 1千万 | 1023     | 100    | 100   | 8500ms |
| 1千万 | 10000     | 10    | 10   | 1710ms |
| 1千万 | 10000     | 10    | 50   | 1870ms |
| 1千万 | 10000     | 10    | 100   | 1910ms |
| 1千万 | 32767     | 10    | 10   | 1930ms |
| 1千万 | 32767     | 10    | 50   | 2030ms |
| 1千万 | 32767     | 10    | 100   | 2070ms |


## 测试数据生成

测试数据：
```
$ ./create_data  -n=10000 -m=10 -t=10000000 -o=data.txt
```

压测执行：参数指定运行线程数
```
$ ./target 10
```
