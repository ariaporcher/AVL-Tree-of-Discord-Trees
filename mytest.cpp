#include "utree.h"
#include <random>

#define NUMACCTS 20
#define RANDDISC (distAcct(rng))

std::mt19937 rng(10);
std::uniform_int_distribution<> distAcct(0, 9999);

class Tester {
public:
  bool testBasicDTreeInsert(DTree& dtree);
  bool testBasicUTreeInsert(UTree& utree);
  
};

///////////////////////////////////////////////////////////////////////////

bool Tester::testBasicDTreeInsert(DTree& dtree) {
    bool allInserted = true;
    for(int i = 0; i < NUMACCTS; i++) {
        int disc = RANDDISC;
	cout << "inserted: " << disc << endl;	
        Account newAcct = Account("", disc, 0, "", "");
        if(!dtree.insert(newAcct)) {
            cout << "Insertion on node " << disc << " did not return true" << endl;
            allInserted = false;
        }
    }
    return allInserted;
}


///////////////////////////////////////////////////////////////////////////


bool Tester::testBasicUTreeInsert(UTree& utree) {
    string dataFile = "accounts.csv";
    try {
        utree.loadData(dataFile);
    } catch(std::invalid_argument e) {
        std::cerr << e.what() << endl;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////

int main() {
    Tester tester;

    /* Basic dtree tests */
    DTree dtree;
    DTree atree;

    // Test that the data was inserted.
    cout << "\n\t\tTESTING DTREE INSERTION:" << endl;
    if(tester.testBasicDTreeInsert(dtree)) {
        cout << "\t\tTest Passed!" << endl;
    } else {
        cout << "\t\tTest Failed!" << endl;
    }

    cout << "\n\tTesting insertion of node that already exists..." << endl;
    Account newAcc = Account("", 7713, 0, "", "");
    if(dtree.insert(newAcc)){
      cout << "\t\tTest Failed!" << endl;
    }else{
      cout << "\t\tTest Passed!" << endl;
    }

    //Print out the Account data in the tree in order
    cout << "\n\n\tResulting Accounts in DTree...";
    dtree.printAccounts();
    cout << endl;

    //Print out the tree in order
    cout << "\n\tResulting DTree..." << endl;
    dtree.dump();

    //Test copy constructor
    atree = dtree;
    cout << "\n\n\t\tTESTING OVERLOADED ASSIGNMENT OPERATOR:" << endl;
    cout << "\tResulting ATree..." << endl; //should be the same as dtree, prev dump
    atree.dump();

    //Test that a deep copy was made (addresses should be different)
    cout << "\n\n\tTesting that addresses of DTree and ATree are different:" << endl;
    atree.printRoot(atree);
    dtree.printRoot(dtree);
    cout << endl;

    {
     
    //Test Dtree remove functionalities
    cout << "\n\t\tTESTING DTREE REMOVE FUNCTIONALITIES:" << endl;
    cout << "\tTesting that node's removed are deleted after rebalance..." << endl;
    DNode* removeNode;
    DTree ctree;

    Account newAcct = Account("", 43, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 75, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 21, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 42, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 18, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 57, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 54, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 7, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 93, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 41, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 10, 0, "", "");
    ctree.insert(newAcct);
    
    
    cout << "\n\tTree after insertions, before remove..." << endl;
    ctree.dump();
    ctree.remove(42, removeNode);
    ctree.remove(57, removeNode);
    ctree.remove(54, removeNode);
    ctree.remove(41, removeNode);
    ctree.remove(75, removeNode);
    
    
    cout << "\n\n\tTree after removing values 42, 57, 54, 41, 75..." << endl;
    ctree.dump();
    
    newAcct = Account("", 80, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 85, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 64, 0, "", "");
    ctree.insert(newAcct);
    newAcct = Account("", 72, 0, "", "");
    ctree.insert(newAcct);

    cout << "\n\n\tTree after inserting 80, 85, 64, 72...(previosuly removed nodes should no longer appear in tree)" << endl;
    ctree.dump();
    cout << endl;
    
    cout << "\n\tTesting removal of node that doesn't exist" << endl;
    if(ctree.remove(200, removeNode)){
      cout << "\t\tTest Failed!" << endl;
    }else{
      cout << "\t\tTest Passed!" << endl;
    }
    }

/////////////////////////////////////////////////////////////////////////////////////////

//Test UNode insertion
    UTree utree;

    cout << "\n\n\tTESTING UTREE INSERTION:" << endl;
    if(tester.testBasicUTreeInsert(utree)) {
        cout << "\t\tTest Passed!" << endl;
    } else {
        cout << "\t\tTest Failed!" << endl;
    }

    cout << "\tResulting UTree:" << endl;
    utree.dump();
    cout << endl;

    cout << "\tResulting UTree printed Accounts..." << endl;
    utree.printUsers();
    
    cout << "\n\tTesting insertion of node that already exists..." << endl;
    Account newAccount = Account("Kippage",5482, 0, "", "");
    if(utree.insert(newAccount)){
      cout << "\t\tTest Failed!" << endl;
    }else{
      cout << "\t\tTest Passed!" << endl;
    }

    cout << "\t\tTESTING UNODE REMOVE FUNCTIONALITY: " << endl;
    DNode *removed;
    cout << "Removing a node that doesn't exist..." << endl;
    if(utree.removeUser("Bill", 5, removed)){
      cout << "\t\tTest Failed!" << endl;
    }else{
      cout << "\t\tTest Passed!" << endl;
    }
    
    UTree vtree;
    cout << "Removing a node that does exist..." << endl;
    if(utree.removeUser("Cinnamon", 8003, removed)){
      cout << "\n\tTest Passed!" << endl;
    }else{
      cout << "\n\tTest Failed!" << endl;
    }

    Account newAcct = Account("Cali", 2, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Ari", 4, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Band", 5, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Trish", 3, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Fig", 8, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Kris", 9, 0, "", "");
    vtree.insert(newAcct);
    newAcct = Account("Cali", 11, 0, "", "");
    vtree.insert(newAcct);

    cout << "Testing that UNode is removed when DTree contains no non-vacant nodes..." << endl;
    vtree.dump();
    cout << endl;    
    int remove = vtree.removeUser("Cali", 11, removed);
    remove = vtree.removeUser("Cali", 2, removed);
    cout << "dumping vtree" << endl;
    vtree.dump();
          
    return 0;
}
