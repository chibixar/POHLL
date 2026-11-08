#pragma once

#include <iostream>
#include <iomanip>
#include <string>

template <typename T>
class BinaryTree
{
private:
    struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(const T& value)
            : data(value), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;
    int count = 0;

    Node* copyNode(const Node* node)
    {
        if (!node) return nullptr;
        Node* fresh = new Node(node->data);
        fresh->left = copyNode(node->left);
        fresh->right = copyNode(node->right);
        return fresh;
    }

    void clearNode(Node* node)
    {
        if (node)
        {
            clearNode(node->left);
            clearNode(node->right);
            delete node;
        }
    }

    Node* insertNode(Node* node, const T& value, bool& added)
    {
        if (!node)
        {
            added = true;
            return new Node(value);
        }
        if (value < node->data)
        {
            node->left = insertNode(node->left, value, added);
        }
        else if (node->data < value)
        {
            node->right = insertNode(node->right, value, added);
        }
        else
        {
            added = false;
        }
        return node;
    }

    Node* findMin(Node* node) const
    {
        while (node && node->left)
            node = node->left;
        return node;
    }

    Node* removeNode(Node* node, const T& value, bool& removed)
    {
        if (!node)
        {
            removed = false;
            return nullptr;
        }

        if (value < node->data)
        {
            node->left = removeNode(node->left, value, removed);
        }
        else if (node->data < value)
        {
            node->right = removeNode(node->right, value, removed);
        }
        else
        {
            removed = true;
            if (!node->left)
            {
                Node* rightChild = node->right;
                delete node;
                return rightChild;
            }
            if (!node->right)
            {
                Node* leftChild = node->left;
                delete node;
                return leftChild;
            }

            Node* successor = findMin(node->right);
            node->data = successor->data;
            node->right = removeNode(node->right, successor->data, removed);
        }
        return node;
    }

    const Node* findNode(const Node* node, const T& value) const
    {
        if (!node) return nullptr;
        if (value < node->data) return findNode(node->left, value);
        if (node->data < value) return findNode(node->right, value);
        return node;
    }

    void printSideways(const Node* node, int level, std::ostream& os) const
    {
        if (!node) return;
        printSideways(node->right, level + 1, os);
        os << std::string(level * 6, ' ') << "--> " << node->data << "\n";
        printSideways(node->left, level + 1, os);
    }

    void printInOrder(const Node* node, std::ostream& os) const
    {
        if (!node) return;
        printInOrder(node->left, os);
        os << node->data << " ";
        printInOrder(node->right, os);
    }

public:
    BinaryTree() : root(nullptr), count(0) {}
    
    BinaryTree(const BinaryTree& other)
        : root(copyNode(other.root)), count(other.count) {}

    ~BinaryTree()
    {
        clear();
    }

    BinaryTree& operator=(const BinaryTree& other)
    {
        if (this != &other)
        {
            clear();
            root = copyNode(other.root);
            count = other.count;
        }
        return *this;
    }

    bool add(const T& value)
    {
        bool added = false;
        root = insertNode(root, value, added);
        if (added) ++count;
        return added;
    }

    bool remove(const T& value)
    {
        bool removed = false;
        root = removeNode(root, value, removed);
        if (removed) --count;
        return removed;
    }

    const T* find(const T& value) const
    {
        const Node* node = findNode(root, value);
        return node ? &(node->data) : nullptr;
    }

    void print(std::ostream& os = std::cout) const
    {
        if (!root)
        {
            os << "(Empty Tree)\n";
            return;
        }
        printSideways(root, 0, os);
    }

    void printList(std::ostream& os = std::cout) const
    {
        printInOrder(root, os);
        os << "\n";
    }

    void clear()
    {
        clearNode(root);
        root = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
};
