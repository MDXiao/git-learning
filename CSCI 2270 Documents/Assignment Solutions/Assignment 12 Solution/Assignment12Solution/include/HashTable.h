#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <string>
#include <vector>

struct HashElem{
	std::string title;
	int year;
	HashElem *next;
    HashElem *previous;

	HashElem(){};
	HashElem(std::string in_title, int in_year)
	{
		title = in_title;
		year = in_year;
		next = NULL;
		previous = NULL;
	}

};

class HashTable
{
	public:
		HashTable();
		~HashTable();
		void printTableContents();
		void insertMovie(std::string name, int year);
		void deleteMovie(std::string name);
		void findMovie(std::string name);
	private:
		int hashSum(std::string x, int s);
		int tableSize = 10;
		HashElem* hashTable[10];

};

#endif // HASHTABLE_H
