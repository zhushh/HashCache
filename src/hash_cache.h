#ifndef __MY_HASH_CACHE__
#define __MY_HASH_CACHE__

#include <pthread.h>

typedef struct _Node
{
    int prev;
    int next;
    int k;
    int value;
} Node;

class HashCache
{
public:
    HashCache();
    HashCache(int n, int m);
    ~HashCache();

    void set(int k, int value);
    void get(int k, int &value);

    void init(int n, int m);
    void selfPrint();
private:
    int m, n;

    // pthread_mutex_t *q_lock;
    Node **data;
    int *header;
    int *unused;
    int *mutex;

    int hashN(int k);
    int hashM(int k);
    void moveToHead(int idx, int pos);

    void print(int idx);
};


#endif
