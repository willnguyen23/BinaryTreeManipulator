#include <vector>
#include <iostream>
using namespace std;

/***** Write your exception class here ******* SEE main function *****/
class duplicateInsertion: public exception {
    protected:
        int value;
    
    public:
        duplicateInsertion(int value) {
            (*this).value = value;
        }
        void error() {
            cout << "The value = " << value << " already in the tree." << endl;
        }
};
class NotFoundException: public exception {
    protected:
        int value;
    
    public:
        NotFoundException(int value) {
            (*this).value = value;
        }
        void error() {
            cout << "The value = " << value << " not found." << endl;
        }
};

template <class DT>
class MTree {
    protected:
        int M; // Maximum number of children per node (M+1 way split)
        vector<DT>* values; // Values stored in the node (M-1 values)e
        vector<MTree*>* children; // Pointers to child MTrees (M+1 children)

    public:
        MTree(int M); // regular constructor
        ~MTree();
        bool is_leaf() const; // Check if the current node is a leaf
        void insert(DT& value); // Insert a value into the MTree
        void remove(DT& value); // Delete a value from the MTree
        void buildTree(vector<DT>& input_values); // Build the tree
        vector<DT> collect_values(); // Collect values from all leaf nodes
        bool find(DT& value);
        int search(DT& value);
};

template <class DT>
MTree<DT>:: MTree(int M) {
    (*this).M = M; // assigns M into the M value
    values = new vector<DT>(); // ensures that there are only m-1 values in the node
    children = new vector<MTree*>();
}

template <class DT> 
MTree<DT>:: ~MTree() {
    (*children).clear();
    (*values).clear();
}

template <class DT>
void MTree<DT>:: buildTree(vector<DT>& input_values) { // recursive function
    int partitions = input_values.size() / M; // assigns partitions the amount of values going into each children and what index is being inserted into values   
    int multiplier = 1; // helps assigns the next partition value into the value vector
    int start = 0; // variable holds the first index that will be put into the recrusive call
    int end = partitions; // variables holds the last index that will be put into the recursive call
    (*children).clear(); // clears the vectors so that it wont add more than neccessary
    (*values).clear();

    // If there are remaining values in the partitioned vector, then it will continue to recursively call the buildTree vector
    // If it is 0, then that means there are no more values to be inserted
    if (partitions > 0) {
        for (int a = 0; a < (M - 1); ++a) { // inserts values from partition index into the node's value vector
            (*values).push_back(input_values[(partitions * multiplier) - 1]);
            multiplier++; // increments multiplier to move to the next value that will be inputted into
        }
        for (int b = 0; b < M; ++b) { // for loop is to intialize all children values
            vector<int> sub(input_values.begin() + start, input_values.begin() + end); // statically creates a vector 
            
            (*children).push_back(new MTree<DT>(M)); // inserts it into children
            (*(*children).at(b)).buildTree(sub);
            
            start += partitions; // updates start to the first element of the next section of the vector
            if (b == (M - 2)) { // considers the last section of the vector that will include more than the previous children
                 end = end + partitions + input_values.size() % M;
            }
            else {
                end += partitions;
            }

            sub.clear(); // clears the vector so we can reuse it
        }
    }
    else if (partitions == 0) { // conditional for when is reaches a leaf node or when partition is 0
        for (int a = 0; a < ((input_values).size() % M); ++a) { // inserts values from partition index into the node's value vector
            (*values).push_back(input_values[a]);
        }
    }
}

template <class DT>
bool MTree<DT>:: find(DT& value) { // Uses the search function to determine if value is there
    int index = (*this).search(value);

    if (index == -1) {
        return false;
    }
    else {
        return true;
    } 
}

template <class DT>
int MTree<DT>:: search(DT& value) { // searches for the index of a value
    vector<int> allValues = (*this).collect_values(); // puts all values into a vector using collect_values function

//  Finds value with non-recursive binary search
    bool isFound = false;
    int mid;
    int low = 0;
    int high = allValues.size() - 1;  
    int index = -1;

    while (!isFound && (low <= high)) {
        int mid = low + (high - low) / 2;
    //    cout << low << " " << mid << " " << high << endl;

        if (allValues.at(mid) == value) {
            index = mid;
            isFound = true;
        }
        else if (allValues.at(mid) < value) {
            low = mid + 1;
        }
        else if (allValues.at(mid) > value) {
            high = mid - 1;
        }
    }

    return index;
}

template <class DT>
bool MTree<DT>:: is_leaf() const {
    bool isLeaf = false; 

    if ((*children).size() == 0) {
        isLeaf = true;
    }
    
    return isLeaf;
}

template <class DT> 
vector<DT> MTree<DT>:: collect_values() { // Divide and conquer (merges)
    vector<DT> allValues; // the vector that will be returned

    if ((*this).is_leaf()) { // conditional for when the node is finally a leaf
        for (int k = 0; k < (*values).size(); ++k) {
            (allValues).push_back((*values).at(k)); // inserts all values of the node into vector that will be returned and appended to the main vector
        }
    }
    else {
       for (int j = 0; j < (*children).size(); ++j) { // goes through all of the children to get all leaf nodes
    // recursively calls the function so that it can access the leaf nodes
    // the vector temp gets the values that was collected recursively to insert into the returned vector
            vector<DT> temp = (*(*children).at(j)).collect_values();
            for (int i = 0; i < temp.size(); ++i) { // for loops inserts all of the values of each leaf children
                (allValues).push_back((temp).at(i));
            }
       }
    }

    return allValues;
}

template <class DT>
void MTree<DT>:: insert(DT& value) {
    int temp; // used for swapping
    int valuePlace; // holds index of the current value during the swap

    if ((*this).find(value)) { // throws exception of value is already in the tree
        throw duplicateInsertion(value);
    }
    else {
        vector<DT> allValues = (*this).collect_values(); // gets all of the values into a vector
        allValues.push_back(value); // adds it to the end of vector
        valuePlace = allValues.size() - 1; // assigns the index of value

// For loop swaps until the value reaches its supposed to be spot
        for (int i = allValues.size() - 2; i >= 0; --i) {
            if (value < allValues.at(i)) {
                temp = allValues.at(valuePlace);
                allValues.at(valuePlace) = allValues.at(i);
                allValues.at(i) = temp;
                valuePlace--;
            }
        }

        (*this).buildTree(allValues); // rebuilds tree
    }
}

template <class DT>
void MTree<DT>:: remove(DT& value) {
    int index; // holds the index of the value being removed

    if (!((*this).find(value))) {
        throw NotFoundException(value);
    }
    else {
        vector<DT> allValues = (*this).collect_values(); // puts all values into a vector
        index = (*this).search(value); // assigns the index of removed value

        allValues.erase(allValues.begin() + index); // removes it
        (*this).buildTree(allValues); // rebuilds tree
    }
}

int main() {
    int n; // number of numbers in the initial sorted array
    int MValue;
    int numCommands;
    char command;
    int value;

    cin >> n;
    vector<int> mySortedValues(n);

    //read n numbers from the input and add them to the vector mySortedValues
    for (int i = 0; i < n; ++i) {
        cin >> value;
        mySortedValues[i] = value;
    }

    //Get the M value
    cin >> MValue;
    MTree<int>* myTree = new MTree<int>(MValue);

    //Build the tree
    (*myTree).buildTree(mySortedValues);

    cin >> numCommands; // Read the number of commands

/************** Read each command Process ***************/
    for (int i = 0; i < numCommands; i++) {
        cin >> command; // Read the command type

        switch (command) {
            case 'I': { // Insert
                cin >> value;

                try {
                    (*myTree).insert(value);
                    cout << "The value = " << value << " has been inserted." << endl;
                }
                catch (duplicateInsertion& e) {
                    e.error();
                }
                break;
            }
            case 'R': { // Remove
                cin >> value;

                try {
                    (*myTree).remove(value);
                    cout << "The value = " << value << " has been removed." << endl;
                }
                catch (NotFoundException& e) {
                    e.error();
                }
                break;
            }
            case 'F': { // Find
                cin >> value;

                if ((*myTree).find(value)) {
                    cout << "The element with value = " << value << " was found." << endl;
                }
                else {
                    cout << "The element with value = " << value << " not found." << endl;
                }
                break;
            }
            case 'B': { // rebuild tree
                vector<int> myValues = (*myTree).collect_values();
                (*myTree).buildTree(myValues);
                
                cout << "The tree has been rebuilt." << endl;
            
                break;
            }
            default:
                cout << "Invalid command!" << endl;
        }
    }

    vector<int> allValues = (*myTree).collect_values();
    cout << "Final list: ";

    for (int i = 0; i < allValues.size(); ++i) {
        if (i % 20 == 0 && i != 0) {
            cout << endl;
        }
        cout << allValues.at(i) << " ";
    }

    delete myTree;
    
    return 0;
};

/***** Debugging plan and LLM Usage ******/
/*

Approach: 
    In the beginning, I want to monitor my progress on the progress by basing my result off of the "input1.txt" file and seeing if it matches
the output of "output1.txt". My plan of action is to first complete the constructors and buildTree function of MTree then complete each case 
in the main function one-by-one, completing all of the MTree class functions and exceptions. I will start with whatever case is called first 
in the input file. If the output matches with the txt file, then I will move on to the next case that is called in the input file.
------------------------------------------------------------------------------------------------------------------------------------------------------------
Adding values into the mySortedValues vector

LLM Usage:
    I only asked ChatGPT "How to insert value into a vector in C++", which is done with the push_back method.

Debugging:
    I wanted to check if the elements were inserted correctly in the array, so I used this for loop to check.

    for (int j = 0; j < n; ++j) {
            cout << j << ": " << mySortedValues.at(j) << endl;
    }

    If all of the values in the input file are shown in the output, the elements were inserted correctly. However, this did not work. As it
showed...

    0: 0
    1: 0
    2: 0
    ... skipping to index 200
    200: 5
    201: 8
    202: 10

    When I was performing the push_back method, it would add elements after the elements that were already inserted when initializing the vector. 
Therefore, I changed from mySortedValues.push_back(value) to mySortedValues[i] = value, which prints the desired output.
------------------------------------------------------------------------------------------------------------------------------------------------------------
MTree(int M) (constructor)

LLM Usage: 
    I did not use any LLM tools for the constructor. If I did, however, I would ask "How can I implement this constructor that initalizes
the MValue?"

Debugging:
    I do not think I need to test this as if it does not work the other functions will not work, so I can check this when I implement other
functions.
------------------------------------------------------------------------------------------------------------------------------------------------------------
void buildTree(vector<DT>& input_values)

LLM Usage: 
    "How to take part of a vector and use it as an argument for a function in c++?" I asked this because I wanted to take part of input_values
and put it into the recursive call of buildTree. I was given...

     vector<int> sub(input_values.begin() + start, input_values.begin() + end); // statically creates a vector 

This will be used to recursively call the buildTree function.

Debugging:
    When implementing this function, I thoought that keeping track of 200 values was too much to do without error. Therefore, I am using the
example that was in the ProjectFour.pdf that has 14 values to check this. I created another txt file in the same format as the other input files
with the values used in the example. Starting off, I wanted to make sure I calculated partitions right, so I wrote...

    cout << partitions << endl;

It printed 4, which is the right number. Next I wanted to see if it placed the right numbers into the node's values vector. I used...

    for (int i = 0; i < (*values).size(); ++i) {
        cout << (*values).at(i) << endl;
    }

It printed out 9 and 18, so I am on the right track because those are the values stored in the first node in the pdf. Next, I wanted to see if the
next level of the tree was built correctly. Therefore, I wrote this for loop...

    for (int i = 0; i < (*values).size(); ++i) {
       cout << (*values).at(i) << endl;
    } 

It did not give me the results I wanted, but I figured out it was from code error. The first thing was me forgetting to consider the remainder values
when entering the last section of values for the last tree. Once I fixed that, the second level looked correct. Next, I wanted to work on the next and 
last level. The output of the for loop above will print zeros, which I assume is the children. I figured out this was due to the partition value for the
leaf nodes being 0, so no matter what the leaf nodes will have zero values. To fix this, I put the for loop that assigns the value vector into the already
made conditional, and I created a new else conditional that will consider this situation. After this fix, the tree is properly built.
------------------------------------------------------------------------------------------------------------------------------------------------------------
bool find (DT& value)

LLM Usage:
    I did not use any LLM Tools. If I did, however, I would ask "How can I create a find function for a tree?"

Debugging:
    I can tell this works because the printed statements matches with the output file.
    
------------------------------------------------------------------------------------------------------------------------------------------------------------
bool is_leaf() const

LLM Usage:
    I did not use any LLM Tools. However, if I did, I would ask "How can I tell if a node of a tree is the leaf?"

Debugging:
    Since this goes hand-and-hand with find, I will know if this works if find works. Implementing the find function will help me figure
out what is wrong with this function. I know that this works because I have a conditional for if is_leaf = true. When it does hit this conditional, which it should
every time, it will print out 1. It did, so this works.
------------------------------------------------------------------------------------------------------------------------------------------------------------
vector<DT>& MTree<DT>:: collect_values()

LLM Usage:
    I did not use any LLM Tools. If I did, I would ask "How can I collect all of the values in the leaf nodes?"

Debugging:
    To check if I collected all of the values, I placed a for loop in the rebuild switch case and see if it printed out of the values. The loop I created was...

        for (int i = 0; i < myValues.size(); ++i) {
            cout << myValues.at(i) << endl;
        }

At first, it did not print anything, so I printed out the size and it printed out zero. Thus, I did not implement this correctly. To fix this, I created
a temporary vector that will hold that section's values and then I would push back the value of those vectors into allValues. This ended up working.
------------------------------------------------------------------------------------------------------------------------------------------------------------
void MTree<DT>:: insert(const DT& value) and void MTree<DT>:: insert(const DT& value)

LLM Usage:
    I did not use any LLM tools. I would ask "How to insert and remove values into a tree?"

Debugging:
    When implementing these functions, the terminal says "floating point exception". According to ChatGPT, this could mean I have an
arithmetic error that could include division by zero, over or underflow, and an invalid operaiton. I also figured out that when i was rebuilding the tree, instead
of recreating it, it added on, which led to a lot of issues. To fix this, I delete the vector every time the function is performed. I did this in the buildTree function
, which was where it was happening, and now it works. What would happen is that due to the vectors not being cleared, the push_back method will jsut add on to the vector
instead of replacing it.
------------------------------------------------------------------------------------------------------------------------------------------------------------
class duplicateInsertion: public exception and class NotFoundException: public exception

LLM Usage:
    I kind of forgot how to implement a custom exception class, so I looked up how to do it. I asked "How can I create my own exception class in C++?"

Debugging:
    After implementing what I learned, I get an error saying "use of class template 'duplicateInsertion' requires template arguments" in the terminal. This means that 
I am incorrectly using the exception.
*/