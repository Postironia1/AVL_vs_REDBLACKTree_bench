#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <numeric>
#include <fstream>


#pragma comment(linker, "/STACK:66777216")

using namespace std;


enum Color { RED, BLACK };

ofstream fout("maxdepth.txt");


struct Node {
    int key;
    Color color;
    Node* left;
    Node* right;
    Node* parent;

    Node(int key) {
        this->key = key;
        color = RED;
        left = right = parent = nullptr;
    }
};


class RedBlackTree {

private:

    Node* root;

    int getMaxHeight(Node* node) {
        if (node == nullptr) {
            return 0;
        }

        return 1 + std::max(getMaxHeight(node->left), getMaxHeight(node->right));
    }

    void getAllBranchesHeight(Node* node, int currentHeight) {
        if (node == nullptr) {
            return;
        }

        if (node->left == nullptr && node->right == nullptr) {
            /*std::cout << currentHeight << " ";*/
            fout << currentHeight << " ";
        }
        else {
            getAllBranchesHeight(node->left, currentHeight + 1);
            getAllBranchesHeight(node->right, currentHeight + 1);
        }
    }

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nullptr)
            y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }


    /**
        * Fixes the Red-Black Tree property violations that may occur after inserting a node.
        *
        * @param n The node that was just inserted and may cause Red-Black Tree property violations.
        */
    void fixInsert(Node* n) {
        // While n's parent is red (a violation of the Red-Black Tree property)
        while (n->parent != nullptr && n->parent->color == RED)
        {
            // If n's parent is the left child of its grandparent
            if (n->parent == n->parent->parent->left) {
                
                Node* u = n->parent->parent->right;
                // If u is red, recolor n's parent, uncle, and grandparent, and move n up to the grandparent
                if (u != nullptr && u->color == RED) {
                    n->parent->color = BLACK;
                    u->color = BLACK;
                    n->parent->parent->color = RED;
                    n = n->parent->parent;
                }
                // If u is black or a nullptr and n is a right child, rotate left about n's parent, recolor some nodes, and move n up to the grandparent
                else {
                    if (n == n->parent->right) {
                        n = n->parent;
                        leftRotate(n);
                    }
                    n->parent->color = BLACK;
                    n->parent->parent->color = RED;
                    rightRotate(n->parent->parent);
                }
            }
            else {
                Node* u = n->parent->parent->left;
                if (u != nullptr && u->color == RED) {
                    n->parent->color = BLACK;
                    u->color = BLACK;
                    n->parent->parent->color = RED;
                    n = n->parent->parent;
                }
                else {
                    if (n == n->parent->left) {
                        n = n->parent;
                        rightRotate(n);
                    }
                    n->parent->color = BLACK;
                    n->parent->parent->color = RED;
                    leftRotate(n->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

   /* void transplant(Node* u, Node* v) {
        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }*/

    Node* minimum(Node* x) {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }

    /**
    * fixDelete function fixes the violation of the red-black tree properties caused by deleting a node from the tree
    *
    * @param x - the node where the fix starts
    */
    void fixDelete(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                // x is a left child
                Node* w = x->parent->right;
                // case 1: x's sibling w is red
                if (w->color == RED) {
                    // case 1: x's sibling w is red
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    // case 2: x's sibling w is black, and both of w's children are black
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->right->color == BLACK) {
                        // case 3: x's sibling w is black, w's left child is red, and w's right child is black
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // case 4: x's sibling w is black, and w's right child is red
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else {
                // x is a right child
                Node* w = x->parent->left;
                if (w->color == RED) {
                    // case 1: x's sibling w is red
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    // case 2: x's sibling w is black, and both of w's children are black
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->left->color == BLACK) {
                        // case 3: x's sibling w is black, w's right child is red, and w's left child is black
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    // case 4: x's sibling w is black, and w's left child is red
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }
public:
    RedBlackTree() {
        root = nullptr;
    }

    int max_depth() {
        return getMaxHeight(root);
    }

    /**
    * Inserts a new node with the given key into the Red-Black Tree.
    * @param key The key to be inserted.
    */
    void insert(int key) {
        Node* current = new Node(key);
        Node* y = nullptr;
        Node* x = root;
        while (x != nullptr) {
            y = x;
            if (current->key < x->key)
                x = x->left;
            else
                x = x->right;
        }
        current->parent = y;
        if (y == nullptr)
            root = current;
        else if (current->key < y->key)
            y->left = current;
        else
            y->right = current;
        fixInsert(current);
    }

    Node* find(int key) {
        Node* x = root;
        while (x != nullptr) {
            if (key < x->key)
                x = x->left;
            else if (key > x->key)
                x = x->right;
            else
                return x;
        }
        return nullptr;
    }

    /**
    *Removes a node with the given key from the Red-Black tree.
    *
    *@param key The key of the node to be removed.
    */
    void remove(int key) {
        Node* z = find(key); // Find the node with the given key
        if (z == nullptr)
            return; // If the node does not exist, return
        Node* x, * y;
        if (z->left == nullptr || z->right == nullptr)
            y = z; // If the node has no or only one child, set y to the node itself
        else
            y = minimum(z->right); // If the node has two children, set y to the minimum node in its right subtree
        if (y->left != nullptr)
            x = y->left;
        else
            x = y->right;
        if (x == nullptr) // If x is null, return
            return;
        x->parent = y->parent;

        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        if (y != z)
            z->key = y->key; // Copy y's key to z, if necessary
        if (y->color == BLACK)
            fixDelete(x); // Fix the Red-Black tree properties, if necessary
        delete y; // Delete y
    }

    void allBranchesBlackHeigth(Node* node, int depth, std::vector<int>& result, int blackHeight) {
        if (node == nullptr)
            return;
        if (node->color == BLACK) {
            blackHeight++;
        }
        if (node->left == nullptr && node->right == nullptr) {
            result.push_back(blackHeight);
        }
        else {
            allBranchesBlackHeigth(node->left, depth + 1, result, blackHeight);
            allBranchesBlackHeigth(node->right, depth + 1, result, blackHeight);
        }
    }

    std::vector<int> getBlackHeigth() {
        std::vector<int> result;
        allBranchesBlackHeigth(root, 0, result, 0);
        return result;
    }

    void getAllBranchesHeight() {
        getAllBranchesHeight(root, 0);
    }
};


struct AVLNode {
    int key;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(int key) {
        this->key = key;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* N) {
        if (N == nullptr)
            return 0;
        return N->height;
    }

    int max(int a, int b) {
        return (a > b) ? a : b;
    }

    // New node creation
    AVLNode* newNode(int key) {
        AVLNode* node = new AVLNode(key);
        node->key = key;
        node->left = nullptr;
        node->right = nullptr;
        node->height = 1;
        return (node);
    }

    // Rotate right
    AVLNode* rightRotate(AVLNode* root) {
        AVLNode* new_root = root->left;
        AVLNode* T2 = new_root->right;
        new_root->right = root;
        root->left = T2;
        root->height = max(height(root->left),
            height(root->right)) + 1;
        new_root->height = max(height(new_root->left),
            height(new_root->right)) + 1;
        return new_root;
    }

    // Rotate left
    AVLNode* leftRotate(AVLNode* root) {
        AVLNode* new_root = root->right;
        AVLNode* T2 = new_root->left;
        new_root->left = root;
        root->right = T2;
        root->height = max(height(root->left),
            height(root->right)) + 1;
        new_root->height = max(height(new_root->left),
            height(new_root->right)) + 1;
        return new_root;
    }

    // Get the balance factor of each node
    int getBalanceFactor(AVLNode* N) {
        if (N == NULL)
            return 0;
        return height(N->left) -
            height(N->right);
    }

    // Insert a node
    AVLNode* insertNode(AVLNode* node, int key) {
        // Find the correct postion and insert the node
        if (node == NULL)
            return (newNode(key));
        if (key < node->key)
            node->left = insertNode(node->left, key);
        else if (key > node->key)
            node->right = insertNode(node->right, key);
        else
            return node;

        // Update the balance factor of each node and
        // balance the tree
        node->height = 1 + max(height(node->left),
            height(node->right));
        int balanceFactor = getBalanceFactor(node);
        if (balanceFactor > 1) {
            if (key < node->left->key) {
                return rightRotate(node);
            }
            else if (key > node->left->key) {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }
        if (balanceFactor < -1) {
            if (key > node->right->key) {
                return leftRotate(node);
            }
            else if (key < node->right->key) {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }
        return node;
    }

    // Node with minimum value
    AVLNode* nodeWithMimumValue(AVLNode* node) {
        AVLNode* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    AVLNode* find_helper_avl(int key) {
        AVLNode* current_node = this->root;
        while (current_node != nullptr) {
            if (current_node->key == key) {
                break;
            }
            current_node = current_node->key < key ? current_node->right : current_node->left;
        }
        return current_node;
    }

    // Delete a node
    AVLNode* deleteNode(AVLNode* root, int key) {
        // Find the node and delete it
        if (root == nullptr)
            return root;
        if (key < root->key)
            root->left = deleteNode(root->left, key);
        else if (key > root->key)
            root->right = deleteNode(root->right, key);
        else {
            if ((root->left == nullptr) ||
                (root->right == nullptr)) {
                AVLNode* temp = root->left ? root->left : root->right;
                if (temp == nullptr) {
                    temp = root;
                    root = nullptr;
                }
                else
                    *root = *temp;
                free(temp);
            }
            else {
                AVLNode* temp = nodeWithMimumValue(root->right);
                root->key = temp->key;
                root->right = deleteNode(root->right,
                    temp->key);
            }
        }

        if (root == NULL)
            return root;

        // Update the balance factor of each node and
        // balance the tree
        root->height = 1 + max(height(root->left),
            height(root->right));
        int balanceFactor = getBalanceFactor(root);
        if (balanceFactor > 1) {
            if (getBalanceFactor(root->left) >= 0) {
                return rightRotate(root);
            }
            else {
                root->left = leftRotate(root->left);
                return rightRotate(root);
            }
        }
        if (balanceFactor < -1) {
            if (getBalanceFactor(root->right) <= 0) {
                return leftRotate(root);
            }
            else {
                root->right = rightRotate(root->right);
                return leftRotate(root);
            }
        }
        return root;
    }

    int getMaxHeight(AVLNode* node) {
        if (node == nullptr) {
            return 0;
        }

        return 1 + std::max(getMaxHeight(node->left), getMaxHeight(node->right));
    }

    void getAllBranchesHeightAvl(AVLNode* node, int currentHeight) {
        if (node == nullptr) {
            return;
        }

        if (node->left == nullptr && node->right == nullptr) {
            /*std::cout << currentHeight << " ";*/
            fout << currentHeight << " ";
        }
        else {
            getAllBranchesHeightAvl(node->left, currentHeight + 1);
            getAllBranchesHeightAvl(node->right, currentHeight + 1);
        }
    }

   

public:
    AVLTree() {
        this->root = nullptr;
    }

    void insert_avl(unsigned int key) {
        this->root = insertNode(root, key);
    }

    void search_avl(unsigned int key) {
        this->root = find_helper_avl(key);
    }

    void remove_avl(unsigned int key) {
        this->root = deleteNode(root, key);
    }

    int max_depth() {
        return getMaxHeight(root);
    }

    void getAllBranchesHeightAvl() {
        getAllBranchesHeightAvl(root, 0);
    }
};


int main() {

    for (int i = 13; i < 14; i++) {
        cout << "Test seria: " << i + 1 << endl << endl;
        fout << "Test seria: " << i + 1 << endl << endl;
        std::vector<double> timing_avl;
        std::vector<double> timing_sort_avl;
        std::vector<double> timing_rbt;
        std::vector<double> timing_sort_rbt;
        for (int j = 0; j < 25; j++) {
            cout << "test number: " << j + 1 << endl;
            fout << "test number: " << j + 1 << endl;
            RedBlackTree rbt;
            AVLTree avl;
            auto vec = std::vector<unsigned int>(pow(2, 5 + i));
            if (j > 25) {
                mt19937 engine(time(nullptr));
                uniform_real_distribution<double> gen(0, pow(2, 5 + i));
                for (auto& el : vec)
                    el = gen(engine);
            }
            else {
                std::iota(vec.begin(), vec.end(), 1);
            }

            //inserting into rbt
            chrono::high_resolution_clock::time_point start1 = chrono::high_resolution_clock::now();
            for (auto& el : vec) {
                rbt.insert(el);
            }
            chrono::high_resolution_clock::time_point end1 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff1 = end1 - start1;
            cout << "Time inserting into RBT tree: " << sec_diff1.count() << " sec." << endl;
            //fout << "Time inserting into RBT tree: " << sec_diff1.count() << " sec." << endl;


            //inserting into avl
            chrono::high_resolution_clock::time_point start2 = chrono::high_resolution_clock::now();
            for (auto& el : vec) {
                avl.insert_avl(el);
            }
            chrono::high_resolution_clock::time_point end2 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff2 = end2 - start2;
            cout << "Time inserting into AVL tree: " << sec_diff2.count() << " sec." << endl;
            //fout << "Time inserting into AVL tree: " << sec_diff2.count() << " sec." << endl;

            //MaxDepthRBT
      
            fout << "Max depth RBT: " << rbt.max_depth() << endl;

            //MaxDepthAVL

            fout << "Max depth AVL: " << avl.max_depth() << endl;

            //AllDepthRBT
          
            //fout << "All depth RBT: "; rbt.getAllBranchesHeight(); fout << endl;

            //AllDepthAVL
           
            //fout << "All depth AVL: "; avl.getAllBranchesHeightAvl(); fout << endl;



            //searching in rbt
            chrono::high_resolution_clock::time_point start3 = chrono::high_resolution_clock::now();
            for (int k = 0; k < 1000; k++) {
                mt19937 engine(time(nullptr));
                uniform_real_distribution<double> gen(0, pow(2, 5 + i));
                rbt.find(gen(engine));
            }
            chrono::high_resolution_clock::time_point end3 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff3 = end3 - start3;
            cout << "Time searching 1000 times in RBT tree: " << sec_diff3.count() << " sec." << endl;
            //fout << "Time searching 1000 times in RBT tree: " << sec_diff3.count() << " sec." << endl;
            auto bst_searching_time = sec_diff3.count() / 1000;
            //fout << "Time searching 1 time in RBT tree: " << bst_searching_time << " sec." << endl;
            cout << "Time searching 1 time in RBT tree: " << bst_searching_time << " sec." << endl;


            //searching in avl
            chrono::high_resolution_clock::time_point start4 = chrono::high_resolution_clock::now();
            for (int k = 0; k < 1000; k++) {
                mt19937 engine(time(nullptr));
                uniform_real_distribution<double> gen(0, pow(2, 5 + i));
                avl.search_avl(gen(engine));
            }
            chrono::high_resolution_clock::time_point end4 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff4 = end4 - start4;
            cout << "Time searching 1000 times in AVL tree: " << sec_diff4.count() << " sec." << endl;
            //fout << "Time searching 1000 times in AVL tree: " << sec_diff4.count() << " sec." << endl;
            auto avl_searching_time = sec_diff4.count() / 1000;
            //fout << "Time searching 1 time in AVL tree: " << avl_searching_time << " sec." << endl;
            cout << "Time searching 1 time in AVL tree: " << avl_searching_time << " sec." << endl;


            //deleting in RBT
            chrono::high_resolution_clock::time_point start6 = chrono::high_resolution_clock::now();
            for (int k = 0; k < 1000; k++) {
                mt19937 engine(time(nullptr));
                uniform_real_distribution<double> gen(0, pow(2, 5 + i));
                rbt.remove(gen(engine));
            }
            chrono::high_resolution_clock::time_point end6 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff6 = end6 - start6;
            cout << "Time deleting 1000 times from RBT tree: " << sec_diff6.count() << " sec." << endl;
            //fout << "Time deleting 1000 times from RBT tree: " << sec_diff6.count() << " sec." << endl;
            auto bst_deleting_time = sec_diff6.count() / 1000;
            //fout << "Time deleting 1 time from RBT tree: " << bst_deleting_time << " sec." << endl;
            cout << "Time deleting 1 time from RBT tree: " << bst_deleting_time << " sec." << endl;
            if (j > 3)
                timing_rbt.push_back(sec_diff6.count());
            else 
                timing_sort_rbt.push_back(sec_diff6.count());

            //deleting in AVL
            chrono::high_resolution_clock::time_point start7 = chrono::high_resolution_clock::now();
            for (int k = 0; k < 1000; k++) {
                mt19937 engine(time(nullptr));
                uniform_real_distribution<double> gen(0, pow(2, 5 + i));
                avl.remove_avl(gen(engine));
            }
            chrono::high_resolution_clock::time_point end7 = chrono::high_resolution_clock::now();
            chrono::duration<double> sec_diff7 = end7 - start7;
            cout << "Time deleting 1000 times from AVL tree: " << sec_diff7.count() << " sec." << endl;
            //fout << "Time deleting 1000 times from AVL tree: " << sec_diff7.count() << " sec." << endl;
            auto avl_deleting_time = sec_diff7.count() / 1000;
            cout << "Time deleting 1 time from AVL tree: " << avl_deleting_time << " sec." << endl;
            //fout << "Time deleting 1 time from AVL tree: " << avl_deleting_time << " sec." << endl;
            if (j > 3)
                timing_avl.push_back(sec_diff7.count());
            else
                timing_sort_avl.push_back(sec_diff7.count());


         

        }
        /*fout << "times of deleting 1000 times from AVL: " << std::reduce(timing_avl.begin(), timing_avl.end(), 0.0) / timing_avl.size();
        fout << "times of deleting 1000 times from AVL sorted: " << std::reduce(timing_sort_avl.begin(), timing_sort_avl.end(), 0.0) / timing_sort_avl.size();
        cout << endl;
        fout << endl;
        fout << "times of deleting 1000 times from RBT: " << std::reduce(timing_rbt.begin(), timing_rbt.end(), 0.0) / timing_rbt.size();
        fout << "times of deleting 1000 times from RBT sorted: " << std::reduce(timing_sort_rbt.begin(), timing_sort_rbt.end(), 0.0) / timing_sort_rbt.size();*/
        fout << endl;

    }

    fout.close();
    return EXIT_SUCCESS;
}
