#include <iostream>
#include "HashTable.h"
#include <string.h>
#include <vector>

using namespace std;

HashTable::HashTable()
{

    for(int i = 0; i < 10; i++){
        hashTable[i] = NULL;
    }
}

HashTable::~HashTable()
{
/*
	for (int i = 0; i < tableSize; i++)
	{
		if (hashTable[i] != NULL)
		{
			delete hashTable[i];
		}
	}
*/
}


void HashTable::printTableContents()
{
	bool empty = true;

	for (int i = 0; i < tableSize; i++)
	{
		if (hashTable[i] != NULL)
		{
            empty = false;
            HashElem *temp = hashTable[i];
            while (temp != NULL){
                cout<<temp->title<<":"<<temp->year<<endl;
                temp = temp->next;
            }
		}
	}
	if (empty == true)
		cout << "empty" << endl;
	return;
}

void HashTable::insertMovie(string name, int year)
{
	int index = hashSum(name,tableSize);
	// If there is nothing in this location.
	if (hashTable[index] == NULL)
	{
        HashElem *temp = new HashElem(name, year);
		hashTable[index] = temp;
	}

	// If we need to add to a chain.
	//bug in this code
	else
	{
        HashElem *temp = hashTable[index];
        HashElem *previous = hashTable[index];
        while(temp != NULL){
			if (temp->title == name){
				cout << "duplicate" << endl;
				return;
			}else{
			    previous = temp;
                temp = temp->next;
			}
		}
		//fix last element is duplicate
        HashElem *newMovie = new HashElem(name, year);
        previous->next = newMovie;
        newMovie->previous = previous;

	}

	return;
}

void HashTable::deleteMovie(string name)
{
	int index = hashSum(name,tableSize);
	bool found = false;

	// If there is a node at this hash location.
	if (hashTable[index] != NULL)
	{
		// Loop through linked list elements
        HashElem *temp = hashTable[index];
        while(temp != NULL)
		{
			// If we find the movie, delete it.
			if (temp->title == name)
			{
                if(temp == hashTable[index]){
                    hashTable[index] = temp->next;
                }
                if(temp->next != NULL)
                    temp->next->previous = temp->previous;
                if(temp->previous != NULL)
                    temp->previous->next = temp->next;

                delete temp;
				temp = NULL;
				found = true;

				break;
			}
			temp = temp->next;
		}
	}
	if (found == false)
	{
		cout << "not found" << endl;
	}
	return;
}

void HashTable::findMovie(string name)
{
	int index = hashSum(name,tableSize);
	bool found = false;

	// If there is a node at this hash location.
	if (hashTable[index] != NULL)
	{
		// Loop through every vector index at this hash location.
		HashElem *temp = hashTable[index];
		while(temp != NULL)//for (int i = 0; i < hashTable[index]->size(); i++)
		{
			// If we find the movie in the vector, delete it.
			if (temp->title == name)
			{
				cout << index << ":" << temp->title << ":" << temp->year << endl;
				found = true;
				break;
			}
			temp = temp->next;
		}
	}
	if (found == false)
	{
		cout << "not found" << endl;
	}
	return;
}

// Returns hashSum of input string x
int HashTable::hashSum(string inputString, int hashLen)
{
	int sum = 0;
    for (int i = 0; i < inputString.length(); i++)
    {
    	sum = sum + inputString[i];  //ascii value of ith character in the string
    }
    sum = sum % hashLen;
    return sum;
}
