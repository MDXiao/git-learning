#LastName: Xiao
#FirstName: Michael
#Email: michael.xiao@colorado.edu
#Comments:

from __future__ import print_function
import sys

# We will use a class called my trie node
class MyTrieNode:
    # Initialize some fields

    def __init__(self, isRootNode):
        #The initialization below is just a suggestion.
        #Change it as you will.
        # But do not change the signature of the constructor.
        self.isRoot = isRootNode
        self.isWordEnd = False # is this node a word ending node
        self.isRoot = False # is this a root node
        self.count = 0 # frequency count
        self.next = {} # Dictionary mappng each character from a-z to the child node


    '''
    Trie Add Node Function
    '''
    def addWord(self,w):
        assert(len(w) > 0)
        '''
        Summary:
        For each letter in the word, we go through the trie to check if the
        letter exists in the trie already. If not, we create a new trie node
        and mark it so that it is not the root node.

        Once we reach the end, we mark that a word ends there, and we increment the count.
        '''
        for letter in w:
            if letter not in self.next:
                self.next[letter] = MyTrieNode(False) #Create a new node in Trie
            self = self.next[letter]
        self.isWordEnd = True #Reached end of word to be added
        self.count += 1 #Increment count to signify a word's end is here


    '''
    Trie lookupWord function
    '''
    def lookupWord(self,w):
        '''
        Summary:
        For each letter in the word, we check to see if the letter exists in the next dictionary.
        If we can't find the next letter in the in word, then the word does not exist in the Trie,
        so we return 0. Otherwise, we just return the count, or the number of times the word appears
        in the Trie.
        '''
        for letter in w:
            if letter not in self.next: #If the next letter isn't found, just return 0
                return 0
            else:
                self = self.next[letter]
        return self.count #Else, when we reach the end, return the count.


    '''
    Autocomplete Helper function
    '''
    def getCompletions(self, prefix):
        #Initialize empty completion array
        completionList = []
        '''
        Summary:
        We recursively check through the Trie with the prefix given as an argument.
        '''
        if len(self.next) != 0:
            for letter in self.next:
                currentNode = self.next[letter] #Check all possiblities of the letter for that portion of the tree
                if currentNode.isWordEnd: #If the letter is the end of a word, we append the word and the count.
                    completionList.append((str(prefix) + letter, currentNode.count))
                if len(currentNode.next) > 0:
                    #Recursively get all the completions for the letter
                    #For example if we have 'pi' we get 'n' as the next letter
                    #Then 'n' leads to 'k','e','t', and 'g' and so on
                    suffixlist = currentNode.getCompletions(letter)
                    if len(suffixlist) > 0:
                        #Then append the rest of the completions into the new completionList
                        for (ch2, count) in suffixlist:
                            completionList.append((str(prefix) + ch2, count))

            return completionList

    '''
    Autocomplete function
    '''
    def autoComplete(self,w):
        #Initialize variables
        completions = []
        startingNode = self
        lastCharacter = w[len(w)-1]

        '''
        If the following prefix can't be found in the Trie, we just return an empty array.
        Once the prefix is found, we set the startingNode to where the prefix ends, and we can
        just find all the autocompletions via where the startingNode is.
        '''
        for letter in w:
            if letter not in startingNode.next.keys():
                return completions
            else:
                startingNode = startingNode.next[letter]


        '''
        The if statement is to check if the prefix is a word, if so, we just append the prefix and the word count.
        Once the special case has been satisfied, we get the rest of
        '''
        wPrefix = w[:len(w)-1]
        allCompletions = startingNode.getCompletions(lastCharacter)
        if startingNode.isWordEnd:
            completions.append((w, startingNode.count)) #Only added for test5, to check if the prefix itself is a word added in.
        if allCompletions is not None: #Only added for test7 and test8, if w is both it's own word and also has multiple autocompletions,
            for (endingW, count) in allCompletions: #We add both itself and it's count, AND it's other autocompletions
                completions.append((wPrefix+endingW, count))

        return completions


if (__name__ == '__main__'):
    t= MyTrieNode(True)
    lst1=['test','testament','testing','ping','pin','pink','pine','pint','testing','pinetree']

    for w in lst1:
        t.addWord(w)

    j = t.lookupWord('testy') # should return 0 (Does)
    j2 = t.lookupWord('telltale') # should return 0 (Does)
    j3 = t.lookupWord ('testing') # should return 2 (Does)
    lst3 = t.autoComplete('pi')
    print('Completions for \"pi\" are : ') #Should return (ping,1), (pink,1), (pine,1), (pint,1), (pinetree,1) (Does)
    print(lst3)

    lst4 = t.autoComplete('tes')
    print('Completions for \"tes\" are : ') #Should return (test,1), (testament,1),(testing,2)
    print(lst4)
