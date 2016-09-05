#include "Queue.h"
#include <iostream>
#include "Queue.h"

using namespace std;

Queue::Queue(int qs)
{
    queueSize = qs;
    arrayQueue = new string[queueSize];
    queueHead = 0;
    queueTail = 0;
    queueCount = 0;
}

Queue::~Queue()
{
    //dtor
    delete []arrayQueue;
}

void Queue::enqueue(string word){
    arrayQueue[queueTail] = word;
    if(queueTail == queueSize-1){
        queueTail = 0;
    }else{
        queueTail++;
    }
    cout<<"E: "<<word<<endl;
    cout<<"H: "<<queueHead<<endl;
    cout<<"T: "<<queueTail<<endl;
    queueCount++;
}

string Queue::dequeue(){
    string word = arrayQueue[queueHead];
    if(queueHead == queueSize-1){
        queueHead = 0;
    }else{
        queueHead++;
    }

    cout<<"H: "<<queueHead<<endl;
    cout<<"T: "<<queueTail<<endl;
    queueCount--;
    return word;
}

/* Prints out the current queue */
void Queue::printQueue(){
    int current = queueHead;
    int count = queueCount;
    if (queueCount > 0)
    {
        // Print out each elemenent
        while(count > 0)
        {
            cout<<current<<": "<<arrayQueue[current]<<endl;
            count--;
            current++;
            // If we hit the end of the queue, move back to front
            if (current == queueSize)
                current = 0;
        }
    }
    else
        cout << "Empty" << endl;
}

/* Returns true if the queue is full, false otherwise */
bool Queue::queueIsFull(){
    if (queueCount == queueSize)
        return true;
    return false;
}

/* Returns true if the queue is full, false otherwise */
bool Queue::queueIsEmpty(){
    if (queueCount == 0)
        return true;
    return false;
}
