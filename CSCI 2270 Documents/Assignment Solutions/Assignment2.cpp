// ==========================================
// Filename: Assignment2Solution.cpp
// Author: Matt Bubernak
// Date: 1/21/2015
// Description: Counts unique words in a file
// outputs the top 10 most common words
// ==========================================

#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

// struct to store word+count combinations
struct wordItem{
    string word;
    int count;
};

// used to sort our final array
void ArraySort(wordItem arr[], int size);
//yes, this is a super-duper icky conditional, and there is a better way
//but, it works
bool checkIfCommonWord(string word){
    bool commonWord = false;
    if (word== "the")
        commonWord = true;
    else if(word == "be")
        commonWord = true;
    else if (word == "to")
        commonWord = true;
    else if(word == "of")
        commonWord = true;
    else if(word == "and")
        commonWord = true;
    else if(word == "a")
        commonWord = true;
    else if(word == "in")
        commonWord = true;
    else if(word == "that")
        commonWord = true;
    else if(word == "have")
        commonWord = true;
    else if(word == "i")
        commonWord = true;
    else if(word == "it")
        commonWord = true;
    else if(word == "for")
        commonWord = true;
    else if(word == "not")
        commonWord = true;
    else if(word == "on")
        commonWord = true;
    else if(word == "with")
        commonWord = true;
    else if(word == "he")
        commonWord = true;
    else if(word == "as")
        commonWord = true;
    else if(word == "you")
        commonWord = true;
    else if(word == "do")
        commonWord = true;
    else if(word == "at")
        commonWord = true;
    else if(word=="this")
        commonWord = true;
    else if(word=="but")
        commonWord = true;
    else if(word=="his")
        commonWord = true;
    else if(word=="by")
        commonWord = true;
    else if(word=="from")
        commonWord = true;
    else if(word=="they")
        commonWord = true;
    else if(word=="we")
        commonWord = true;
    else if(word=="say")
        commonWord = true;
    else if(word=="her")
        commonWord = true;
    else if(word=="she")
        commonWord = true;
    else if(word=="or")
        commonWord = true;
    else if(word=="an")
        commonWord = true;
    else if(word=="will")
        commonWord = true;
    else if(word=="my")
        commonWord = true;
    else if(word=="one")
        commonWord = true;
    else if(word=="all")
        commonWord = true;
    else if(word=="would")
        commonWord = true;
    else if(word=="there")
        commonWord = true;
    else if(word=="their")
        commonWord = true;
    else if(word=="what")
        commonWord = true;
    else if(word=="so")
        commonWord = true;
    else if(word=="up")
        commonWord = true;
    else if(word=="out")
        commonWord = true;
    else if(word=="if")
        commonWord = true;
    else if(word=="about")
        commonWord = true;
    else if(word=="who")
        commonWord = true;
    else if(word=="get")
        commonWord = true;
    else if(word=="which")
        commonWord = true;
    else if(word=="go")
        commonWord = true;
    else if(word=="me")
        commonWord = true;
    else
        commonWord = false;
    return commonWord;
}
// main expects 1 argument, filename
int main(int argc, char* argv[])
{
    char * fileName;        // stores the file we will be opening
    ifstream inStream;      // stream for reading in file

    wordItem *arr;          // array for storing words
    wordItem *arr2;         // used for dynamically doubling array size

    int timesDoubled = 0;   // stores the times we double the array
    int arrLoc = 0;         // stores the location of the last element in the arr
    int arrCapacity = 100;   // stores the capacity of te array
    int numCommon = atoi(argv[2]);
    // verify we have the correct number of parameters, else throw error msg & return
    if (argc != 3){
        cout << "Usage: Assignment2Solution <filename.txt> <number of words>" << endl;
        return 0;
    }

    // open the file
    fileName = argv[1];
    inStream.open(fileName);

    // throw an error if we couldn't open the file
    if (!inStream)
    {
        cout << "Error: Could not open file for reading" << endl;
        return 0;
    }

    arr = new wordItem[arrCapacity]; // declare our initial array
    string word;                     // stores each word we read in
    wordItem tmp;                    // stores our word item temporarily
    bool wordFound;                  // flag for whether we have found the word

    // loop until the end of the file
    while (!inStream.eof())
    {
        wordFound = false;
        // read in a word
        inStream >> word;
        //remove the common words
        bool commonWord = checkIfCommonWord(word);
        if(!commonWord){
            // check if the word has already been found
            for (int i = 0; i < arrLoc; i++)
            {
                if (word==arr[i].word)
                {
                    wordFound=true;
                    arr[i].count++;
                }
            }

            // if the word was never found, add it to the array
            if (wordFound == false)
            {
                tmp.word = word;
                tmp.count = 1;

                // if there is space for our element
                if (arrLoc < arrCapacity)
                {
                    arr[arrLoc] = tmp;
                    arrLoc++;
                }

                // if there is not space, double the size of the array
                else
                {
                    // double the capacity
                    arrCapacity*=2;
                    // allocate new array
                    arr2 = new wordItem[arrCapacity];
                    // copy the original array in
                    for (int i = 0; i < arrCapacity/2; i++)
                    {
                        arr2[i] = arr[i];
                    }
                    arr2[arrCapacity/2] = tmp;
                    arrLoc++;
                    // free the memory associated with arr
                    delete[] arr;
                    // make arr point to the new memory we just allocated
                    arr = arr2;
                    // incrament our double count
                    timesDoubled++;
                }
            }
        }

    }
    // close the file
    inStream.close();

    // sort the array , bubble sort for simplicity
    ArraySort(arr,arrLoc);

    for (int i = 0; i < numCommon; i++)
    {
        cout << arr[i].count << " - " << arr[i].word << endl;
    }
    cout << "#" << endl;
    cout << "Array doubled: "<<timesDoubled << endl;
    // display the output
    cout << "#" << endl;
    cout<<"Unique non-common words: "<<arrLoc<<endl;
    cout<<"#"<<endl;
    int totalWords = 0;
    for(int i = 0; i < arrLoc; i++){
        totalWords += arr[i].count;
    }
    cout<<"Total non-common words: "<<totalWords<<endl;
    // free the final array
    delete[] arr;

    return 0;
}


// ================================================
// Function Name: ArraySort
// Pre-condition: Array of wordItems, size of array
// Post-Condition: Array is sorted(slowly!!)
// ================================================
void ArraySort(wordItem arr[], int size)
{
    int i, j, flag = 1;
    wordItem temp;
    for(i = 1; (i <= size) && flag; i++)
    {
      flag = 0;
      for (j=0; j < (size -1); j++)
      {
           if (arr[j+1].count > arr[j].count)
           {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
                flag = 1;
           }
      }
    }
    return;
}
