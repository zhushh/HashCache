#include "hash_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <iostream>
#include <string>
#include <fstream>
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::fstream;


// 命令行执行
// $ make
// $ ./Target [n]
// 上面的[n]是指定线程数

typedef struct _Op
{
    char op[10];
    int k;
    int value;
} Operation;


int t = 10;
int n = 10;
int m = 1;
long thread_count = 1;
long op_loop = 1;
Operation *testOp;
HashCache cache;


void load_test_data();
void* Thread_do_op(void*);
void output(long threadCount, int cur, int k, int value, bool bingo);


int main(int argc, char const *argv[])
{
    if (argc < 2) {
        cout << "Please input thread_count" << endl;
        cout << "./Target [count]" << endl;
        exit(0);
    }

    // 解析线程数量
    thread_count = strtol(argv[1], NULL, 10);
    if (thread_count <= 0) {
        return 0;
    }

    // 加载测试数据
    load_test_data();

    op_loop = t / thread_count;

    cout << "test t=" << t << ", n=" << n << ", m=" << m << ", thread_count=" << thread_count << ", op_loop=" << op_loop << endl;

    cache.init(n, m);

    // 创建线程测试
    pthread_t* thread_handlers;
    thread_handlers = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

    for (long thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handlers[thread], NULL, Thread_do_op, (void*)thread);
    }

    for (long thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handlers[thread], NULL);
    }

    free(thread_handlers);

    cout << "1" << endl;
    free(testOp);

    cout << "2" << endl;
    return 0;
}

void load_test_data() {
    string datafile = "data.txt";
    cout << datafile << endl;

    fstream fs(datafile);

    fs >> t >> n >> m;

    testOp = (Operation*)malloc(sizeof(Operation) * t);
    for (int i = 0; i < t; i++) {
        fs >> testOp[i].op >> testOp[i].k >> testOp[i].value;
    }

    fs.close();
}

void* Thread_do_op(void *count) {
    long threadCount = (long)count;
    long start, end;

    cout << "thread=" << threadCount << " start" << endl;

    start = threadCount * op_loop;
    end = (threadCount + 1) * op_loop;

    int value;
    while (start < t && start < end) {
        value = 0;

        cout << "start=" << start << ", op=" << testOp[start].op << ", k=" << testOp[start].k << ", value=" << testOp[start].value << endl;

        if (strncmp("get", testOp[start].op, sizeof("get")) == 0) 
        {
            cache.get(testOp[start].k, value);
        }
        else if (strncmp("set", testOp[start].op, sizeof("set")) == 0) 
        {
            cache.set(testOp[start].k, testOp[start].value);
        }
        start++;
    }
}

void output(long threadCount, int cur, int k, int value, bool bingo) {
    // 命中
    if (bingo) 
    {
        cout << "thread=" << threadCount << ", start=" << cur << ", k=" << k << ", value=" << value << ", bingo!" << endl;
    } 
    // else 
    // {
    //     cout << "thread=" << threadCount << ", start=" << cur << ", k=" << k << ", not found" << endl;
    // }
}
