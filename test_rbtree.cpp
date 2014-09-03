/// test rbtree.
/// @file
/// @date 2013-10-12 18:27:23
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "rbtree.h"
#include "time_counter.h"
using namespace tree;

struct Node
{
    Node* parent;
    Node* left;
    Node* right;
    uint color: 1;
    uint size: 31;
    int key;

    void Destroy() {}
    // void Print() { printf("<%p,%d>", this, key); }
    // void Print() { printf("%d\n", key); fflush(stdout); }
    void Print() { printf("%d\n", key); }
    // void Print() { printf("%d(%d) ", key, parent->key); fflush(stdout); }
#if 1
    bool operator<(int k) const { return key < k; }
    bool operator<(const Node& n) const { return key < n.key; }
    bool operator>(int k) const { return key > k; }
    bool operator>(const Node& n) const { return key > n.key; }
    bool operator==(int k) const { return key == k; }
    bool operator==(const Node& n) const { return key == n.key; }
#else
    operator int() const { return key; } // 产生一次拷贝，性能差一点
#endif
};

const uint N = 200000;
Node nodes[N];
int num[N];

typedef RBTree<Node, int> rbtree_t;
rbtree_t rbtree;

static void Run()
{
    TimeCounter tc;
    long cost = 0;

    // insert
    tc.Reset();
    for (uint i = 0; i < N;++i)
    {
        nodes[i].key = num[i];
        rbtree.Insert(&nodes[i]);
        assert(rbtree.Size() == i + 1);
        assert(rbtree.Find(num[i])->key == num[i]);
    }
    cost = tc.Cost(); 
    int depth = rbtree.Depth();
    printf("insert: time: %ld ms, root->key: %d, size: %d, depth: %d rotate_num: %d\n",
            cost, rbtree.Root()->key, rbtree.Size(), depth, rbtree.RotateNum());
    printf("first: %d, last: %d\n", rbtree.First()->key, rbtree.Last()->key);

    // print inorder
    int fdout = fileno(stdout);
    int fdout_backup = dup(fdout);

    freopen("./recursive", "w", stdout);
    tc.Reset();
    rbtree.Print(); // inorder print
    cost = tc.Cost(); 
    printf("time %ld ms\n", cost);
    fflush(stdout);

    freopen("./iterative", "w", stdout);
    tc.Reset();
    Node* node = rbtree.First();
    for (; node != rbtree.Null(); node = rbtree.Next(node))
        printf("%d\n", node->key);
    cost = tc.Cost(); 
    printf("time %ld ms\n", cost);
    fflush(stdout);

    dup2(fdout_backup, fdout);

    // delete
    rbtree.ClearRotateNum();
    tc.Reset();
    for (uint i = 0; i < N;++i)
    {
        rbtree.Delete(num[i]);
        assert(rbtree.Size() == N - i - 1);
    }
    cost = tc.Cost(); 
    depth = rbtree.Depth();
    printf("delete: time: %ld ms, size: %d, depth: %d rotate_num: %d\n\n",
            cost, rbtree.Size(), depth, rbtree.RotateNum());
}

int main()
{
    // 测试自然数
    for (uint i = 0; i < N; ++i)
        num[i] = i;
    Run();

    // 测试相等数
    for (uint i = 0; i < N; ++i)
        num[i] = 1;
    Run();

    // 测试随机数
    srandom(getpid());
    for (uint i = 0; i < N; ++i)
        num[i] = random();
    Run();
    return 0;
}
