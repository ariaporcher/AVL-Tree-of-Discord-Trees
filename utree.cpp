
#include "utree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
UTree::~UTree() {
  clear();
}

/**
 * Sources a .csv file to populate Account objects and insert them into the UTree.
 * @param infile path to .csv file containing database of accounts
 * @param append true to append to an existing tree structure or false to clear before importing
 */ 
void UTree::loadData(string infile, bool append) {
    std::ifstream instream(infile);
    string line;
    char delim = ',';
    const int numFields = 5;
    string fields[numFields];

    /* Check to make sure the file was opened */
    if(!instream.is_open()) {
        std::cerr << __FUNCTION__ << ": File " << infile << " could not be opened or located" << endl;
        exit(-1);
    }

    /* Should we append or clear? */
    if(!append) this->clear();

    /* Read in the data from the .csv file and insert into the UTree */
    while(std::getline(instream, line)) {
        std::stringstream buffer(line);

        /* Quick check to make sure each line is formatted correctly */
        int delimCount = 0;
        for(unsigned int c = 0; c < buffer.str().length(); c++) if(buffer.str()[c] == delim) delimCount++;
        if(delimCount != numFields - 1) {
            throw std::invalid_argument("Malformed input file detected - ensure each line contains 5 fields deliminated by a ','");
        }

        /* Populate the account attributes - 
         * Each line always has 5 sections of data */
        for(int i = 0; i < numFields; i++) {
            std::getline(buffer, line, delim);
            fields[i] = line;
        }
        Account newAcct = Account(fields[0], std::stoi(fields[1]), std::stoi(fields[2]), fields[3], fields[4]);
        this->insert(newAcct);
    }
}

/**
 * Dynamically allocates a new UNode in the tree and passes insertion into DTree. 
 * Should also update heights and detect imbalances in the traversal path after
 * an insertion.
 * @param newAcct Account object to be inserted into the corresponding DTree
 * @return true if the account was inserted, false otherwise
 */
bool UTree::insert(Account newAcct) {
  int success = insert(newAcct, _root);
  return success;
}


bool UTree::insert(Account newAcct, UNode *&node) {
  if(node == nullptr){
    DTree *newDTree = new DTree();
    if(newDTree->insert(newAcct)){ //if Account doesn't already exist, create 
      UNode *newNode = new UNode();
      node = newNode;
      node->_dtree = newDTree;
      updateHeight(node);
      rebalance(node);
      return true;
    }else{
      return false;}
  }
  if(newAcct.getUsername() == node->getUsername()){
    //insert account into dtree if username is the same
    int success = node->_dtree->insert(newAcct);
    if(!success){return false;
    }else{return true;}
  }else if(newAcct.getUsername() < node->getUsername()){
    bool inserted = insert(newAcct, node->_left);
    updateHeight(node);
    rebalance(node);
    return inserted;
  }else{ 
    bool inserted = insert(newAcct, node->_right);
    updateHeight(node);
    rebalance(node);
    return inserted;
  }
}

/**
 * Removes a user with a matching username and discriminator.
 * @param username username to match
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool UTree::removeUser(string username, int disc, DNode*& removed) {
  cout << "Removing: " << username << " at disc: " << disc << endl;
  UNode* found = retrieve(username);
  //if no UNode with the username was found, return false.
  if(found == nullptr){return false;}
  found->_dtree->remove(disc, removed);
  //if no DNode with the disc was removed, return false.
  if(removed == nullptr){return false;}
  if(numUsers(username) == 0){
    //if all nodes in dtree are vacant, delete UNode.
    remove(found, username);
    updateParents(_root);
  }
  return true;
}

void UTree::updateParents(UNode* node){
  updateParents(node->_left);
  updateParents(node->_right);
  updateHeight(node);
}

void UTree::remove(UNode*& node, string username){
  UNode* nodeX;
  if(node->_left != nullptr){
    // find left subtree's highest value
    nodeX = findMax(node->_left);
    // copy its value to the node that we want to delete
    node->_dtree = nodeX->_dtree;
    if(nodeX->_left != nullptr){
      nodeX->_dtree = nodeX->_left->_dtree;
      delete nodeX->_left;
      updateHeight(node);
      rebalance(node);
      cout << "UNode was removed!" << endl; //myTest print statement
      return;
    }
  }
  else{
    nodeX = node;
    if(node->_left == nullptr && node->_right != nullptr)
      node->_dtree = node->_right->_dtree;
    
  }
  delete nodeX;
  //nodeX = nullptr;
  cout << "UNode was removed!" << endl; //myTest print statement
  updateHeight(node);
  rebalance(node);
}



UNode* UTree::findMax(UNode* node){
  if (node == nullptr || node->_right == nullptr)
    return node;
  else //to find max we only need to move to the right
    return findMax(node->_right);
}

/**
 * Retrieves a set of users within a UNode.
 * @param username username to match
 * @return UNode with a matching username, nullptr otherwise
 */
UNode* UTree::retrieve(string username) {
  UNode* node = retrieve(username, _root);
  if(node!= nullptr){
  }
  return node;
}

UNode* UTree::retrieve(string username, UNode* node) {
  if(node != nullptr){
    if(node->getUsername() == username){
      return node; 
    }else if(username < node->getUsername()){
      return retrieve(username, node->_left);
    }else{
      return retrieve(username, node->_right);
    }
  }
  return nullptr; //return null if no match is found
}



/**
 * Retrieves the specified Account within a DNode.
 * @param username username to match
 * @param disc discriminator to match
 * @return DNode with a matching username and discriminator, nullptr otherwise
 */
DNode* UTree::retrieveUser(string username, int disc) {
  UNode* node = retrieve(username, _root);
  if(node != nullptr){
    DNode* found = node->_dtree->retrieve(disc);
    if(found != nullptr)
      return found;
  }
  return nullptr;
}

/**
 * Returns the number of users with a specific username.
 * @param username username to match
 * @return number of users with the specified username
 */
int UTree::numUsers(string username) {
  UNode *found = retrieve(username);
  return found->_dtree->getNumUsers();
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void UTree::clear() {
  clearTree(_root);
}
    

void UTree::clearTree(UNode* node){
  if (node == nullptr)
    return;
  else{
    clearTree(node->_left);
    node->_dtree->clear();
    clearTree(node->_right);
    delete node;
    }
}

/**
 * Prints all accounts' details within every DTree.
 */
void UTree::printUsers() const {
  printUsers(_root);
}

void UTree::printUsers(UNode *node) const {
  if(node == nullptr){
    return;
  }else{
    printUsers(node->_left);
    node->_dtree->printAccounts();
    printUsers(node->_right);
  }
}

/**
 * Dumps the UTree in the '()' notation.
 */
void UTree::dump(UNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getUsername() << ":" << node->getHeight() << ":" << node->getDTree()->getNumUsers();
    dump(node->_right);
    cout << ")";
}

/**
 * Updates the height of the specified node.
 * @param node UNode object in which the height will be updated
 */
void UTree::updateHeight(UNode* node) {
 
  if(node == nullptr){return;}
   int leftHeight = (node->_left == nullptr ? -1 : node->_left->_height);
   int rightHeight = (node->_right == nullptr ? -1 : node->_right->_height );
   node->_height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight );
} 

/**
 * Checks for an imbalance, defined by AVL rules, at the specified node.
 * @param node UNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
int UTree::checkImbalance(UNode* node) {
  if(node == nullptr){
    return true;
  }else{
    int leftHeight = -1;
    int rightHeight = -1;
    if(node->_left != nullptr){
      leftHeight = node->_left->_height;
    }
    if(node->_right != nullptr){
      rightHeight = node->_right->_height;
    }
    if(leftHeight - rightHeight <= 1 && leftHeight - rightHeight >= -1){
      return false;
     }
  }
  return true;
}



int UTree::checkBalance(UNode* node) {
  if(node == nullptr){
    return -1;
  }else{
    int leftHeight = -1;
    int rightHeight = -1;
    if (node->_left != nullptr){
      leftHeight = node->_left->_height;
    }
    if (node->_right != nullptr){
      rightHeight = node->_right->_height;
    }
    return (leftHeight - rightHeight);
  }
}


//----------------
/**
 * Begins and manages the rebalance procedure for an AVL tree (pass by reference).
 * @param node UNode object where an imbalance occurred
 */
void UTree::rebalance(UNode*& node) {
  if ((checkBalance(node) < -1) && (checkBalance(node->_right) <= 0))
    node = leftRotation(node);//do left rotation at z
  else if ((checkBalance(node) > 1) && (checkBalance(node->_left) >= 0))
    node = rightRotation(node);//do right rotation at z
  else if ((checkBalance(node) < -1) && (checkBalance(node->_right) >=0)){
    //double rotation, right rotation at node->_right, left rotation at node
    node->_right = rightRotation(node->_right);//right rotation at node->_right
    node = leftRotation(node);//left rotation at node
  }
  else if ((checkBalance(node) > 1) && (checkBalance(node->_left) <= 0)){
    //double rotation, left rotation at node->_left, right rotation at node
    node->_left = leftRotation(node->_left);//left rotation at node->_left
    node = rightRotation(node);//right rotation at node
  }
}

/**
 * Helper function to reblance node.
 * Performs a left rotation on the node passed in.
**/
UNode* UTree::leftRotation(UNode* node){
  UNode* z = node;
  UNode* y = z->_right;
  z->_right = y->_left;
  y->_left = z;
  updateHeight(z);
  updateHeight(y);
  return y;
}

/**
 * Helper function to reblance node.
 * Performs a right rotation on the node passed in.
**/
UNode* UTree::rightRotation(UNode* node){
  UNode* z = node;
  UNode* y = z->_left;
  z->_left = y->_right;
  y->_right = z;
  updateHeight(z);
  updateHeight(y);
  return y;
}

