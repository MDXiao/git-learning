#include <iostream>
#include "Queue.h"
#include <fstream>  //allows istream/ostream
#include <sstream>
using namespace std;
void displayMenu();
//int getFileSize(char * fileName);
//void readFileIntoArray(string arr[], char * fileName);

int main(int argc, char*argv[])
{
    int input;

    Queue *q = new Queue(10);

    bool quit = false;
    string word;

    while(quit != true){
        displayMenu();
        cin >> input;

        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');
        switch (input)
        {
            case 1:{
                //enqueue one word, need to get word from user
                cout<<"word: ";
                getline(cin, word);
                if (q->queueIsFull() != true)
                {
                    q->enqueue(word);
                }
                else
                    cout << "Queue is full." << endl;
                break;
            }case 2:{
                //dequeue word and print it
                if (q->queueIsEmpty() != true)
                {
                    word = q->dequeue();
                    cout<<"word: "<<word<<endl;
                }
                else
                    cout << "Queue is empty." << endl;
                break;
            }case 3:{
                //print contents from head to tail
                q->printQueue();
                break;
            }case 4:{
                //read an entire sentence from user. Add individual
                //words to queue until queue full.
                //Throw out words that don't fit
                string sentence;
                cout<<"sentence: ";
                getline(cin, sentence);
                stringstream ss(sentence);
                while(getline(ss, word, ' ')){
                    if(q->queueIsFull() != true){
                        q->enqueue(word);
                    }
                    else{
                        cout<<"Queue is full"<<endl;
                    }
                }
                break;
            }case 5:{
                //quit
                cout << "Goodbye!" << endl;
                quit = true;
                break;
            // invalid input
            }
            default:{
                cout << "Invalid Input" << endl;
                cin.clear();
                cin.ignore(10000,'\n');
                break;
            }
        }
    }

    // Free memory we allocated for message array, and communication network
    delete q;
//    delete [] message;
    return 0;
}

/*displays a menu with options to enqueue and dequeue a message and transmit the entire message and quit*/
void displayMenu()
{
    cout << "======Main Menu=====" << endl;
    cout << "1. Enqueue word" << endl;
    cout << "2. Dequeue word" << endl;
    cout << "3. Print queue" << endl;
    cout << "4. Enqueue sentence" << endl;
    cout << "5. Quit" << endl;
    return;
}

