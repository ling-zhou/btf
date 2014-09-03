/// bst 基类
/// @file
/// @date 2013-10-09 19:13:55
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn
/// @namespace tree

#ifndef __BST_BASE_H__
#define __BST_BASE_H__ \
    "$Id$"

#include <stdint.h>
#include <sys/types.h>
#include <limits.h>

#undef uint
typedef uint32_t uint;

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
    Postorder(_root, (Callback)&Node::Destroy); // add -Wno-pmf-conversions
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

}

#endif
