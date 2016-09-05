#include <iostream>
#include "MovieTree.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

MovieTree::MovieTree()
{
    root = NULL;
}

MovieTree::~MovieTree()
{
    DeleteAll(root);
}

/* Used to delete all nodes in the tree */
void MovieTree::DeleteAll(MovieNode * node)
{
    // clean to the left
    if (node->leftChild != NULL)
        DeleteAll(node->leftChild);
    // clean to the right
    if (node->rightChild != NULL)
        DeleteAll(node->rightChild);
    // delete this node
    cout<<"Deleting: "<<node->title<<endl;
    delete node;

}


/* Helper for the printMovieInventory recursive function */
void MovieTree::printMovieInventory()
{
    printMovieInventory(root);

}

/* Prints the inventory(in order traversal) */
void MovieTree::printMovieInventory(MovieNode * node)
{

    // Left Node
    if(node->leftChild!=NULL)
        printMovieInventory(node->leftChild);

    // Value
    //cout<<"Movie: "<<node->title<< endl;
    cout<<"Movie: "<<node->title<<" "<<node->quantity<<endl;
    // Right Node
    if(node->rightChild!=NULL)
        printMovieInventory(node->rightChild);

}

void MovieTree::addMovieNode(int ranking, std::string title, int releaseYear, int quantity)
{

    // Create the node we will be inserting
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

    return;

}

/* used to find movie information, provides info or says movie can't be found */
void MovieTree::findMovie(std::string title)
{
    // Find the movie
    MovieNode * foundMovie = search(title);
    if (foundMovie != NULL)
    {
        cout << "Movie Info:" << endl;
        cout << "===========" << endl;
        cout << "Ranking:" << foundMovie->ranking << endl;
        cout << "Title:" << foundMovie->title << endl;
        cout << "Year:" << foundMovie->year << endl;
        cout << "Quantity:" << foundMovie->quantity << endl;
    }
    else
        cout << "Movie not found." << endl;

    return;
}

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
            // If the stock is 0 delete the movie
            if (foundMovie->quantity == 0)
                deleteMovieNode(foundMovie->title);

        }

    }
    // If it doesn't exist.
    else
        cout << "Movie not found." << endl;

}

void MovieTree::deleteMovieNode(std::string title)
{

    MovieNode * foundMovie = search(title);

    // If the movie exists
    if (foundMovie != NULL)
    {
        // If it has no children
        if (foundMovie->leftChild == NULL && foundMovie->rightChild == NULL)
        {
            // If this node is the left child, set the parents left child to NULL
            if (foundMovie->parent->leftChild == foundMovie)
                foundMovie->parent->leftChild = NULL;
            // Else, this node is the right child, set that to NULL
            else
                foundMovie->parent->rightChild = NULL;
            // Delete the node
            delete foundMovie;

        }
        // If it only has a left child
        else if (foundMovie->rightChild == NULL)
        {
            if (foundMovie->parent->leftChild == foundMovie)
                foundMovie->parent->leftChild = foundMovie->leftChild;
            else
                 foundMovie->parent->rightChild = foundMovie->leftChild;

            delete foundMovie;

        }
        // If it only has a right child
        else if (foundMovie->leftChild == NULL)
        {
            if (foundMovie->parent->leftChild == foundMovie)
                foundMovie->parent->leftChild = foundMovie->rightChild;
            else
                 foundMovie->parent->rightChild = foundMovie->rightChild;

            delete foundMovie;
        }

        // Node has two children, we need the smallest node from the right child
        else
        {
            // Start on the right sub-tree
            MovieNode * replacementNode = foundMovie->rightChild;

            // search for the smallest left child
            while (replacementNode->leftChild != NULL)
            {
                replacementNode = replacementNode->leftChild;
            }

            // Swap in all the info from the replacement to this node we are "deleting"
            foundMovie->title = replacementNode->title;
            foundMovie->quantity = replacementNode->quantity;
            foundMovie->ranking = replacementNode->ranking;
            foundMovie->year = replacementNode->year;


            // If the replacement node has a right child, update the parent
            if (replacementNode->rightChild != NULL)
                replacementNode->rightChild->parent = replacementNode->parent;

            // If the replacement node is a left child
            if (replacementNode->parent->leftChild == replacementNode)
                replacementNode->parent->leftChild = replacementNode->rightChild;
            // If it is a right child
            else
                replacementNode->parent->rightChild = replacementNode->rightChild;

            // Delete the node
            delete replacementNode;
        }
    }
    // If it doesn't exist
    else
        cout << "Movie not found." << endl;



}

int MovieTree::countMovieNodes()
{
    int *count = new int;
    *count = 0;
    if(root != NULL)
        *count = *count + 1;
    countMovieNodes(root, count);
    return *count;
}

/*
int MovieTree::countMovieNodes(MovieNode *node)
{
    if (node == NULL)
        return 0;
    return countMovieNodes(node->leftChild) + countMovieNodes(node->rightChild) + 1;
}
*/

void MovieTree::countMovieNodes(MovieNode *node, int *count)
{

    if (node->leftChild != NULL){
        *count = *count + 1;
        countMovieNodes(node->leftChild, count);
    }
    if(node->rightChild != NULL){
        *count = *count + 1;
        countMovieNodes(node->rightChild, count);
    }

}
