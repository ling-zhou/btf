/// rbtree
/// @file
/// @date 2013-10-09 21:48:08
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn
/// @namespace tree

#ifndef __RBTREE_H__
#define __RBTREE_H__ \
    "$Id$"

#define BST_HAVE_SIZE 1
#define BST_HAVE_PARENT_PTR 1
#include "bst_base.h"

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

    // init new_node
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

}

#undef BST_HAVE_SIZE
#undef BST_HAVE_PARENT_PTR
#endif
