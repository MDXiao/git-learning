from __future__ import print_function
import sys
import random
import mySortingFunctions as msf

def generateRandomList(lstSize):
    assert(lstSize > 0)
    lst = []
    for i in range(0,lstSize):
        r = random.randint(-40,40)
        lst.append(r)
    return lst

def callUserSortFunction(lst,fns):
    if ('merge' in fns):
        retList = msf.mergeSort(lst)
    elif ('quick' in fns):
        retList = msf.quickSort(lst)
    elif ('insertion' in fns):
        retList = msf.insertionSort(lst)
    else:
        print('Unknown function:',retList)
        sys.exit(1)
    return retList

def checkAscendingSorted(lstRet,lst):
    # Are they the same size
    if (len(lstRet) == len(lst)):
        # Is lstRet sorted?
        for i in range(0,len(lstRet)-1):
            if (lstRet[i] > lstRet[i+1]):
                return (False, ('Not sorted in ascending order at position %d'%(i)))
        # Is lstRet same as lst?
        for i in lst:
            if (i not in lstRet):
                return (False, '%d in original list but not in sorted list'%(i))
        for i in lstRet:
            if (i not in lst):
                return (False, '%d in sorted list but not in original list'%(i))
        return (True, 'OK')
    else:
        return (False,'Sizes of list differ. Original = %d and Sorted = %d '%(len(lst), len(lstRet)))


def failedTest(lst,lstRet,fns,reason):
    print('---------')
    print('Failed Test for ', fns, ' Original:',lst, 'Returned', lstRet )
    print(reason)
    print('---------\n')
    
    
if __name__ == '__main__':
    if (len(sys.argv) <= 1):
        sortsToGrade=['mergeSort','quickSort','insertionSort']
    else:
        sortsToGrade=sys.argv[1:len(sys.argv)] 

    print('Grading the following functions:',sortsToGrade)
    nGradingRuns = 100
    fail = False
    testPassed = 0
    for i in range(0,nGradingRuns):
        listSize = random.randint(5,50)
        lst=generateRandomList(listSize)
        for fns in sortsToGrade:
            lstRet = callUserSortFunction(lst,fns)
            (passed,reason) = checkAscendingSorted(lstRet,lst)
            if (passed):
                testPassed = testPassed +1
            else:
                failedTest(lst,lstRet,fns,reason)
                fail = True

    if (fail):
        print('failures encountered.')
    else:
        print('All ', testPassed, 'tests passed. Congratulations!')
    


