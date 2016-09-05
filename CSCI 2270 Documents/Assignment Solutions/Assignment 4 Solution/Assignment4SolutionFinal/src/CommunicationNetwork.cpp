#include "CommunicationNetwork.h"
#include <iostream>
#include <fstream>

using namespace std;

CommunicationNetwork::CommunicationNetwork()
{
    head = NULL;
    tail = NULL;
}

CommunicationNetwork::~CommunicationNetwork()
{
    //dtor
    deleteNetwork();
}
void CommunicationNetwork::addCity(string previousCity, string cityName){
    //add a new node to the network after the previousCity
    // if we are passed an empty list, just create a new head node, and return
    if(previousCity == "First")
    {
        //case where it's a new head city
        City *c = new City(cityName, head, NULL, "");
        head = c;
    }else{
        //create a tmp pointer for moving through list
        City * tmp = head;

        // move until we land on "previous" , or the next node is NULL
        while(tmp->cityName != previousCity && tmp->next != NULL)
        {
            tmp = tmp->next;
        }

        // create a new node, with next equal to city after "previous"
        City * newCity = new City(cityName, tmp->next, tmp, "");

        // point tmp to this new node we inserted.
        tmp->next = newCity;
    }


}
void CommunicationNetwork::deleteCity(string cityNameIn){
    //find city and delete it
    City *delCity = NULL;
    City *searchCity = head;
    bool found = false;
    while(!found and searchCity != NULL){
        if(searchCity->cityName == cityNameIn){
            found = true;
        }else{
            searchCity = searchCity->next;
        }

    }
    if(found == true){
        if(searchCity == head){
            delCity = head;
            head = head->next;
            head->previous = NULL;
            delete delCity;
        }else{
            searchCity->previous->next = searchCity->next;
            searchCity->next->previous = searchCity->previous;
            delete searchCity;
        }
    }else{
        cout<<cityNameIn<<"not found"<<endl;
    }

}
void CommunicationNetwork::deleteNetwork(){
//delete entire network
    // tmp is used for deleting nodes
    City * tmp;

    // go through every node
    while (head != NULL)
    {
        // store the node
        tmp = head;
        // move to next node
        head = head->next;
        // delete the stored node
        cout<<"deleting "<<tmp->cityName<<endl;
        delete tmp;
    }
}

void CommunicationNetwork::buildNetwork(){
//build the network with a fixed list of cities.
//can hard code cities in this function and add them in order
    string cityArray[10] = {"Los Angeles", "Phoenix", "Denver", "Dallas", "St. Louis", "Chicago", "Atlanta", "Washington, D.C.", "New York", "Boston"};
    int numCities = 10;
    City *cityAdded;
    City *currentCity;
    for(int i = 0; i < numCities; i++){
        if(head == NULL){
            cityAdded = new City(cityArray[i], NULL, NULL, "");
            currentCity = cityAdded;
            head = currentCity;
        }else{
            cityAdded = new City(cityArray[i], NULL, currentCity, "");
            currentCity->next = cityAdded;
            currentCity = cityAdded;

        }
    }
    tail = currentCity;
}
void CommunicationNetwork::transmitMsg(char *filename){
//filename is argument to function
//start at head of list and set message for first node to word read from file
    ifstream inStream; // stream for reading in file
    inStream.open(filename);
    string msg;
    if(head == NULL){
        cout<<"Empty list"<<endl;
        return;
    }
    City *sender = head;
    // read through every line , transmit message
    while (inStream>>msg)
    {
        sender->message = msg;
        cout<<sender->cityName<<" received "<<sender->message<<endl;
        while(sender->next != NULL){
            sender->next->message = msg;
            cout<<sender->next->cityName<<" received "<<sender->next->message<<endl;
            sender->message = "";
            sender = sender->next;
        }
        //Rhonda: back the other direction starting here
        sender = tail;

        while(sender->previous != NULL){
            sender->previous->message = msg;
            cout<<sender->previous->cityName<<" received "<<sender->previous->message<<endl;
            sender->message="";
            sender = sender->previous;
        }

        sender = head;
    }
    // close the file
    inStream.close();

}
void CommunicationNetwork::printNetwork(){
//traverse network, printing name of each city
/*prints the current list nicely*/
    cout << "===CURRENT PATH===" << endl;

    // If the head is NULL
    if (head == NULL)
        cout << "NULL" << endl;

    // Otherwise print each node, and then a NULL
    else
    {
        City *current = head;

        cout<<"NULL <- ";

        while (current->next != NULL)
        {
            cout << current->cityName << " <-> ";
            current = current->next;
        }
        cout << current->cityName << " -> ";
        cout << "NULL" << endl;
    }

    cout << "==================" << endl;

}

