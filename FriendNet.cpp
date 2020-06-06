
#include "FriendNet.h"


#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "Queue.h"

using namespace std;

static string arrayToString( Friend ** friends, const int friendAmt ) {

    string result;
    for( int i = 0; i < friendAmt; i++ ) {
        if( i != friendAmt - 1 ) {
            result = result + friends[i]->getName() + ", ";
        } else {
            result = result + friends[i]->getName() + "\n";
        }
    }

    return result;

}

FriendNet::FriendNet(const string fileName) {
    isEmpty = !parseFile(fileName);
}

FriendNet::FriendNet( const FriendNet& aNet ) {

    isEmpty = aNet.isEmpty;

    if( isEmpty ) {
        return;
    }

    this->peopleAmt = aNet.peopleAmt;
    this->people = new Friend*[peopleAmt];
    this->nodes = new AdjNode*[peopleAmt];


    //deep copy both "people" and "nodes"
    for( int i = 0; i < peopleAmt; i++ ) {
        //create a new copy of friends
        Friend * copy = new Friend( aNet.people[i]->getName(),
                                    aNet.people[i]->getId(),
                                    aNet.people[i]->getFriendAmount() );

        this->people[i] = copy;

    }

    //after the initialization of the friends array,
    //copy adjacency lists of each friend
    for( int i = 0; i < peopleAmt; i++ ) {

        AdjNode * curr = aNet.nodes[i];

        //copy first element of the list
        this->nodes[i] = new AdjNode(people[curr->item->getId()] );

        AdjNode * currCopy = this->nodes[i];

        //link other elements to the list
        while( curr != NULL ) {

            // last pointer points to NULL
            if( curr->next == NULL ) {
                currCopy->next = NULL;
                break;
            }

            currCopy->next = new AdjNode(this->people[curr->next->item->getId()]);
            currCopy = currCopy->next;
            curr = curr->next;
        }
    }
}

FriendNet::~FriendNet() {

    if( isEmpty ) {
        return;
    }

    //delete people
    for( int i = 0; i < peopleAmt; i++ ) {
        Friend * curr = people[i];
        delete curr;
    }

    delete [] people;

    //delete nodes
    for( int i = 0; i < peopleAmt; i++ ) {
        AdjNode * curr = nodes[i];
        while( curr != NULL ){
            AdjNode * temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    delete [] nodes;

}


static int stringToInt(string s) {

    stringstream convert(s);
    int number = 0;
    convert >> number;
    return number;
}


//returns true if the file exists and false if it does not exist
bool FriendNet::parseFile(string fileName) {

    ifstream inFile;
    inFile.open( fileName.c_str() );

    if( !inFile.good() ) {
        cout << "File does not exist. " << endl;
        return false;
    }

    string line;
    int line_count = 0;
    int ** friends; //To temporarily store friends of each person.

    while( getline(inFile, line) ) {

        stringstream ss(line);
        string value;

        int word_count = 0;

        int ID;
        string name;
        int numOfFriends;

        int arrayIndex = 0;

        while( getline( ss, value, ' ' ) ) {

            if( value == "" ) {
                continue;
            }

            if( line_count == 0 ) {
                peopleAmt = stringToInt(value);
                people = new Friend*[peopleAmt]; //create friend array
                friends = new int*[peopleAmt]; //create each row of size peopleAmt
                continue;
            }

            if( word_count == 0 ) {
                ID = stringToInt(value);
            } else if( word_count == 1 ) {
                name = value;
            } else if( word_count == 2 ) {
                numOfFriends = stringToInt(value);
                friends[line_count - 1] = new int[numOfFriends]; //create column for each person
            } else {
                friends[line_count - 1][arrayIndex] = stringToInt(value); //initialize each friend
                arrayIndex++;
            }

            word_count++;
        }

        if( line_count != 0 ) {
            Friend * theFriend = new Friend( name, ID, numOfFriends );
            people[line_count - 1] = theFriend; //assign all to people array
        }
        line_count++;

    }


    nodes = new AdjNode*[peopleAmt];
    //link the nodes with each other
    for( int i = 0; i < peopleAmt; i++ ) {
        AdjNode * currNode;
        nodes[i] = new AdjNode(people[i]);
        currNode = nodes[i];
        int friendAmt = people[i]->getFriendAmount();

        if( friendAmt == 0 ) { //if no friend, set next to NULL
            currNode->next = NULL;
        }

        for( int j = 0; j < friendAmt; j++ ) {
            currNode->next = new AdjNode( people[ friends[i][j] ] ); //setup nodes' next node
            currNode = currNode->next;

            if( j == friendAmt - 1 ) {
                currNode->next = NULL; //last->next is NULL
            }
        }
    }

    //clean up
    for( int i = 0; i < peopleAmt; i++ ) {
        delete [] friends[i];
    }
    delete [] friends;

    return true;

}

//returns the depth and friends of start
string FriendNet::iterativeBFT( Friend * start, const int hopNo, int & depth ) {

    if( hopNo <= 0 ) {
        return "";
    }

    Friend * friends[peopleAmt];
    int total = 0;
    Queue q;
    q.enqueue(nodes[start->getId()]);
    q.enqueue(NULL); //to keep track of the hops
    start->setAsVisited();
    depth = 0;

    AdjNode * curr;
    while( !q.isEmpty() && depth != hopNo ) {
        q.dequeue(curr);

        if( curr == NULL ) { //increase depth(hops)
            depth++;
            q.enqueue(NULL);
            AdjNode * top;
            q.getFront(top);
            if( top == NULL ) { //if two NULLs, terminate
                break;
            } else {
                continue;
            }
        }

        curr = nodes[curr->item->getId()];
        int friendAmt = curr->item->getFriendAmount();
        for( int i = 0; i < friendAmt; i++ ) {
            curr = curr->next;
            if( !curr->item->isVisited() ) {
                curr->item->setAsVisited();
                q.enqueue(curr);

                bool exist = false;
                for( int i = 0; i < total; i++ ) {
                    if( friends[i] == curr->item ) {
                        exist = true;
                    }
                }

                if(!exist) {
                    friends[total] = curr->getItem();
                    total++;
                }
            }
        }
    }

    //set friends to unvisited again
    for(int i = 0; i < peopleAmt; i++) {
        people[i]->visited = false;
    }

    return arrayToString(friends, total);

}

//list friends within hopNo by calling helper function iterativeBFT
void FriendNet::listFriends(const string personName, const int hopNo) {

    int personNo = -1;
    for( int i = 0; i < peopleAmt; i++ ) {
        if( personName == people[i]->getName() ) {
            personNo = i;
        }
    }

    if( personNo == -1 ) {
        cout << personName << " does not exist in the network. " << endl;
        return;
    }


    Friend * curr = people[personNo];
    cout << "People accessible from " << personName << " within " << hopNo << " hops: ";
    string accessiblePeople = "";

    int depth; //not used

    accessiblePeople = iterativeBFT(curr, hopNo, depth);

    if( accessiblePeople == "" ) {
        cout << "NOBODY" << endl;
    } else {
        cout << accessiblePeople << endl;
    }

}


// find disconnected sets using Breadth First Search and
// return it with dynamically allocated double array set, size of the set, sizes of
// each subset(each connected component) as int array as reference
// need to remove the set and subsetSizes using delete after calling the method!
void FriendNet::findDisconnectedSets( int & setSize, Friend ***& set, int *& subsetSizes ) {
    setSize = 0;
    int total;
    Queue q;
    set = new Friend**[peopleAmt];
    subsetSizes = new int[peopleAmt];

    for( int i = 0; i < peopleAmt; i++ ) {
        total = 0;
        Friend *friends[peopleAmt];
        if( !people[i]->isVisited() ) {
            q.enqueue(nodes[people[i]->getId()]);
            friends[total] = people[i];
            total++;
            people[i]->setAsVisited();

            AdjNode * curr;
            while( !q.isEmpty() ) {
                q.dequeue(curr);

                curr = nodes[curr->item->getId()];
                int friendAmt = curr->item->getFriendAmount();
                for( int j = 0; j < friendAmt; j++ ) {
                    curr = curr->next;
                    if( !curr->item->isVisited() ) {
                        curr->item->setAsVisited();
                        q.enqueue(curr);

                        bool exist = false;
                        for( int k = 0; k < total; k++ ) {
                            if( friends[k] == curr->item ) {
                                exist = true;
                            }
                        }

                        if(!exist) {
                            friends[total] = curr->getItem();
                            total++;
                        }
                    }
                }
            }

            set[setSize] = new Friend*[total];

            // deep copy the disconnected friend set
            for( int p = 0; p < total; p++ ) {
                set[setSize][p] = friends[p];
            }

            // save size of each subset( each connected component )
            subsetSizes[setSize] = total;
            setSize++;
        }

    }

    //set friends as unvisited again
    for(int i = 0; i < peopleAmt; i++) {
        people[i]->visited = false;
    }

}

// returns the degrees of each set
// delete the degrees array after calling this function to avoid memory leak
void FriendNet::getAverageDegrees( Friend *** set, const int * subsetSizes,
        double *& degrees, const int setSize ) {

    degrees = new double[setSize]; //degree for each set
    double currDegree;

    for( int i = 0; i < setSize; i++ ) {
        currDegree = 0;
        for( int j = 0; j < subsetSizes[i]; j++ ) {
            currDegree += set[i][j]->getFriendAmount();
        }

        degrees[i] = currDegree / subsetSizes[i]; //save avg degree of each set
    }

}

void FriendNet::displayAverageDegrees() {


    int setSize;
    Friend *** set;
    int * subsetSizes;
    double * degrees;

    //get all separate(disconnected) components
    findDisconnectedSets( setSize, set, subsetSizes );

    cout << "There are " << setSize << " connected components. The average degrees are: " << endl;

    //get average degrees for those individual components
    getAverageDegrees( set, subsetSizes, degrees, setSize );

    //print the degrees of each set
    for( int i = 0; i < setSize; i++ ) {
        cout << "For component " << i << ": " << degrees[i] << endl;
    }


    //clean up
    for( int i = 0; i < setSize; i++ ) {
        delete set[i];
    }

    delete [] subsetSizes;
    delete [] set;
    delete [] degrees;


}


void FriendNet::displayDiameters() {

    int setSize;
    Friend *** set;
    int * subsetSizes;

    //get all separate(disconnected) components
    findDisconnectedSets( setSize, set, subsetSizes );

    cout << "There are " << setSize << " connected components. The diameters are: " << endl;

    int diameter;
    int maxDiameter;

    //find max diameter of each set
    for( int i = 0; i < setSize; i++ ) {
        maxDiameter = 0;
        for( int j = 0; j < subsetSizes[i]; j++ ) {
            //find diameter for each friend in the set and then find the max diameter
            iterativeBFT(set[i][j], peopleAmt, diameter);
            diameter = diameter - 1; //diameter is noOfHops - 1
            if( diameter > maxDiameter ) {
                maxDiameter = diameter;
            }
        }
        cout << "For component " << i << ": " << maxDiameter << endl;
    }



    //clean up
    for( int i = 0; i < setSize; i++ ) {
        delete set[i];
    }

    delete [] subsetSizes;
    delete [] set;

}
