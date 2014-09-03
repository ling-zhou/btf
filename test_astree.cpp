/// test astree. 
/// 测试发现，astree的性能似乎比rbtree还要稍高, 但是旋转数更多
/// @file
/// @date 2013-10-30 20:31:22 
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <set>
#include "astree.h"
#include "time_counter.h"
using namespace tree;

struct Node
{
    Node* left;
    Node* right;
    uint size;
    int key;

    void Destroy() {}
    void Print() { printf("%d\n", key); }
    //void Print() { printf("%d(%d) ", key, parent->key); fflush(stdout); }
#if 1
    bool operator<(int k) const { return key < k; }
    bool operator<(const Node& n) const  { return key < n.key; }
    bool operator>(int k) const { return key > k; }
    bool operator>(const Node& n) const { return key > n.key; }
    bool operator==(int k) const { return key == k; }
    bool operator==(const Node& n) const  { return key == n.key; }
#else
    operator int() const { return key; } // 产生一次拷贝，性能差一点
#endif
};

const uint N = 200000;
Node nodes[N];
int num[N];

typedef ASTree<Node, int> astree_t;
astree_t astree;

static void Run(bool test_next)
{
    TimeCounter tc;
    long cost;

    // insert
    tc.Reset();
    for (uint i = 0; i < N;++i)
    {
        nodes[i].key = num[i];
        astree.Insert(&nodes[i]);
        assert(astree.Size() == i + 1);
        assert(astree.Find(num[i])->key == num[i]);
    }
    cost = tc.Cost(); 
    int depth = astree.Depth();
    printf("insert: time: %ld ms, root->key: %d, size: %d, depth: %d rotate_num: %d\n",
            cost, astree.Root()->key, astree.Size(), depth, astree.RotateNum());
    printf("first: %d, last: %d\n", astree.First()->key, astree.Last()->key);

    // print inorder
    int fdout = fileno(stdout);
    int fdout_backup = dup(fdout);

    freopen("./recursive", "w", stdout);
    tc.Reset();
    astree.Print(); // inorder print
    cost = tc.Cost(); 
    printf("time %ld ms\n", cost);
    fflush(stdout);

    if (test_next)
    {
        freopen("./iterative", "w", stdout);
        tc.Reset();
        Node* node = astree.First();
        for (; node != astree.Null(); node = astree.Next(node))
            printf("%d\n", node->key);
        cost = tc.Cost(); 
        printf("time %ld ms\n", cost);
        fflush(stdout);
    }

    dup2(fdout_backup, fdout);

    // delete
    astree.ClearRotateNum();
    tc.Reset();
    for (uint i = 0; i < N;++i)
    {
        astree.Delete(num[i]);
        assert(astree.Size() == N - i - 1);
    }
    cost = tc.Cost(); 
    depth = astree.Depth();
    printf("delete: time: %ld ms, size: %d, depth: %d rotate_num: %d\n\n",
            cost, astree.Size(), depth, astree.RotateNum());
}

int main()
{
    // 测试自然数
    for (uint i = 0; i < N; ++i)
        num[i] = i;
    Run(false);

    // 测试相等数
    for (uint i = 0; i < N; ++i)
        num[i] = 1;
    Run(false);

    // 测试随机数
    srandom(getpid());
    std::set<int> s;
    uint i = 0;
    while (i < N)
    {
        int r = random();
        if (0 == s.count(r))
        {
            s.insert(r);
            num[i++] = r;
        }
    }
    Run(true);
    return 0;
}
