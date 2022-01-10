#include "dtree.h"

/**
 * Destructor, deletes all dynamic memory.
**/
DTree::~DTree() {
  clear();
}

/**
 * Overloaded assignment operator, makes a deep copy of a DTree.
 * @param rhs Source DTree to copy
 * @return Deep copy of rhs
**/
DTree& DTree::operator=(const DTree& rhs) {
  if(this != &rhs){
    clear();
    DNode* newRoot = new DNode(rhs._root->_account);
    _root = newRoot;
    makeDeep(rhs._root, _root);
  }
  return *this;
}

/**
 * Helper function for overloaded assignment operator.
 * creates a deep copy.
 **/
void DTree::makeDeep(const DNode* rhs, DNode*& node){
  if(rhs != nullptr){
    if(node == _root){
      makeDeep(rhs->_left, node->_left);
      makeDeep(rhs->_right, node->_right);
    }else{
      DNode* newNode = new DNode(rhs->_account);
      node = newNode;
      makeDeep(rhs->_left, node->_left);
      makeDeep(rhs->_right, node->_right);
    }
  }

}

//Tests that a deep copy was made. (used in mytest)
void DTree::printRoot(DTree& node){
  cout << node._root << endl;  
}

/**
 * Dynamically allocates a new DNode in the tree. 
 * Should also update heights and detect imbalances in the traversal path
 * an insertion.
 * @param newAcct Account object to be contained within the new DNode
 * @return true if the account was inserted, false otherwise
**/
bool DTree::insert(Account newAcct) {
  //ONLY insert newAcct if it doesn't already exist in Dtree. 
  if(retrieve(newAcct._disc) != nullptr){
    return false;
  }
  bool success = insert(newAcct, _root);
  helpUpdateSize(_root);
  helpCheckBalance(_root);
  return success;
}


bool DTree::insert(Account newAcct, DNode*& node){
  if(node == nullptr){
    DNode* newNode = new DNode(newAcct);
    node = newNode;
    return true;
  }else{
    if(newAcct._disc < node->_account._disc){
      if(node->isVacant()){
	DNode* newNode = new DNode(newAcct);
	node = newNode;
	node->_vacant = false;
	updateNumVacant(node);
	return true;
      }else{
	return insert(newAcct, node->_left);
      }
    }else{
      if(node->isVacant()){
	DNode* newNode = new DNode(newAcct);
	node = newNode;
	node->_vacant = false;
	updateNumVacant(node);
	return true;
      }else{
	return insert(newAcct, node->_right);
      }
    }
  }
  return false;
}

/**
 *Helper function to updateSize of node.
 * this function will take in the root node of the tree, do an inorder
 * traversal through the tree and update it's size at each node.
**/
void DTree::helpUpdateSize(DNode* node){
  if(node == nullptr){return;}
  helpUpdateSize(node->_left);
  helpUpdateSize(node->_right);
  updateSize(node);
  updateNumVacant(node);
}

/**
 *Helper function to checkImbalance of node.
 * this function will take in the root node of the tree, do an inorder
 * traversal through the tree and check if it's imbalanced at each node.
**/
void DTree::helpCheckBalance(DNode*& node){
  if(node == nullptr){return;}
  helpCheckBalance(node->_left);
  helpCheckBalance(node->_right);
  if(checkImbalance(node)){
    rebalance(node);
    //helpCheckBalance(_root);
  }
}



/**
 * Removes the specified DNode from the tree.
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
**/

bool DTree::remove(int disc, DNode*& removed) {
  DNode* nodeRemoved = retrieve(disc);
  if(nodeRemoved != nullptr){
    removed = nodeRemoved;
    removed->_vacant = true;
    removed->_numVacant ++;
    updateParents(disc, _root);
    return true;
  }else{return false;}
}
  
  
/**
 * Helper function for remove.
 • if the node the user wants to remove is found, it wil become vacant and we'll need to
 * update _numVacant for all parent nodes of the newly vacant node.
**/
void DTree::updateParents(int disc, DNode*& parent){
  if(disc == parent->_account._disc){
    return;
  }else{
    if(disc < parent->_account._disc){
      parent->_numVacant ++;
      updateParents(disc, parent->_left);
    }else{
      parent->_numVacant ++;
      updateParents(disc, parent->_right);
    }
  }
}

/**
 * Retrieves the specified Account within a DNode.
 * @param disc discriminator int to search for
 * @return DNode with a matching discriminator, nullptr otherwise
**/
DNode* DTree::retrieve(int disc) {
  DNode* matchFound = retrieve(disc, _root);
  return matchFound;
}


DNode* DTree::retrieve(int disc, DNode* node) {
  if(node != nullptr){
    if(node->_account._disc == disc){
      if(!node->isVacant()){
	return node; //if node isn't vacant and a match, return curr node
      }else{
	return nullptr; //if node is vacant, return null
      }
  }else if(disc < node->_account._disc){
      return retrieve(disc, node->_left);
    }else{
      return retrieve(disc, node->_right);
    }
  }
    return nullptr; //return null if no match is found
}

/**
 * Helper for the destructor to clear dynamic memory.
**/
void DTree::clear() {
  if (_root != nullptr){
    clearTree(_root);
    _root = nullptr;
  }
}

void DTree::clearTree(DNode* node){
  if (node == nullptr)
    return;
  else{
    clearTree(node->_left);
    clearTree(node->_right);
    delete node;
    }
}

/**
 * Prints all accounts' details within the DTree.
**/
void DTree::printAccounts() const {
  printAccounts(_root);
}

void DTree::printAccounts(DNode* node) const {
  if(node == nullptr){
    return;
  }
  printAccounts(node->_left);
  if(!node->isVacant()){
    cout << endl << node->_account;
  }
  printAccounts(node->_right);
}

/**
 * Dump the DTree in the '()' notation.
**/
void DTree::dump(DNode* node) const {
  if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getAccount().getDiscriminator() << ":" << node->getSize() << ":" << node->getNumVacant();
    dump(node->_right);
    cout << ")";
}

/**
 * Returns the number of valid users in the tree.
 * @return number of non-vacant nodes
**/
int DTree::getNumUsers() const {
  return _root->_size - _root->_numVacant;
}

/**
 * Updates the size of a node based on the imedaite children's sizes
 * @param node DNode object in which the size will be updated
**/
void DTree::updateSize(DNode* node) {
  if(node->_left == nullptr && node->_right == nullptr){
    node->_size = DEFAULT_SIZE;
  }else if(node->_left == nullptr && node->_right != nullptr){
    node->_size = node->_right->_size + 1;
  }else if(node->_right == nullptr && node->_left != nullptr){
    node->_size = node->_left->_size + 1;
  }else{
    node->_size = node->_left->_size + node->_right->_size + 1;
  }
}

/**
 * Updates the number of vacant nodes in a node's subtree based on the immediate children
 * @param node DNode object in which the number of vacant nodes in the subtree will be updated
**/
void DTree::updateNumVacant(DNode* node) {
  if(node->_left == nullptr && node->_right == nullptr){
    node->_numVacant = 0;
  }else if (node->_left == nullptr && node->_right != nullptr){
    node->_numVacant = node->_right->_numVacant;
  }else if (node->_right == nullptr && node->_left != nullptr){
    node->_numVacant = node->_left->_numVacant;
  }else{
    node->_numVacant = node->_left->_numVacant + node->_right->_numVacant;
  }

  if(node->isVacant()){
    node->_numVacant ++;
  }
}

/**
 * Checks for an imbalance, defined by 'Discord' rules, at the specified node.
 * @param checkImbalance DNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
**/
bool DTree::checkImbalance(DNode* node) {
  //for imbalance, at least one child's size must be 4 or greater...
  int leftSize = 0;
  int rightSize = 0;
  if (node->_left != nullptr)
    leftSize = node->_left->_size;
  if (node->_right != nullptr)
    rightSize = node->_right->_size;

  //  cout << "checking imbalance at " << node->_account._disc << endl;
  //cout << "node's left child size: " << leftSize << " and right child: "
  //   << rightSize << endl << endl;
  
  if(leftSize >= 4 || rightSize >=4){
    //and the size of on`e child must be at least 50% bigger than the
    //size of the other child
    if(leftSize >= rightSize * 1.5 || rightSize >= leftSize * 1.5){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}


//----------------
/**
 * Begins and manages the rebalancing process for a 'Discrd' tree (pass by reference).
 * @param node DNode root of the subtree to balance
**/
void DTree::rebalance(DNode*& node) {
  if(node!= nullptr){
    DNode** tempArr = new DNode *[(node->_size - node->_numVacant)];
    int sizeArr = node->_size - node->_numVacant;
    int count = 0;
    int index = ((sizeArr-1)/2);
    fillArr(tempArr, node, sizeArr, count);
    
    //set all left and right pointers in array at each node equal to null
    // and set their sizes equal to 1.
    for(int i = 0; i < sizeArr; i++){
      tempArr[i]->_left = nullptr;
      tempArr[i]->_right = nullptr;
      //cout << tempArr[i]->_account._disc << ", ";
    }
    node = tempArr[index];
    //set node equal to the middle element in the array, this will be the new root
    //of the subtree.
    fillTree(tempArr, tempArr[index], index, sizeArr); 
    delete [] tempArr;
    helpUpdateSize(node);
  }
}

/**
 * Helper function for rebalance function.
 * Fills the array using in order traversal of the subtree needing to be rebalanced
 * and puts each node in a cell of the array in order from least to greatest. 
**/

void DTree::fillArr(DNode**& tempArr, DNode*& node, int count, int &index){
  if(node!= nullptr){
    if(node->isVacant()){
      fillArr(tempArr, node->_left, count, index);
      fillArr(tempArr, node->_right, count, index);
      delete node;
    }else{
      fillArr(tempArr, node->_left, count, index);
      fillCell(tempArr, node, index++);
      fillArr(tempArr, node->_right, count, index);
    }
  }
}


void DTree::fillCell(DNode **&tempArr, DNode*& node, int index){
  tempArr[index] = node;
}

void DTree::fillTree(DNode **&tempArr, DNode*& node, int &index, int size){
  if(size == 1){
    return;
  }
  if(size == 2){
    node->_left = tempArr[0];
    return;
  }
  if (size == 3){
    node->_left = tempArr[0];
    node->_right = tempArr[2];
    return;
  }
  if(index == 0){
    index ++;
    int filled = fillInnerNodes(tempArr, node, index, size);
    index = (size+1)/2; //send in the root node and the index for the first element on the right
    return fillTree(tempArr, tempArr[(size-1)/2], index, size);
  }else if(index != 0 && index != size - 1){
    //half the left side of the tree
    if(index <= (size-1)/2){
      index = (index-1)/2;
      node->_left = tempArr[index];
      return fillTree(tempArr, node->_left, index, size);
    }else{ //half the right side of the tree
      index = (size+index)/2;
      node->_right = tempArr[index];
      return fillTree(tempArr, node->_right, index, size);
    }
  }else{
    index --;
    int filled = fillInnerNodes(tempArr, node, index, size);
    if(filled){return;}
  }
}

bool DTree::fillInnerNodes(DNode **&tempArr, DNode*& node, int &index, int size){
  //when the root of the subtree is reached, the left or right side is filled.
  if(index == (size-1)/2){
    return true;
  }
  //fills the left side of the tree
  if(index < (size-1)/2){
    if(tempArr[index]->_left != nullptr){
      index++;
      int retValue = fillInnerNodes(tempArr, tempArr[index-1], index, size);
      if (retValue){
	return false;
      }
    }else{
      node->_right = tempArr[index];
      index++;
      int retValue =  fillInnerNodes(tempArr, node->_right, index, size);
      if (retValue){return false;}
    }
  }else{ //fills the right side if the tree
    if(tempArr[index]->_right != nullptr){
      index--;
      return fillInnerNodes(tempArr, tempArr[index+1], index, size);
    }else{
      node->_left = tempArr[index];
      index--;
      return fillInnerNodes(tempArr, node->_left, index, size);
    }
    
  }
}


/**
 * Overloaded << operator for an Account to print out the account details
 * @param sout ostream object
 * @param acct Account objec to print
 * @return ostream object containing stream of account details
**/

ostream& operator<<(ostream& sout, const Account& acct) {
    sout << "Account name: " << acct.getUsername() << 
            "\n\tDiscriminator: " << acct.getDiscriminator() <<
            "\n\tNitro: " << acct.hasNitro() << 
            "\n\tBadge: " << acct.getBadge() << 
            "\n\tStatus: " << acct.getStatus();
    return sout;
}
