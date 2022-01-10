#pragma once

#include "dtree.h"
#include <fstream>
#include <sstream>

#define DEFAULT_HEIGHT 0

class Grader;   /* For grading purposes */
class Tester;   /* Forward declaration for testing class */

class UNode {
    friend class Grader;
    friend class Tester;
    friend class UTree;
public:
    UNode() {
        _dtree = new DTree();
        _height = DEFAULT_HEIGHT;
        _left = nullptr;
        _right = nullptr;
    }

    ~UNode() {
        delete _dtree;
        _dtree = nullptr;
    }

    /* Getters */
    DTree*& getDTree() {return _dtree;}
    int getHeight() const {return _height;}
    string getUsername() const {return _dtree->getUsername();}

private:
    DTree* _dtree;
    int _height;
    UNode* _left;
    UNode* _right;

};

class UTree {
    friend class Grader;
    friend class Tester;

public:
    UTree():_root(nullptr){}

    /* destructor */
    ~UTree();

    /* Basic operations */

    void loadData(string infile, bool append = true);
    bool insert(Account newAcct);
    bool removeUser(string username, int disc, DNode*& removed);
    UNode* retrieve(string username);
    DNode* retrieveUser(string username, int disc);
    int numUsers(string username);
    void clear();
    void printUsers() const;
    void dump() const {dump(_root);}
    void dump(UNode* node) const;


    /*"Helper" functions */
    
    void updateHeight(UNode* node);
    int checkImbalance(UNode* node);
    void rebalance(UNode*& node);

private:
  UNode* _root;
  void clearTree(UNode* node);
  UNode* leftRotation(UNode* node);
  UNode* rightRotation(UNode* node);
  UNode* retrieve(string username, UNode* node);
  bool insert(Account newAcct, UNode *&node);
  void remove(UNode*& node, string username);
  UNode* findMax(UNode* node);
  void printUsers(UNode *node) const;
  int checkBalance(UNode* node);
  void updateParents(UNode* node);
};
