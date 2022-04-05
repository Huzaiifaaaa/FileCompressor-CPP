// TODO(Jamani Alford)
#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>

using namespace std;

template <typename keyType, typename valueType>
class mymap {
    private:
    // NODE structure
    struct NODE {
        keyType key;    // used to build BST
        valueType value;    // stored data for the map
        NODE *left;     // links to left child
        NODE *right;    // links to right child
        int nL;     // number of nodes in left subtree
        int nR;     // number of nodes in right subtree
        bool isThreaded;

        NODE(keyType k, valueType v) {
            key = k;
            value = v;
            left = 0;
            right = 0;
            isThreaded = true;
            nL = 0;
            nR = 0;
        }
    };
    NODE *root;     // pointer to root node of the BST
    int size;   // # of key/value pairs in my map
    // iterator structure
    struct iterator {
    private:
        NODE *curr;     // points to current in-order node for begin/end
    public:
        explicit iterator(NODE *node) {
            curr = node;
        }

        keyType operator *() {
            return curr->key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }
        iterator operator++() {
            // if current->left = null and isThreaded = true then go to right
            // if isThreaded equal to true, find min in right subtree
            if (curr->left == 0 && curr->isThreaded) {
                curr = curr->right;
            }
            else if(!curr->isThreaded) {
                curr = curr->right;
                while(curr->left != 0) {
                    curr = curr->left;
                }
            }
            else {
                curr = curr->right;
            }

            return iterator(curr);
        }
    };

    // private helper functions
    NODE* insert(NODE *&node, NODE *&parent, keyType key, valueType value, NODE *&unbalancedNode, NODE *&unbalancedNodeParent) {
        // if tree does not exist
        if (node == 0) {
            node = new NODE(key, value);
            if (node != root) {
                // left insertion
                if (parent->key > key) {
                    node->right = parent;
                }
                // right insertion
                else if (parent->key < key) {
                    node->right = parent->right;
                }
            }
        }
        else if (node->key < key) {
            if (node->isThreaded) {
                NODE *x = 0;
                node->right = insert(x, node, key, value, unbalancedNode, unbalancedNodeParent);
            } else {
                node->right = insert(node->right, node, key, value, unbalancedNode, unbalancedNodeParent);
            }
            node->isThreaded = false;
            // check balance property here
            if (max(node->nR+1, node->nL) > 2 * min(node->nR+1, node->nL) + 1) {
                unbalancedNode = node;
                unbalancedNodeParent = parent;
            } else {
                node->nR = node->nR + 1;
            }
        } else {
            node->left = insert(node->left, node, key, value, unbalancedNode, unbalancedNodeParent);

            // check balance property here
            if (max(node->nR, node->nL+1) > 2 * min(node->nR, node->nL+1) + 1) {
                unbalancedNode = node;
                unbalancedNodeParent = parent;
            }else {
                node->nL = node->nL + 1;
            }
        }
        return node;
    }

    NODE* search(NODE *&node, keyType key) {
        if (node == 0) {
            return 0;
        }
        else if (node->key == key) {
            return node;
        }
        else if (node->key < key) {
            if (node->isThreaded){
                NODE *x = 0;
                return search(x, key);
            }else {
                return search(node->right, key);
            }
        }else {
            return search(node->left, key);
        }
    }

    // inorder for toString
    void inorder(NODE *node, stringstream &s) {
        if (node == 0) {
            return ;
        }
        inorder(node->left, s);
        s << "key: " << node->key << " value: " << node->value << "\n";
        if (node->isThreaded) {
            inorder(0, s);
        }else {
            inorder(node->right, s);
        }
    }

    // inorder for toVector
    void inorder(NODE *node, vector<pair<keyType, valueType>> &v) {
        if (node == 0) {
            return ;
        }
        inorder(node->left, v);
        pair<keyType, valueType> x;
        x.first = node->key;
        x.second = node->value;
        v.push_back(x);
        if (node->isThreaded) {
            inorder(0, v);
        }else {
            inorder(node->right, v);
        }
    }
    
    // inorder to store unbalanced insertion path
    void inorder(NODE *node, vector<NODE *> &v) {
        if (node == 0) {
            return ;
        }

        inorder(node->left, v);
        v.push_back(node);
        if (node->isThreaded) {
            inorder(0, v);
        }else {
            inorder(node->right, v);
        }
    }
    
    // reorder the unbalanced insertion path
    void reorder(vector<NODE *> vectorOfNodes,
                 int left, int middle,
                 int right,
                 NODE *&saveUnbalancedNode) {
        if (left <= right) {
            insert(saveUnbalancedNode, saveUnbalancedNode, vectorOfNodes[middle]);
            reorder(vectorOfNodes, left, (left+middle-1)/2, middle-1, saveUnbalancedNode);
            reorder(vectorOfNodes, middle+1, (right+middle+1)/2 , right, saveUnbalancedNode);
        }
    }
    
    // rewire nodes
    NODE *insert(NODE *&node, NODE *&parentNode, NODE *&keyNode) {
        if (node == keyNode) {return 0;}

        if (node == 0) {
            node = keyNode;
            // left insertion
            if (parentNode->key > keyNode->key) {
                node->right = parentNode;
            }
            // right insertion
            else if (parentNode->key < keyNode->key) {
                node->right = parentNode->right;
            }
        }
        else if (node->key < keyNode->key) {
            if (node->isThreaded) {
                NODE *x = 0;
                node->right = insert(x, node, keyNode);
            }else {
                node->right = insert(node->right, node, keyNode);
            }
            node->isThreaded = false;
            node->nR += 1;
        }else {
            node->left = insert(node->left, node, keyNode);
            node->nL += 1;
        }
        return node;
    }

    void preorder(NODE *node, stringstream &s) {
        if (node == 0) {
            return;
        }
        s << "key: "<< node->key << ", nL: " << node->nL << ", nR: " << node->nR <<'\n';
        preorder(node->left, s);
        if (node->isThreaded) {
            preorder(0, s);
        }else {
            preorder(node->right, s);
        }
    }

    void clear(NODE *&node) {
        if (node != 0) {
            clear(node->left);
            if (node->isThreaded) {
                NODE *x = 0;
                clear(x);
            } else {
                clear(node->right);
            }
            delete node;
            // TODO: change
            node = 0;
        }
    }

    void breadthFirstTraversal(NODE *OtherRoot, vector<NODE *> &nodes) {
        queue<NODE *> q;
        NODE *node = OtherRoot;

        if (node != 0) {
            q.push(node);
        }
        while (q.size() != 0) {
            node = q.front();
            q.pop();
            nodes.push_back(node);
            if (node->left != 0) {
                q.push(node->left);
            }
            if (node->right != 0 && !node->isThreaded) {
                q.push(node->right);
            }
        }
    }

public:
    mymap() {
        root = 0; // set root to null
        size = 0; // set size to 0
    }

    mymap(const mymap& other) {
        root = 0;
        size = 0;
        vector<NODE *> nodes;
        breadthFirstTraversal(other.root, nodes);

        for (auto node: nodes) {
            this->put(node->key, node->value);
        }
    }
    
    void put(keyType key, valueType value) {
        // check if key already exist in map
        NODE *node = search(root, key);

        // unbalanced node and its parent
        NODE *saveUnbalancedNode = 0;
        NODE *parentOfUnbalancedNode = 0;

        // if node found
        if(node) {
            node->value = value;
        }
        else {
            root = insert(root, root, key, value, saveUnbalancedNode, parentOfUnbalancedNode);
            ++size;
        }

        // if insertion leads to unbalance
        if (saveUnbalancedNode != 0) {
            vector<NODE *> insertionPathNodes;
            // get insertion path nodes
            inorder(saveUnbalancedNode, insertionPathNodes);

            // change the nodes values to default
            for (int i = 0; i < insertionPathNodes.size(); ++i) {
                insertionPathNodes[i]->nL = 0;
                insertionPathNodes[i]->nR = 0;
                insertionPathNodes[i]->left = 0;
                insertionPathNodes[i]->right = 0;
                insertionPathNodes[i]->isThreaded = true;
            }

            // reorder to get balance factor correct
            reorder(insertionPathNodes, 0, (insertionPathNodes.size()-1)/2, insertionPathNodes.size()-1, insertionPathNodes[(insertionPathNodes.size()-1)/2]);
            // if unbalancing started from root change root
            if (saveUnbalancedNode == root) {
                root = insertionPathNodes[(insertionPathNodes.size()-1)/2];
            } else if (parentOfUnbalancedNode->right == saveUnbalancedNode) {
                parentOfUnbalancedNode->right = insertionPathNodes[(insertionPathNodes.size()-1)/2];
            } else if (parentOfUnbalancedNode->left == saveUnbalancedNode) {
                parentOfUnbalancedNode->left = insertionPathNodes[(insertionPathNodes.size()-1)/2];
            }
        }
    }

    int Size() {
        return size;
    }

    bool contains(keyType key) {
        return search(root, key) != 0;
    }

    valueType get(keyType key) {
        NODE *x = search(root, key);
        if (x == 0) {
            return {};
        } else {
            return x->value;
        }
    }

    string toString() {
        stringstream s;
        inorder(root, s);
        return s.str();
    }

    valueType operator [] (keyType key) {
        NODE *node = search(root, key);
        if (node == 0) {
            put(key, {});
            return {};
        }
        else {
            return node->value;
        }
    }

    vector<pair<keyType, valueType>> toVector() {
        vector<pair<keyType, valueType>> v;
        inorder(root, v);
        return v;
    }

    string checkBalance() {
        stringstream s;
        preorder(root, s);
        return s.str();
    }
    
    iterator begin() {
        NODE *x = root;
        while(x->left != 0) {
            x = x->left;
        }
        return iterator(x);
    }

    iterator end() {
        return iterator(nullptr);
    }

    void clear() {
        clear(root);
        root = 0;
        size = 0;
    }

    mymap& operator=(const mymap& other) {
        // breadth first traversal
        vector<NODE *> nodes;
        breadthFirstTraversal(other.root, nodes);
        this->clear();

        for (auto node: nodes) {
            this->put(node->key, node->value);
        }
        return *this;
    }

    ~mymap() {
        if (this->root){
            this->clear();
        }
    }
};
