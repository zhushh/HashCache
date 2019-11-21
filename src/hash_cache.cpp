#include "hash_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#include <iostream>
using std::cout;
using std::endl;

HashCache::HashCache() {
    m = n = 0;
    data = NULL;
    unused = NULL;
    header = NULL;
    q_lock = NULL;
}

HashCache::HashCache(int n, int m) {
    init(n, m);
}

void HashCache::init(int n, int m) {
    if (m <= 0) {
        return;
    }

    if (n <= 0) {
        return;
    }

    this->m = m;
    this->n = n;

    int err;
    q_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * n);
    header = (int*)malloc(sizeof(int) * n);
    unused = (int*)malloc(sizeof(int) * n);
    data = (Node**)malloc(sizeof(Node*) * (n));
    for (int i = 0; i < n; i++) {
        header[i] = -1;
        unused[i] = 0;

        if ((err = pthread_mutex_init(q_lock + i, NULL)) != 0) {
            exit(err);
        }

        data[i] = (Node*)malloc(sizeof(Node) * (m));
        for (int j = 0; j < m; j++) {
            memset(&data[i][j], 0, sizeof(Node));
            data[i][j].prev = j-1;
            data[i][j].next = j+1;
        }
    }
}

HashCache::~HashCache() {
    if (NULL != data) {
        for (int i = 0; i < n; i++) {
            if (data[i] != NULL) {
                free(data[i]);
            }

            pthread_mutex_destroy(q_lock + i);
        }

        free(data);
    }

    if (NULL != header) {
        free(header);
    }

    if (NULL != unused) {
        free(unused);
    }

    if (NULL != q_lock) {
        free(q_lock);
    }
}

void HashCache::get(int k, int &value) {
    if (k < 0) { // 下标错误返回
        return;
    }

    int idx = hashN(k);

    // 加锁
    pthread_mutex_lock(q_lock + idx);

    int pos = header[idx];
    while (pos != -1 && data[idx][pos].k != k) {
        pos = data[idx][pos].next;
    }

    // 未找到返回
    if (pos == -1) {
        // 释放锁
        pthread_mutex_unlock(q_lock + idx);
        return;
    }

    // 找到pos位置
    value = data[idx][pos].value;
    moveToHead(idx, pos);

    // 释放锁
    pthread_mutex_unlock(q_lock + idx);

    // cout << "free idx="<< idx << " lock succ" << endl;
}

void HashCache::set(int k, int value) {
    if (k < 0) {
        return;
    }

    int idx = hashN(k);

    // 加锁
    pthread_mutex_lock(q_lock + idx);

    int pos = header[idx];
    int prev = header[idx];
    while (pos != -1 && data[idx][pos].k != k) {
        prev = pos;
        pos = data[idx][pos].next;
    }

    // 新key
    if (pos == -1) {
        // 已满，LRU淘汰最后一个，未满则分配一个空间使用
        if (unused[idx] >= m) {
            pos = prev;
        } else {
            pos = unused[idx];

            // 更新未使用表头
            unused[idx] = data[idx][pos].next;

            // 分配使用断开链接
            data[idx][pos].next = -1;

            if (unused[idx] < m) {
                data[idx][unused[idx]].prev = -1;
            }

            // cout << "alloc pos=" << pos << endl;
        }
    }


    // 旧key
    data[idx][pos].k = k;
    data[idx][pos].value = value;
    moveToHead(idx, pos);

    // 释放锁
    pthread_mutex_unlock(q_lock + idx);

    // cout << "free lock idx="<< idx << " succ" << endl;
}

int HashCache::hashN(int k) {
    return (k % n);
}

int HashCache::hashM(int k) {
    return (k % m);
}

void HashCache::moveToHead(int idx, int pos) {
    if (pos == header[idx]) {
        return;
    }

    int prev = data[idx][pos].prev;
    int next = data[idx][pos].next;

    // 调整当前位置前后指针
    if (prev != - 1) {
        data[idx][prev].next = next;
    }
    if (next != -1) {
        data[idx][next].prev = prev;
    }

    // 调整元素到头部
    data[idx][pos].next = header[idx];
    data[idx][pos].prev = -1;

    // 调整原来头部元素指针
    if (header[idx] != -1) {
        data[idx][header[idx]].prev = pos;
    }
    header[idx] = pos;
}

void HashCache::print(int idx) {
    cout << "header[idx]=" << header[idx] << ", unused[idx]=" << unused[idx] << endl;
    for (int j = 0; j < m; j++) {
        cout << "k=" << data[idx][j].k << ", value=" << data[idx][j].value << ", prev=" << data[idx][j].prev << ", next=" << data[idx][j].next << endl;
    }
}


void HashCache::selfPrint() {
    printf("q_lock=%x\n", q_lock);
    printf("header=%x\n", header);
    printf("unused=%x\n", unused);
    printf("data=%x\n", data);


    for (int i = 0; i < n; i ++ ) {
        printf("header[%d]=%d, unused[%d]=%d\n", i, header[i], i, unused[i]);
        for (int j = 0; j < m; j++) {
            printf("data[%d][%d] key=%d, value=%d, next=%d, prev=%d\n", i, j, data[i][j].k, data[i][j].value, data[i][j].next, data[i][j].prev);
        }
    }
}
