
#ifndef CS202_HW5_QUEUE_H
#define CS202_HW5_QUEUE_H

#include "FriendNet.h"

typedef AdjNode* QueueItemType;

class Queue{
public:
    Queue();                     // default constructor
    Queue(const Queue& Q);       // copy constructor
    ~Queue();                    // destructor

    // Queue operations:
    bool isEmpty() const;
    bool enqueue(QueueItemType newItem);
    bool dequeue();
    bool dequeue(QueueItemType& queueFront);
    bool getFront(QueueItemType& queueFront) const;

private:
    // The queue is implemented as a linked list with one external
    // pointer to the front of the queue and a second external
    // pointer to the back of the queue.
    struct QueueNode{
        QueueItemType  item;
        QueueNode     *next;
    };
    QueueNode *backPtr;
    QueueNode *frontPtr;
};

#endif //CS202_HW5_QUEUE_H
