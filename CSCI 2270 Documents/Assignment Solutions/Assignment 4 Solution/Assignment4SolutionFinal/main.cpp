// ==========================================
// Filename: Assignment3Solution.cpp
// Author: Matt Bubernak and Rhonda Hoenigman
// Date: 1/29/2015, 1/31/2016
// Description: Linked List Fun
// ==========================================

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "CommunicationNetwork.h"

using namespace std;

void displayMenu();
//void sendMessagesFromFile(city * sender, city * receiver);

// main expects 1 argument, filename
int main(int argc, char* argv[])
{
    // indicates the user is ready to leave the program
    bool quit = false;
    int input;
    CommunicationNetwork cn;

    // loop until the user quits
    while (quit != true)
    {
        displayMenu();
        cin >> input;

        // used for user input
        string cityNew;
        string cityPrevious;
        string message;
        string city1;

        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');

        switch (input)
        {
            // print all nodes
            case 1:
                //rebuild network
                cn.buildNetwork();
                cn.printNetwork();
                //head = loadDefaultSetup(head);
                //printPath(head);
                break;
            case 2:
                cn.printNetwork();
                //printPath(head);
                break;
            case 3:
                //transmit message is read in word from file
                //send first word through all cities
                //using transmitMsg function to pass between two cities
                //when message reaches the last city, read next word from file and repeat
                //all of this should happen in the transmitMsg function
                cn.transmitMsg(argv[1]);
                //transmitMsg(head);
                break;
            case 4:
                cout << "Enter a city name: " << endl;
                getline(cin,cityNew);
                cout << "Enter a previous city name: " << endl;
                getline(cin,cityPrevious);
                // add the new node
                cn.addCity(cityPrevious, cityNew);
                //head = addCity(head,tmp,city1, false);
                break;

            // delete city
            case 5:
                cout << "Enter a city name: " << endl;
                getline(cin,city1);
                cn.deleteCity(city1);
                break;

            // delete network
            case 6:

                cn.deleteNetwork();
                //cout << "Deleted network" << endl;
                break;
            // quit
            case 7:
                cout << "Goodbye!" << endl;
                quit = true;
                break;

            // invalid input
            default:
                cout << "Invalid Input" << endl;
                cin.clear();
                cin.ignore(10000,'\n');
                break;
        }
    }

    // cleanup list before return.
//    deleteEntireNetwork(head);
    return 0;

}

/* Add a new city to the network between the city *previous and the city that follows it
in the network. The name of the new city is in the argument cityName. Return the head of
the linked list */
// NOTE: If node is not found, this will return NULL.
/*This function deletes all cities in the network starting at the head city. The function
should return the hnext pointer of the last city deleted, which should be the tail of the
linked list. The next pointer should be NULL*/
/*
city *deleteEntireNetwork(city * head)
{
    // return NULL if empty list
    if (head == NULL)
        return NULL;

    // tmp is used for deleting nodes
    city * tmp;

    // go through every node
    while (head != NULL)
    {
        // store the node
        tmp = head;
        // move to next node
        head = head->next;
        // delete the stored node
        delete tmp;
    }
    // return head, which should be NULL
    return head;
}

/* pass the specified message between the sender and receiver cities. Call transmitMsg
each time you sedn the word to the next city. Transmitting the message means that only
one city can hold the message / word at one time. As soon as the message is transmitted
to the reciever city, the message should be removed from the sender city. */



/* delete the city in the network with the specified name. Return the head of the linked
list */
/*
city * deleteCity(city * head, string cityName)
{
    // locate the city we need to delete
    city * delCity = searchNetwork(head,cityName);

    // if the city dosn't exist, nothing we can do.
    if (delCity == NULL)
        cout << "City does not exist." << endl;

    // If the head is the one we need to delete
    else if (delCity == head)
    {
        head = head->next;
        delete delCity;
    }

    // If it's not the head, but exists, start searching.
    else
    {
        city * tmp = head;
        // loop until the node before the city we want to delete
        while (tmp->next != delCity)
        {
            tmp = tmp->next;
        }

        // move the pointer behind the city we want to delete to the next node
        tmp->next = tmp->next->next;

        // delete the city
        delete delCity;
    }

    return head;
}
*/




/*displays a menu with options: add city, delete city, transmit coast-coast,
clear network, quit*/
void displayMenu()
{
    cout << "======Main Menu======" << endl;
    cout << "1. Build Network" << endl;
    cout << "2. Print Network Path" << endl;
    cout << "3. Transmit Message Coast-To-Coast-To-Coast" << endl;
    cout << "4. Add City" << endl;
    cout << "5. Delete City" << endl;
    cout << "6. Clear Network" << endl;
    cout << "7. Quit" << endl;
    return;
}




