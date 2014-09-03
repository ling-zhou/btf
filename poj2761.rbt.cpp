/* Source Code

Problem: 2761  User: rr 
Memory: 3348K  Time: 2657MS 
Language: G++  Result: Accepted 

Source Code */
// @brief test rbtree
// @author 周龄, brk@syscall.cn
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <limits.h>
#include <algorithm>

#undef uint
typedef uint32_t uint;

#define BST_HAVE_SIZE 1
#define BST_HAVE_PARENT_PTR 1

namespace tree {

template <typename Node, typename Key = Node>
class BST
{
public:
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
#if BST_HAVE_SIZE
    uint Rank(const Node* node) const { return node->left->size + 1; }
    Node* Nth(uint n) const; // n in [1,x]
#endif
    void Preorder(Node* node, Callback cb);
    void Inorder(Node* node, Callback cb);
    void Postorder(Node* node, Callback cb);
    uint Depth(Node* node) const;
    uint Depth() const { return Depth(_root); } 
    void Print() { /* Inorder(_root, (Callback)&Node::Print); */ }

public:
    BST() { Init(); }
    ~BST() { Destroy(); }

protected:
    Node* Find(const Key& key, Node*& parent) const;
    void Init();
    void Destroy();

protected:
    Node    _null_node;
    Node*   _NULL;
    Node*   _root;
    uint    _size;
    uint    _rotate_num;
};

template <typename Node, typename Key>
void BST<Node, Key>::Init()
{
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
#if BST_HAVE_PARENT_PTR
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
#if BST_HAVE_PARENT_PTR
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

#if BST_HAVE_SIZE
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
Node* BST<Node, Key>::Find(const Key& key, Node*& parent) const
{
    Node* node = _root;
    Node* tmp = _NULL;
    while (node != _NULL)
    {
        if (key == node)
            break;
        else
        {
            tmp = node;
            if (key < node)
                node = node->left;
            else
                node = node->right;
        }
    }
    parent = tmp;
    return node;
}

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
class RBTree: public BST<Node, Key>
{
public:
    typedef Node* NodePtr;
    using BST<Node, Key>::_null_node;
    using BST<Node, Key>::_NULL;
    using BST<Node, Key>::_root;
    using BST<Node, Key>::_size;
    using BST<Node, Key>::_rotate_num;

public:
    void Insert(Node* new_node);
    void Delete(Node* del_node);
    Node* Delete(Key& key);

public:
    RBTree() { Init(); }
    ~RBTree() { }

protected:
    void Init();
    void RotateLeft(Node* node);
    void RotateRight(Node* node);
    void InsertFixup(Node* node);
    void DeleteFixup(Node* node);

protected:
    enum Color { BLACK = 0, RED = 1 };
};

template <typename Node, typename Key>
void RBTree<Node, Key>::Init()
{
    BST<Node, Key>::Init();
    _null_node.parent = _NULL;
    _null_node.size = 0;
    _null_node.color = BLACK;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::RotateLeft(Node* node)
{
    NodePtr node_right = node->right, node_parent = node->parent;
    // link node and node->right->left
    node->right = node_right->left;
    node_right->left->parent = node;
    // link node->right and node->parent
    node_right->parent = node_parent;
    if (_NULL == node_parent)
        _root = node_right;
    else if (node == node_parent->left)
        node_parent->left = node_right;
    else
        node_parent->right = node_right;
    // link node->right and node
    node_right->left = node;
    node->parent = node_right;
    // update size
    node_right->size = node->size;
    node->size = node->left->size + node->right->size + 1;
    ++_rotate_num;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::RotateRight(Node* node)
{
    NodePtr node_left = node->left, node_parent = node->parent;
    // link node and node->left->right
    node->left = node_left->right;
    node_left->right->parent = node;
    // link node->left and node->parent
    node_left->parent = node_parent;
    if (_NULL == node_parent)
        _root = node_left;
    else if (node == node_parent->right)
        node_parent->right = node_left;
    else
        node_parent->left = node_left;
    // link node->left and node
    node_left->right = node;
    node->parent = node_left;
    // update size
    node_left->size = node->size;
    node->size = node->right->size + node->left->size + 1;
    ++_rotate_num;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::InsertFixup(Node* node)
{
    NodePtr parent, gparent, uncle;

    while (RED == (parent = node->parent)->color)
    {
        gparent = parent->parent;
        if (parent == gparent->left)
        {
            uncle = gparent->right;

            // insert case 1
            if (RED == uncle->color)
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                gparent->color = RED;
                node = gparent;
            }
            else
            {
                // insert case 2
                if (node == parent->right)
                {
                    RotateLeft(parent);
                    parent = node;
                }

                // insert case 3
                parent->color = BLACK;
                gparent->color = RED;
                RotateRight(gparent);
                break;
            } // else
        } // if (parent == gparent->left)
        else
        {
            uncle = gparent->left;

            // insert case 1
            if (RED == uncle->color)
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                gparent->color = RED;
                node = gparent;
            }
            else
            {
                // insert case 2
                if (node == parent->left)
                {
                    RotateRight(parent);
                    parent = node;
                }

                // insert case 3
                parent->color = BLACK;
                gparent->color = RED;
                RotateLeft(gparent);
                break;
            } // else
        } // else
    } // while (RED == (parent = node->parent)->color)

    _root->color = BLACK;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::DeleteFixup(Node* node)
{
    NodePtr parent, sibling;

    while (node != _root && BLACK == node->color)
    {
        parent = node->parent;
        if (node == parent->left)
        {
            sibling = parent->right;

            // delete case 1
            if (RED == sibling->color)
            {
                sibling->color = BLACK;
                parent->color = RED;
                RotateLeft(parent);
                sibling = parent->right;
            }
            
            // delete case 2
            if (BLACK == sibling->left->color && BLACK == sibling->right->color)
            {
                sibling->color = RED;
                node = parent;
            }
            else
            {
                // delete case 3
                if (BLACK == sibling->right->color)
                {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    RotateRight(sibling);
                    sibling = parent->right;
                }

                // delete case 4
                sibling->color = parent->color;
                parent->color = BLACK;
                sibling->right->color = BLACK;
                RotateLeft(parent);
                node = _root;
                break;
            } // else
        } // if (node == parent->left)
        else
        {
            sibling = parent->left;

            // delete case 1
            if (RED == sibling->color)
            {
                sibling->color = BLACK;
                parent->color = RED;
                RotateRight(parent);
                sibling = parent->left;
            }

            // delete case 2
            if (BLACK == sibling->right->color && BLACK == sibling->left->color)
            {
                sibling->color = RED;
                node = parent;
            }
            else
            {
                // delete case 3
                if (BLACK == sibling->left->color)
                {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    RotateLeft(sibling);
                    sibling = parent->left;
                }

                // delete case 4
                sibling->color = parent->color;
                parent->color = BLACK;
                sibling->left->color = BLACK;
                RotateRight(parent);
                node = _root;
                break;
            } // else
        } // else
    } // while (node != _root && BLACK == node->color)

    node->color = BLACK;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::Insert(Node* new_node)
{
    NodePtr parent = _NULL, node = _root;

    while (node != _NULL)
    {
        ++node->size; // upate size in insertion path
        parent = node;
        if (*new_node < *node)
            node = node->left;
        else
            node = node->right;
    }

    // link new_node and parent
    new_node->parent = parent;
    if (_NULL == parent)
        _root = new_node; // new_node is the first node
    else if (*new_node < *parent)
        parent->left = new_node;
    else
        parent->right = new_node;

    // init new_node pointers and color
    new_node->left = _NULL;
    new_node->right = _NULL;
    new_node->color = RED;
    new_node->size = 1;

    // rebalance
    InsertFixup(new_node);
    _size = _root->size;
}

template <typename Node, typename Key>
Node* RBTree<Node, Key>::Delete(Key& key)
{
    Node* del_node = Find(key);
    if (del_node != _NULL)
        Delete(del_node);
    return del_node;
}

template <typename Node, typename Key>
void RBTree<Node, Key>::Delete(Node* del_node)
{
    NodePtr parent, gparent, node;

    if (_NULL == del_node->left || _NULL == del_node->right)
        parent = del_node; // del_node最多只有一个子节点
    else
        parent = Next(del_node); // 找到del_node的后继节点
    if (parent->left != _NULL)
        node = parent->left; // node是实际被删除节点的子节点
    else
        node = parent->right; // 同上

    // update size in deletion path
    for (NodePtr tmp = parent; tmp != _NULL; tmp = tmp->parent)
        --tmp->size;

    // link parent->parent and node
    gparent = parent->parent;
    node->parent = gparent;
    if (_NULL == gparent) // parent必为根，此时del_node == parent
        _root = node;
    else if (parent == gparent->left)
        gparent->left = node;
    else
        gparent->right = node;

    int parent_color = parent->color;
    if (del_node != parent) // del_node不是实际删除的那个节点
    {
        // 用parent节点替换del_node节点，不用数据拷贝的方式
        // 1: 拷贝parent的指向
        NodePtr del_parent = del_node->parent;
        parent->parent = del_parent;
        parent->left = del_node->left;
        parent->right = del_node->right;
        parent->color = del_node->color;
        parent->size = del_node->size;

        // 2: 拷贝parent的被指向
        if (del_node == _root)
            _root = parent;
        else if (del_node == del_parent->left)
            del_parent->left = parent;
        else
            del_parent->right = parent;
        del_node->left->parent = parent;
        del_node->right->parent = parent;
    }

    if (BLACK == parent_color)
        DeleteFixup(node); // rebalance
    _size = _root->size;

    // clear del_node
    del_node->parent = del_node->left = del_node->right = _NULL;
    del_node->color = BLACK;
    del_node->size = 0;
}

} // namespace tree

struct Node
{
    Node* parent;
    Node* left;
    Node* right;
    uint color: 1;
    uint size: 31;
    int key;

    void Destroy() {}
    void Print() { printf("%d\n", key); }
#if 1
    bool operator<(int k) { return key < k; }
    bool operator<(const Node& n) { return key < n.key; }
    bool operator>(int k) { return key > k; }
    bool operator>(const Node& n) { return key > n.key; }
    bool operator==(int k) { return key == k; }
    bool operator==(const Node& n) { return key == n.key; }
#else
    operator int() { return key; }
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

tree::RBTree<Node, int> rbtree;

static inline int select(int k)
{
    Node* p = rbtree.Nth(k);
    return (p != rbtree.Null()) ? p->key : INT_MAX;
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
        rbtree.Insert(node + i);

    result[interval[1].order] = select(interval[1].k);    
    for (i = 2; i <= m; ++ i)
    {
        j = interval[i - 1].from;
        for (; j <= interval[i - 1].to && j < interval[i].from; ++ j)
            rbtree.Delete(node[j].key);

        if (interval[i].from > interval[i - 1].to)
            j = interval[i].from;
        else
            j = interval[i - 1].to + 1;

        for (; j <= interval[i].to; ++ j)
            rbtree.Insert(node + j);    
        result[interval[i].order] = select(interval[i].k);
    }

    for (i = 1; i <= m; ++ i)
        printf("%d\n", result[i]);    
    
    return 0;
}
