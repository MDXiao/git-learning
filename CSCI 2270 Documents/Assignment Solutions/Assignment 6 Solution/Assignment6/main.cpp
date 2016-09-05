#include <iostream>
#include "MovieTree.h"
#include <fstream>  //allows istream/ostream
#include <string>
#include <stdlib.h>

using namespace std;

struct Movie{
    string ranking;
    string title;
    string quantity;
    string releaseYear;
    Movie(){};

    Movie(string in_ranking, string in_title, string in_year, string in_quantity)
    {
        ranking = in_ranking;
        title = in_title;
        releaseYear = in_year;
        quantity = in_quantity;
    }
};

void displayMenu();
int getFileSize(char * fileName);
void readFileIntoTree(MovieTree * mt, char * fileName);

int main(int argc, char*argv[])
{
    int input;
    // Determine the size of the text file.
    //int fileSize = getFileSize(argv[1]);
    //cout << "about to create object\n";
    // Create a new communication network
    MovieTree *mt = new MovieTree();

    // Read each line and add it to tree
    readFileIntoTree(mt, argv[1]);

    // Flag used for exiting menu
    bool quit = false;

    // Used for input
    string title;

    while(quit != true)
    {
        displayMenu();
        cin >> input;

        //clear out cin
        cin.clear();
        cin.ignore(10000,'\n');

        switch (input)
        {
            case 1:
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->findMovie(title);
                break;
            // Rent a movie
            case 2:
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->rentMovie(title);

                break;
            // Print the inventory
            case 3:
                mt->printMovieInventory();
                break;

            // Delete Node
            /*
            case 3:
                cout << "Enter title:" << endl;
                getline(cin,title);
                mt->deleteMovieNode(title);
                break;
            // Count Tree
            case 4:
                cout << "Tree contains: " << mt->countMovieNodes() << " nodes." << endl;
                break;
            // Quit
            */
            case 4:
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

    // Free memory and return
    delete mt;

    return 0;
}

/*displays a menu with options to enqueue and dequeue a message and transmit the entire message and quit*/
void displayMenu()
{
    cout << "======Main Menu======" << endl;
    cout << "1. Find a movie" << endl;
    cout << "2. Rent a movie" << endl;
    cout << "3. Print the inventory" << endl;
    cout << "4. Quit" << endl;
    return;
}

/*grabs the number of words in a file */
int getFileSize(char * fileName)
{
    ifstream in_stream;
    in_stream.open(fileName);
    int count = 0;
    string word;

    while (!in_stream.eof())
    {
        getline(in_stream,word);
        count++;
    }

    in_stream.close();

    return count;
}

/* reads file into tree */
void readFileIntoTree(MovieTree * mt, char * fileName)
{
    ifstream in_stream;
    //cout << fileName << endl;
    in_stream.open(fileName);
    if (!in_stream)
    {
        cout << "Could not open file\n";
        return;
    }
    string ranking;
    string title;
    string releaseYear;
    string quantity;

    while (!in_stream.eof())
    {
        title ="";
        getline(in_stream, ranking, ',');
        getline(in_stream, title, ',');
        getline(in_stream, releaseYear, ',');
        getline(in_stream, quantity); // "\n" is the default delimiter
        if (title != "")
        {
            //cout << "Adding: " << title << endl;
            mt->addMovieNode(atoi(ranking.c_str()),title,atoi(releaseYear.c_str()),atoi(quantity.c_str()));
        }
    }
}

