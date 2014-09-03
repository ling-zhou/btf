/* Source Code

Problem: 2761  User: rr 
Memory: 2904K  Time: 2610MS 
Language: G++  Result: Accepted 

Source Code */
// @brief test astree
// @author 周龄, brk@syscall.cn
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <algorithm>

#undef uint
typedef uint32_t uint;

#define BST_HAVE_SIZE 1

namespace tree {

template <typename Node, typename Key = Node>
class BST
{
public:
    // typedef void (Callback)(Node* node); NO!
    typedef void (*Callback)(Node* node);

public:
    Node* Null() const { return _NULL; }
    Node* Root() const { return _root; }
    Node* First() const;
    Node* Last() const;
    // predecessor
    Node* Prev(const Node* node) const;
    // successor
    Node* Next(const Node* node) const;
    Node* Min() const { return First(); }
    Node* Max() const { return Last(); }
    Node* Find(const Key& key) const;
    uint Size() const { return _size; }
    uint RotateNum() const { return _rotate_num; }
    void ClearRotateNum() { _rotate_num = 0; }
#ifdef BST_HAVE_SIZE
    uint Rank(const Node* node) const { return node->left->size + 1; }
    Node* Nth(uint n) const; // n in [1,x]
#endif
    void Preorder(Node* node, Callback cb);
    void Inorder(Node* node, Callback cb);
    void Postorder(Node* node, Callback cb);
    uint Depth(Node* node) const;
    uint Depth() const { return Depth(_root); } 
    void Print() { Inorder(_root, (Callback)&Node::Print); }

public:
    BST() { Init(); }
    ~BST() { Destroy(); }

protected:
#ifndef BST_HAVE_PARENT_PTR
    Node* FindPath(const Key& key);
#endif
    void Init();
    void Destroy();

protected:
#ifndef BST_HAVE_PARENT_PTR
    Node**  _path[128];
    int     _path_idx;     // index of _path
    int     _del_idx;  // index of delete
#endif
    Node    _null_node;
    Node*   _NULL;
    Node*   _root;
    uint    _size;
    uint    _rotate_num;
};

template <typename Node, typename Key>
void BST<Node, Key>::Init()
{
#ifndef BST_HAVE_PARENT_PTR
    _path_idx = -1;
    _del_idx = -1;
#endif
    _root = _NULL = &_null_node;
    _null_node.left = _null_node.right = _NULL;
    _size = _rotate_num = 0;
}

template <typename Node, typename Key>
void BST<Node, Key>::Destroy()
{
    // Postorder(_root, (Callback)&Node::Destroy); // add -Wno-pmf-conversions
    _root = _NULL;
}

template <typename Node, typename Key>
Node* BST<Node, Key>::First() const
{
    Node* node = _root;
    for (; node->left != _NULL; node = node->left);
    return node;
}

template <typename Node, typename Key>
Node* BST<Node, Key>::Last() const
{
    Node* node = _root;
    for (; node->right != _NULL; node = node->right);
    return node;
}

template <typename Node, typename Key>
Node* BST<Node, Key>::Prev(const Node* node) const
{
    Node* tmp = _NULL;
    if (node->left != _NULL)
    {
        for (tmp = tmp->left; tmp->right != _NULL; tmp = tmp->right);
        return tmp;
    }
    else if (node == _root)
        return _NULL;
    else
    {
#ifdef BST_HAVE_PARENT_PTR
        Node* parent = node->parent;
        while (parent != _NULL && node == parent->left)
        {
            node = parent;
            parent = parent->parent;
        }
#else
        // must no duplicated key
        Node* parent = _NULL;
        for (tmp = _root; tmp != _NULL; )
        {
            if (*tmp < *node)
            {
                parent = tmp;
                tmp = tmp->right;
            }
            else
                tmp = tmp->left;
        }
#endif
        return parent;
    }
}

template <typename Node, typename Key>
Node* BST<Node, Key>::Next(const Node* node) const
{
    Node* tmp = _NULL;
    if (node->right != _NULL)
    {
        for (tmp = node->right; tmp->left != _NULL; tmp = tmp->left);
        return tmp;
    }
    else if (node == _root)
        return _NULL;
    else
    {
#ifdef BST_HAVE_PARENT_PTR
        Node* parent = node->parent;
        while (parent != _NULL && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }
#else
        // must no duplicated key
        Node* parent = _NULL;
        for (tmp = _root; tmp != _NULL; )
        {
            if (*node < *tmp)
            {
                parent = tmp;
                tmp = tmp->left;
            }
            else
                tmp = tmp->right;
        }
#endif
        return parent;
    }
}

#ifdef BST_HAVE_SIZE
template <typename Node, typename Key>
Node* BST<Node, Key>::Nth(uint n) const
{
    Node* node = _root;
    while (node != _NULL)
    {
        uint size = node->left->size + 1;
        if (n == size)
            return node;
        else if (n < size)
            node = node->left;
        else
        {
            n -= size;
            node = node->right;
        }
    }
    return _NULL;
}
#endif

template <typename Node, typename Key>
Node* BST<Node, Key>::Find(const Key& key) const
{
    Node* node = _root;
    while (node != _NULL)
    {
        if (*node == key)
            break;
        else if (*node > key)
            node = node->left;
        else
            node = node->right;
    }
    return node;
}

#ifndef BST_HAVE_PARENT_PTR
template <typename Node, typename Key>
Node* BST<Node, Key>::FindPath(const Key& key)
{
    Node* node;
    Node** p = &_root;
    _path_idx = -1;
    while ((node = *p) != _NULL)
    {
        _path[++_path_idx] = p;
        if (*node == key)
            break;
        else if (*node > key)
            p = &node->left;
        else
            p = &node->right;
    }
    _del_idx = _path_idx;
    return node;
}
#endif

template <typename Node, typename Key>
void BST<Node, Key>::Preorder(Node* node, Callback cb)
{
    if (_NULL == node) return;
    cb(node);
    if (node->left != _NULL) Preorder(node->left, cb);
    if (node->right != _NULL) Preorder(node->right, cb);
}

template <typename Node, typename Key>
void BST<Node, Key>::Inorder(Node* node, Callback cb)
{
    if (_NULL == node) return;
    if (node->left != _NULL) Inorder(node->left, cb);
    cb(node);
    if (node->right != _NULL) Inorder(node->right, cb);
}

template <typename Node, typename Key>
void BST<Node, Key>::Postorder(Node* node, Callback cb)
{
    if (_NULL == node) return;
    if (node->left != _NULL) Postorder(node->left, cb);
    if (node->right != _NULL) Postorder(node->right, cb);
    cb(node);
}

template <typename Node, typename Key>
uint BST<Node, Key>::Depth(Node* node) const
{
    if (_NULL == node) return 0;
    int depth_left = (node->left != _NULL) ? Depth(node->left) + 1 : 1;
    int depth_right = (node->right != _NULL) ? Depth(node->right) + 1 : 1;
    return (depth_left < depth_right) ? depth_right : depth_left;
}

} // namespace tree

namespace tree {

template <typename Node, typename Key = Node>
class ASTree: public BST<Node, Key>
{
public:
    typedef Node* NodePtr;
    using BST<Node, Key>::_path;
    using BST<Node, Key>::_path_idx;
    using BST<Node, Key>::_del_idx;
    using BST<Node, Key>::_null_node;
    using BST<Node, Key>::_NULL;
    using BST<Node, Key>::_root;
    using BST<Node, Key>::_size;
    using BST<Node, Key>::_rotate_num;
    using BST<Node, Key>::Print;

public:
    void Insert(Node* new_node);
    void Delete(Node* del_node);
    Node* Delete(Key& key);

public:
    ASTree() { Init(); }
    ~ASTree() { }

protected:
    void Init();
    void RotateLeft(Node*& node);
    void RotateRight(Node*& node);
    void Fixup(int idx, bool is_insert);
};

template <typename Node, typename Key>
void ASTree<Node, Key>::Init()
{
    BST<Node, Key>::Init();
    _null_node.size = 0;
}

template <typename Node, typename Key>
void ASTree<Node, Key>::RotateLeft(Node*& node)
{
    NodePtr tmp = node; // 减少寻址运算
    NodePtr node_right = tmp->right;
    // link node and node->right->left
    tmp->right = node_right->left;
    // link node->right and node
    node_right->left = tmp;
    // update size
    node_right->size = tmp->size;
    tmp->size = tmp->left->size + tmp->right->size + 1;
    ++_rotate_num;
    // link node->right and node->parent
    node = node_right;
}

template <typename Node, typename Key>
void ASTree<Node, Key>::RotateRight(Node*& node)
{
    NodePtr tmp = node; // 减少寻址运算
    NodePtr node_left = tmp->left;
    // link node and node->left->right
    tmp->left = node_left->right;
    // link node->left and node
    node_left->right = tmp;
    // update size
    node_left->size = tmp->size;
    tmp->size = tmp->right->size + tmp->left->size + 1;
    ++_rotate_num;
    // link node->left and node->parent
    node = node_left;
}

template <typename Node, typename Key>
void ASTree<Node, Key>::Fixup(int idx, bool is_insert)
{
    for (; idx >= 0; --idx)
    {
        Node*& node = *_path[idx];
        Node* tmp = node; // 减少寻址运算
        is_insert ? ++tmp->size : --tmp->size;

        // 自创简单高效的平衡策略(by brk@syscall.cn)：
        // 1.右子树节点数不应该小于左子树节点数的一半
        // 2.左子树节点数不应该小于右子树节点数的一半
        if ((tmp->left->size >> 1) > tmp->right->size)
            RotateRight(node);
        else if ((tmp->right->size >> 1) > tmp->left->size)
            RotateLeft(node);
    }
}

template <typename Node, typename Key>
void ASTree<Node, Key>::Insert(Node* new_node)
{
    Node* node;
    Node** p = &_root;
    _path_idx = -1;
    while ((node = *p) != _NULL)
    {
        _path[++_path_idx] = p;
        if (*new_node < *node)
            p = &node->left;
        else
            p = &node->right;
    }

    // link new_node and parent
    *p = new_node;

    // init new_node
    new_node->left = _NULL;
    new_node->right = _NULL;
    new_node->size = 1;

    // rebalance
    Fixup(_path_idx, true);
    _size = _root->size;
}

template <typename Node, typename Key>
Node* ASTree<Node, Key>::Delete(Key& key)
{
    Node* del_node = FindPath(key);
    if (del_node != _NULL)
        Delete(del_node);
    return del_node;
}

template <typename Node, typename Key>
void ASTree<Node, Key>::Delete(Node* del_node)
{
    NodePtr node, parent;

    if (_NULL == del_node->left || _NULL == del_node->right)
        parent = del_node; // del_node最多只有一个子节点
    else // 找到del_node的后继节点
    {
        parent = del_node->right;
        _path[++_path_idx] = &del_node->right;
        for (; parent->left != _NULL; parent = parent->left)
            _path[++_path_idx] = &parent->left;
    }
    if (parent->left != _NULL)
        node = parent->left; // node是实际被删除节点的子节点
    else
        node = parent->right; // 同上

    // link parent->parent and node
    *_path[_path_idx] = node;

    if (del_node != parent) // del_node不是实际删除的那个节点
    {
        // 用parent节点替换del_node节点，不用数据拷贝的方式
        // 1: 拷贝parent的指向
        parent->left = del_node->left;
        parent->right = del_node->right;
        parent->size = del_node->size;

        // 2: 拷贝parent的被指向
        *_path[_del_idx] = parent;

        // 3: 修复path指向!!!
        _path[_del_idx + 1] = &parent->right;
    }

    Fixup(_path_idx - 1, false); // rebalance
    _size = _root->size;

    // clear del_node
    del_node->left = del_node->right = _NULL;
    del_node->size = 0;
}

} // namespace tree

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

struct Interval
{
    int from;
    int to;
    int k;
    int order;
};

struct Cmp
{
    inline bool operator()(const Interval& i1, const Interval& i2) const
    {
        return i1.from < i2.from;
    }
};

static const int N = 100001;
static const int M = 50001;
static const int D = 64;
static Node node[N];
static Interval interval[M];
static int result[M];

tree::ASTree<Node, int> astree;

static inline int select(int k)
{
    Node* p = astree.Nth(k);
    return (p != astree.Null()) ? p->key : INT_MAX;
}

int main()
{
#ifdef _DEBUG
    freopen("in.txt", "r", stdin);
#endif

    int n, m, i, j;

    scanf("%d %d", &n, &m);
    for (i = 1; i <= n; ++ i)
        scanf("%d", &node[i].key);
    for (i = 1; i <= m; ++ i)
    {
        Interval* p = interval + i;
        scanf("%d %d %d", &p->from, &p->to, &p->k); 
        p->order = i;
    }

    std::sort(interval + 1, interval + m + 1, Cmp());
    for (i = interval[1].from; i <= interval[1].to; ++i)
        astree.Insert(node + i);

    result[interval[1].order] = select(interval[1].k);    
    for (i = 2; i <= m; ++ i)
    {
        j = interval[i - 1].from;
        for (; j <= interval[i - 1].to && j < interval[i].from; ++ j)
            astree.Delete(node[j].key);

        if (interval[i].from > interval[i - 1].to)
            j = interval[i].from;
        else
            j = interval[i - 1].to + 1;

        for (; j <= interval[i].to; ++ j)
            astree.Insert(node + j);    
        result[interval[i].order] = select(interval[i].k);
    }

    for (i = 1; i <= m; ++ i)
        printf("%d\n", result[i]);    
    
    return 0;
}
