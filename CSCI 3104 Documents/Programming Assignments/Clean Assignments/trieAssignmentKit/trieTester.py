from __future__ import print_function
import sys
from trieClass import MyTrieNode

def lookupTest(t, lstToLookup):
    retValue = True
    for (w,k) in lstToLookup:
        j = t.lookupWord(w)
        if (j != k):
            print('\t Lookup for word %s failed. Expected result: %d, obtained from your trie: %d \n'%(w,k,j))
            retValue = False
    return retValue

def autoCompleteTest(t, stem, expResult):
    lst1 = sorted(t.autoComplete(stem))
    lstRet = sorted(expResult)
    if (len(lst1) != len(lstRet)):
        print('\t autoComplete(\"%s\") failed'%(stem))
        print('\t\t Expected: ',lstRet)
        print('\t\t Got: ', lst1)
        return False
    n = len(lstRet)
    for i in range(0,n):
        (expI,freqI) = lstRet[i]
        (gotI,freqJ) = lst1[i]
        if (expI != gotI or freqI != freqJ):
            print('autoComplete(\"%s\") failed'%(stem))
            print('\t Expected: ',lstRet)
            print('\t Got: ', lst1 )
            return False   
    return True

def runTest(fileName):
    try:
        print('Running',fileName)
        file = open(fileName+'.spec','r')
        t = MyTrieNode(True)
        lNum = 0
        result = True
        for line in file:
            lNum += 1
            lst = [x.strip() for x in line.split(',')]
            if (lst[0] == 'W'):
                print('\t Insert:',lst[1])
                t.addWord(lst[1])
            elif (lst[0] == 'L'):
                print('\t Lookup:', lst[1])
                j = t.lookupWord(lst[1])
                if (j != int(lst[2])):
                    print('\t\t Failed --> expected : %s, got %d'%(lst[2],j))
                    result=False
            elif (lst[0] == 'A'):
                wrd = lst[1]
                rList = lst[2::]
                rWords = rList[0::2]
                print('\t Autocomplete: ',lst[1])
                rNums = [int(x) for x in rList[1::2] ]
                retList = sorted(zip (rWords,rNums))
                result = (autoCompleteTest(t,wrd, retList)) and result
            else:
                print('Error in test specification line number %d -- Unknown command spec %s'%(lNum,lst[0]))
                sys.exit(1)
        return result
    except IOError:
        print('Unable to open',fileName)


if (__name__=='__main__'):
    if (len(sys.argv) <=1 ):
        fileName = 'test1'
    else:
        fileName = sys.argv[1]
    
    rslt = runTest(fileName)
    if (rslt):
        print(fileName, 'passed')
    else:
        print(fileName, 'failed')

    
    

        
                         
        
    
