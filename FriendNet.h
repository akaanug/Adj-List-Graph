
#ifndef CS202_HW5_FRIENDNET_H
#define CS202_HW5_FRIENDNET_H

#include <string>
using namespace std;

class Friend;

class AdjNode { //carries pointer to the person and its friends
public:
    AdjNode( Friend * item ) {
        this->item = item;
    }

    Friend * getItem() {
        return item;
    }

private:
    Friend * item;
    AdjNode * next;
    friend class FriendNet;

};

class Friend{
public:
    Friend( string name, int ID, int friendAmount ) {
        this->name = name;
        this->ID = ID;
        this->friendAmount = friendAmount;
        this->visited = false;
    }

    const string &getName() const {
        return name;
    }

    int getId() const {
        return ID;
    }

    int getFriendAmount() {
        return friendAmount;
    }

    bool isVisited() {
        return visited;
    }

    void setAsVisited() {
        visited = true;
    }

private:
    int ID;
    string name;
    int friendAmount;
    bool visited;
    friend class FriendNet;

};

class FriendNet{
public:
    FriendNet(const string fileName);
    FriendNet(const FriendNet& aNet);
    ~FriendNet();
    bool parseFile( string fileName );

    void listFriends(const string personName, const int hopNo);
    void displayAverageDegrees();
    void displayDiameters();
    string iterativeBFT( Friend * start, const int hopNo, int & depth );
    void findDisconnectedSets( int & setSize, Friend ***& set, int *& subsetSizes );
    void getAverageDegrees( Friend *** set, const int * subsetSizes, double *& degrees, const int setSize );

private:
    int peopleAmt;
    Friend ** people;
    AdjNode ** nodes;
    bool isEmpty;

};

#endif //CS202_HW5_FRIENDNET_H
