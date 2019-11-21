package main

// 命令行执行命令
// $ go build
// $ ./test -n=100 -m=2 -t=1000 -max=10000 -o=data_100_2_1000.txt

import (
	"crypto/rand"
	"flag"
	"fmt"
	"math"
	"math/big"
	"os"
)

var (
	number     = flag.Int("n", 1, "Input create data number")
	slotSize   = flag.Int("m", 1, "Input slot number")
	dataSize   = flag.Int("t", 1, "Input create data size")
	maxKey     = flag.Int("k", math.MaxInt32, "Input max key")
	maxNumber  = flag.Int("max", math.MaxInt32, "Input max number")
	outputfile = flag.String("o", "./data.txt", "Input output filename")
)

var op = []string{
	"set",
	"get",
}

// GetRandomNumber 获取小于max的随机数
func GetRandomNumber(max int64) int64 {
	result, _ := rand.Int(rand.Reader, big.NewInt(max))
	return result.Int64()
}

func main() {
	flag.Parse()

	kMax := int64(*maxKey)
	numMax := int64(*maxNumber)
	fmt.Printf("n = %d, m = %d, testCase = %d, maxKey = %d, numMax = %d, outputfile = %s\n", *number, *slotSize, *dataSize, kMax, numMax, *outputfile)

	f, err := os.Create(*outputfile)
	if err != nil {
		panic(err)
		return
	}
	defer f.Close()

	// 写入测试数据t，n，m
	_, _ = f.WriteString(fmt.Sprintf("%d\t%d\t%d\n", *dataSize, *number, *slotSize))
	for k := 0; k < *dataSize; k++ {
		// 获取value
		num := GetRandomNumber(numMax) + 1
		key := GetRandomNumber(kMax)

		// 获取key
		opNum := GetRandomNumber(num)

		// 100:1 场景模拟
		op := "set"
		if opNum%100 >= 10 {
			op = "get"
		}

		_, _ = f.WriteString(fmt.Sprintf("%s\t%d\t%d\n", op, key, num))
	}
}
