/// alpha-size-balanced tree
/// @file
/// @date 2013-10-09 21:48:08
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn
/// @namespace tree

#ifndef __ASTREE_H__
#define __ASTREE_H__ \
    "$Id$"

#define BST_HAVE_SIZE 1
#include "bst_base.h"

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

}

#undef BST_HAVE_SIZE
#endif
