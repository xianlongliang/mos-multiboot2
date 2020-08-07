#pragma once

#include <std/stdint.h>
#include <std/math.h>

template <class T>
class AVLTree
{
public:
    struct Node
    {
        uint64_t key;
        Node *left;
        Node *right;
        T *val;
        uint32_t height;
    };

    void insert(uint64_t key, T *val)
    {
        this->root = this->insert_node(this->root, key, val);
    }

    void erase(uint64_t key)
    {
        this->root = this->delete_node(this->root, key);
    }

    Node *find(uint64_t key)
    {
        return this->find_node(this->root, key);
    }

    bool empty() {
        return this->root == nullptr;
    }
private:
    Node *root;

    Node *alloc_node(uint64_t key, T *val)
    {
        Node *node = new Node();
        node->key = key;
        node->left = nullptr;
        node->right = nullptr;
        node->val = val;
        node->height = 1;
    }

    uint64_t height(Node *node)
    {
        if (node == nullptr)
            return 0;
        return node->height;
    }

    Node *left_most(Node *node)
    {
        auto cur = node;
        while (cur->left != nullptr)
            cur = cur->left;
        return cur;
    }

    // cal via height(left) - height(right)
    int balance_factor(Node *node)
    {
        if (node == nullptr)
            return 0;
        return height(node->left) - height(node->right);
    }

    // perform op on node x and y
    Node *left_rotate(Node *x)
    {
        Node *y = x->right;
        Node *b = y->left;
        y->left = x;
        x->right = b;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Node *right_rotate(Node *y)
    {
        Node *x = y->left;
        Node *b = x->right;
        x->right = y;
        y->left = b;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    Node *find_node(Node *root, uint64_t key)
    {
        if (root == nullptr)
            return nullptr;
        if (key < root->key)
            return find_node(root->left, key);
        else if (key > root->key)
            return find_node(root->right, key);
        else
            return root;
    }

    // node: the beging node to insert
    Node *insert_node(Node *node, uint64_t key, T *val)
    {
        if (node == nullptr)
            return this->alloc_node(key, val);
        if (key < node->key)
            node->left = this->insert_node(node->left, key, val);
        else if (key > node->key)
            node->right = this->insert_node(node->right, key, val);
        // if key == node->key
        else
            return node;

        // code reaches here only when key != node->key
        node->height = 1 + max(height(node->left), height(node->right));
        auto balanceFactor = balance_factor(node);
        // if left leaf is deeper than the right one
        if (balanceFactor > 1)
        {
            // if the key is on the left
            // which forms a left line
            if (key < node->left->key)
            {
                return right_rotate(node);
            }
            // if the key is on the right
            // which forms a left line
            else if (key > node->left->key)
            {
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }
        }
        // opposite to the above one
        if (balanceFactor < -1)
        {
            if (key > node->right->key)
            {
                return left_rotate(node);
            }
            else if (key < node->right->key)
            {
                node->right = right_rotate(node->right);
                return left_rotate(node);
            }
        }
        return node;
    }

    Node *delete_node(Node *root, uint64_t key)
    {
        // Find the node and delete it
        if (root == nullptr)
            return root;
        if (key < root->key)
            root->left = this->delete_node(root->left, key);
        else if (key > root->key)
            root->right = this->delete_node(root->right, key);
        else
        {
            if ((root->left == nullptr) ||
                (root->right == nullptr))
            {
                Node *temp = root->left ? root->left : root->right;
                if (temp == nullptr)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                    *root = *temp;
                    delete temp;
            }
            else
            {
                auto temp = left_most(root->right);
                root->key = temp->key;
                root->right = this->delete_node(root->right,
                                                temp->key);
            }
        }

        if (root == nullptr)
            return root;

        // Update the balance factor of each node and
        // balance the tree
        root->height = 1 + max(height(root->left), height(root->right));
        int balanceFactor = balance_factor(root);
        if (balanceFactor > 1)
        {
            if (balance_factor(root->left) >= 0)
            {
                return right_rotate(root);
            }
            else
            {
                root->left = left_rotate(root->left);
                return right_rotate(root);
            }
        }
        if (balanceFactor < -1)
        {
            if (balance_factor(root->right) <= 0)
            {
                return left_rotate(root);
            }
            else
            {
                root->right = right_rotate(root->right);
                return left_rotate(root);
            }
        }
        return root;
    }
};