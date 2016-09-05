// ==========================================
// Filename: Assignment3Solution.cpp
// Author: Matt Bubernak
// Date: 1/29/2015
// Description: Linked List Fun
// ==========================================

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

// struct to store city.
struct city{
    string name; // name of the city
    city * next; // pointer to the next city
    string message; // message we are sending accross

    city(){}; // default constructor

    city(string init_name, city * init_next, string init_message)
    {
        name = init_name;
        next = init_next;
        message = init_message;
    }
};

city * addCity(city * head, city * previous, string cityName, bool initBuild);
city * deleteCity(city * head, string cityName);
city *deleteEntireNetwork(city * head);
city *searchNetwork(city * head, string cityName);
city * loadDefaultSetup(city * head);

void transmitMsg(city * head);
void printPath(city * head);
void displayMenu();
//void sendMessagesFromFile(city * sender, city * receiver);

// main expects 1 argument, filename
int main(int argc, char* argv[])
{
    // indicates the user is ready to leave the program
    bool quit = false;
    int input;

    // pointer to the head of our network of cities
    city * head = NULL;

    // used for temporary pointer storage
    city * tmp = NULL;
    city * tmp2 = NULL;

    // loop until the user quits
    while (quit != true)
    {
        displayMenu();
        cin >> input;

        // used for user input
        string city1;
        string city2;
        string message;

        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');

        switch (input)
        {
            // print all nodes
            case 1:
                //rebuild network
                head = loadDefaultSetup(head);
                printPath(head);
                break;
            case 2:
                printPath(head);
                break;
            case 3:
                //transmit message is read in word from file
                //send first word through all cities
                //using transmitMsg function to pass between two cities
                //when message reaches the last city, read next word from file and repeat
                //all of this should happen in the transmitMsg function
                transmitMsg(head);
                break;
            case 4:
                cout << "Enter a city name: " << endl;
                getline(cin,city1);
                cout << "Enter a previous city name: " << endl;
                getline(cin,city2);
                // find the node containing prevCity
                if(city2 !="First")
                    tmp = searchNetwork(head,city2);
                else
                    city *tmp = NULL;
                // add the new node
                head = addCity(head,tmp,city1, false);
                break;
            // delete city
            case 5:
                //may want to print city list
                cout << "Enter a city name: " << endl;
                getline(cin,city1);
                head = deleteCity(head,city1);
                break;
            // transmit a message
            // delete network
            case 6:
                head = deleteEntireNetwork(head);
                cout << "Deleted network" << endl;
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
    deleteEntireNetwork(head);
    return 0;

}

/* Add a new city to the network between the city *previous and the city that follows it
in the network. The name of the new city is in the argument cityName. Return the head of
the linked list */
city * addCity(city * head, city * previous, string cityName, bool initialBuild)
{
    // if we are passed an empty list, just create a new head node, and return
    if (head == NULL)
        head = new city(cityName,NULL,"");

    // if it's not empty, we need to search for previous and append our node there.
    else if(previous == NULL && initialBuild == false)
    {
        //case where it's a new head city
        city *c = new city(cityName, head, "");
        head = c;
    }else{
        //create a tmp pointer for moving through list
        city * tmp = head;

        // move until we land on "previous" , or the next node is NULL
        while(tmp != previous && tmp->next != NULL)
        {
            tmp = tmp->next;
        }

        // create a new node, with next equal to city after "previous"
        city * newCity = new city(cityName, tmp->next, "");

        // point tmp to this new node we inserted.
        tmp->next = newCity;
    }

    // return the head of the list
    return head;
}


/*Search the network for the specified city and return a pointer to that node in the
network, this fucntion is called in the deleteCity and addCity functions*/
// NOTE: If node is not found, this will return NULL.
city *searchNetwork(city * head, string cityName)
{
    // Search until the head is NULL, or we find the city
    while (head != NULL && head->name != cityName)
    {
        head = head->next;
    }
    // Return the node, or NULL
    return head;
}

/*This function deletes all cities in the network starting at the head city. The function
should return the hnext pointer of the last city deleted, which should be the tail of the
linked list. The next pointer should be NULL*/
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
/*
void transmitMsg(city * sender, city * receiver, string message)
{
    // check if either sender or reciever are null
    if (sender == NULL || receiver == NULL)
        cout << "Message failed to send, one or more cities = NULL";

    // send the message on!
    else
    {
        // send message on if the next node is not NULL
        if (sender->next != NULL)
        {
            sender->next->message = message;
            sender->message = "";
            //transmitMsg(sender->next, receiver, message);
        }

        // otherwise note that message can not be delivered
        else
            cout << "Message sent from "<< sender->name <<" but never recieved" << endl;
    }

    return;

}
*/
/* sends messages from file, word by word */
void transmitMsg(city * head)
{
    ifstream inStream; // stream for reading in file
    inStream.open("messageIn.txt");
    string msg;
    if(head == NULL){
        cout<<"Empty list"<<endl;
        return;
    }
    city *sender = head;
    // read through every line , transmit message
    while (inStream>>msg)//!inStream.eof())
    {
        //inStream >> msg;
        sender->message = msg;
        cout<<sender->name<<" received "<<sender->message<<endl;
        while(sender->next != NULL){
            sender->next->message = msg;
            cout<<sender->next->name<<" received "<<sender->next->message<<endl;
            sender->message = "";
            sender = sender->next;
        }
        sender = head;
    }
    // close the file
    inStream.close();

    return;
}


/* delete the city in the network with the specified name. Return the head of the linked
list */
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


/*prints the current list nicely*/
void printPath(city * head)
{
    cout << "===CURRENT PATH===" << endl;

    // If the head is NULL
    if (head == NULL)
        cout << "NULL" << endl;

    // Otherwise print each node, and then a NULL
    else
    {
        while (head->next != NULL)
        {
            cout << head->name << " -> ";
            head = head->next;
        }
        cout << head->name << " -> ";
        cout << " NULL" << endl;
    }

    cout << "=================" << endl;
    return;
}

/* populates the network with the predetermined cities */
city * loadDefaultSetup(city * head)
{
    head = deleteEntireNetwork(head);
    head = addCity(head,NULL,"Los Angeles", true);
    head = addCity(head,NULL,"Phoenix", true);
    head = addCity(head,NULL,"Denver", true);
    head = addCity(head,NULL,"Dallas", true);
    head = addCity(head,NULL,"St. Louis", true);
    head = addCity(head,NULL,"Chicago", true);
    head = addCity(head,NULL,"Atlanta", true);
    head = addCity(head,NULL,"Washington, D.C.", true);
    head = addCity(head,NULL,"New York", true);
    head = addCity(head,NULL,"Boston", true);
    return head;
}


/*displays a menu with options: add city, delete city, transmit coast-coast,
clear network, quit*/
void displayMenu()
{
    cout << "======Main Menu=====" << endl;
    cout << "1. Build Network" << endl;
    cout << "2. Print Network Path" << endl;
    cout << "3. Transmit Message Coast-To-Coast" << endl;
    cout << "4. Add City" << endl;
    cout << "5. Delete City" << endl;
    cout << "6. Clear Network" << endl;
    cout << "7. Quit" << endl;
    return;
}




