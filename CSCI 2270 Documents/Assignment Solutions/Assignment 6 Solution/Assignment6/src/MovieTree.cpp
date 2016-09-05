#include <iostream>
#include "MovieTree.h"
using namespace std;

MovieTree::MovieTree()
{
    root = NULL;
}

MovieTree::~MovieTree()
{
   // DeleteAll(root);
}

/* Used to delete all nodes in the tree */
/*
void MovieTree::DeleteAll(MovieNode * node)
{
    // clean to the left
    if (node->leftChild != NULL)
        DeleteAll(node->leftChild);
    // clean to the right
    if (node->rightChild != NULL)
        DeleteAll(node->rightChild);
    // delete this node
    delete node;

    return;
}
*/
/* Helper for the printMovieInventory recursive function */
void MovieTree::printMovieInventory()
{
    printMovieInventory(root);
    return;
}

/* Prints the inventory(in order traversal) */
void MovieTree::printMovieInventory(MovieNode * node)
{
    // Left Node
    if(node->leftChild!=NULL)
        printMovieInventory(node->leftChild);

    // Value
    cout<<"Movie: "<<node->title<<" "<<node->quantity<<endl;

    // Right Node
    if(node->rightChild!=NULL)
        printMovieInventory(node->rightChild);
}



void MovieTree::addMovieNode(int ranking, std::string title, int releaseYear, int quantity)
{

    MovieNode * newMovie = new MovieNode(ranking,title,releaseYear,quantity);
    MovieNode * x = root;
    MovieNode * y = NULL;

    // Do we have an empty tree?
    if (root == NULL)
        root = newMovie;

    // If the tree is not empty
    else
    {
        // Get to the end of the tree, where we need to add this node.
        while (x != NULL)
        {
            y = x;
            if(newMovie->title.compare(x->title) < 0)
                x = x->leftChild;
            else
                x = x->rightChild;
        }
        // set the parent of this node to be the previous node.
        newMovie->parent = y;

        // Determine which child to this previous node we are at.
        if (newMovie->title.compare(y->title) < 0)
            y->leftChild = newMovie;
        else
            y->rightChild = newMovie;
    }

}
/* used to find movie information, print movie information */
void MovieTree::findMovie(std::string title)
{
    MovieNode *foundMovie = root;
    bool found = false;
    while(foundMovie != NULL && !found){
        if(foundMovie->title < title){
            foundMovie = foundMovie->rightChild;
        }else if(foundMovie->title > title){
            foundMovie = foundMovie->leftChild;
        }else{
            found = true;
        }
    }
    if(foundMovie != NULL){
        cout << "Movie Info:" << endl;
        cout << "===========" << endl;
        cout << "Ranking:" << foundMovie->ranking << endl;
        cout << "Title:" << foundMovie->title << endl;
        cout << "Year:" << foundMovie->year << endl;
        cout << "Quantity:" << foundMovie->quantity << endl;
    }
    else{
        cout << "Movie not found." << endl;
    }

}

/* used to find movie information, print movie information */
MovieNode* MovieTree::search(std::string title)
{
    MovieNode *foundMovie = root;
    bool found = false;
    while(foundMovie != NULL && !found){
        if(foundMovie->title < title){
            foundMovie = foundMovie->rightChild;
        }else if(foundMovie->title > title){
            foundMovie = foundMovie->leftChild;
        }else{
            found = true;
        }
    }
    return foundMovie;
}

void MovieTree::rentMovie(std::string title)
{
    MovieNode * foundMovie = search(title);
    // If the movie exists.
    if (foundMovie != NULL)
    {
        // If it's in stock.
        if (foundMovie->quantity > 0)
        {
            cout << "Movie has been rented." << endl;
            foundMovie->quantity--;
            //change this to print information
            cout << "Movie Info:" << endl;
            cout << "===========" << endl;
            cout << "Ranking:" << foundMovie->ranking << endl;
            cout << "Title:" << foundMovie->title << endl;
            cout << "Year:" << foundMovie->year << endl;
            cout << "Quantity:" << foundMovie->quantity << endl;

        }
        // If it's not in stock.
        else
            cout << "Movie out of stock." << endl;

    }
    // If it doesn't exist.
    else
        cout << "Movie not found." << endl;
}
