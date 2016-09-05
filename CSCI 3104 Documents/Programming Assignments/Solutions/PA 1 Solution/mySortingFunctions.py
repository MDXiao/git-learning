# Name: Michael Xiao
# Email: mixi3377@colorado.edu
# SUID: 102358826
#
# By submitting this file as my own work, I declare that this
# code has been written on my own with no unauthorized help. I agree to the
# CU Honor Code. I am also aware that plagiarizing code may result in
# a failing grade for this class.
from __future__ import print_function
import sys
import random
import time
import matplotlib.pyplot as plt
import numpy as np


# --------- Insertion Sort -------------
# Implementation of getPosition
# Helper function for insertionSort
def getPosition(rList, elt):
    # Find the position where element occurs in the list
    #
    for (i,e) in enumerate(rList):
        if (e >= elt):
            return i
    return len(rList)

# Implementation of Insertion Sort
def insertionSort(lst):
    n = len(lst)
    retList = []
    for i in lst:
        pos = getPosition(retList,i)
        retList.insert(pos,i)
    return retList

#------ Merge Sort --------------
def mergeSort(lst):
    # TODO: Implement mergesort here
    # You can add additional utility functions to help you out.
    # But the function to do mergesort should be called mergeSort

    n = len(lst)
    i = 0
    j = 0
    k = 0
    if n > 1:
        midpoint = int(n/2)
        a = lst[:midpoint]
        b = lst[midpoint:]
        mergeSort(a)
        mergeSort(b)
        n1 = len(a)
        n2 = len(b)
        while i < n1 and j < n2:
            if a[i] < b[j]:
                lst[k] = a[i]
                i = i + 1
            else:
                lst[k] = b[j]
                j = j + 1
            k = k + 1

        while i < n1:
            lst[k] = a[i]
            i = i + 1
            k = k + 1

        while j < n2:
            lst[k] = b[j]
            j = j + 1
            k = k + 1
    return lst


#------ Quick Sort --------------
def quickSort(lst):
    # TODO: Implement quicksort here
    # You may add additional utility functions to help you out.
    # But the function to do quicksort should be called quickSort

    #Will implement an in place pivot quicksort

    firstPart = []
    pivotPart = []
    secondPart = []
    n = len(lst)
    if len(lst) > 1:
        pivot = random.choice(lst)
        for x in lst:
            if x < pivot:
                firstPart.append(x)
            elif x == pivot:
                pivotPart.append(x) #Append Pivot to own List
            else:
                secondPart.append(x)
        return quickSort(firstPart) + pivotPart + quickSort(secondPart)
    else:
        return lst




# ------ Timing Utility Functions ---------

# Code below is given only for demonstration purposes

# Function: generateRandomList
# Generate a list of n elements from 0 to n-1
# Shuffle these elements at random

def generateRandomList(n):
   # Generate a random shuffle of n elements
   lst = list(range(0,n))
   random.shuffle(lst)
   return lst


def measureRunningTimeComplexity(sortFunction,lst):
    t0 = time.clock()
    sortFunction(lst)
    t1 = time.clock() # A rather crude way to time the process.
    return (t1 - t0)

def worstComplexityData(sortFunction,n):
    data = []
    for i in range(0,20):
        lst = generateRandomList(n)
        data.append(measureRunningTimeComplexity(sortFunction,lst))
    return max(data)

def averageComplexityData(sortFunction,n):
    data = []
    for i in range(0,20):
        lst = generateRandomList(n)
        data.append(measureRunningTimeComplexity(sortFunction,lst))
    return sum(data)/20


def main():

    worstInsertion = []
    worstMerge = []
    worstQuick = []
    averageInsertion = []
    averageMerge = []
    averageQuick = []
    for i in range(5,500,5):
        worstInsertion.append(worstComplexityData(insertionSort,i))
        worstMerge.append(worstComplexityData(mergeSort,i))
        worstQuick.append(worstComplexityData(quickSort,i))
        averageInsertion.append(averageComplexityData(insertionSort,i))
        averageMerge.append(averageComplexityData(mergeSort,i))
        averageQuick.append(averageComplexityData(quickSort,i))


    print("\n")
    print("                  Total Average and Worst Times     ")
    print("==================================================================")
    print("InsertionSort----> Worst: ", '%.6f'%max(worstInsertion), " Average: ", '%6f'%(sum(averageInsertion)/99))
    print("MergeSort--------> Worst: ", '%.6f'%max(worstMerge), " Average: ", '%.6f'%(sum(averageMerge)/99))
    print("QuickSort--------> Worst: ", '%.6f'%max(worstQuick), " Average: ", '%.6f'%(sum(averageQuick)/99))
    print("\n")

    n = []
    for i in range(5,500,5):
        n.append(i)

     #TO see quicksort and insertionsort, just replace worstMerge and averageMerge with their
     #Respective Counterparts
    plt.plot(n,worstInsertion,label="Worst Case")
    plt.plot(n,averageInsertion,label="Average Case")
    plt.title("Average/Worst Case Complexity for InsertionSort")
    plt.legend(loc="upper left")
    plt.xlabel("N size")
    plt.ylabel("Time (Seconds)")
    plt.show()

    '''
    plt.plot(n,worstMerge,label="Worst Case")
    plt.plot(n,averageMerge,label="Average Case")
    plt.title("Average/Worst Case Complexity for MergeSort")
    plt.legend(loc="upper left")
    plt.xlabel("N size")
    plt.ylabel("Time (Seconds)")
    plt.show()

    plt.plot(n,worstQuick,label="Worst Case")
    plt.plot(n,averageQuick,label="Average Case")
    plt.title("Average/Worst Case Complexity for QuickSort")
    plt.legend(loc="upper left")
    plt.xlabel("N size")
    plt.ylabel("Time (Seconds)")
    plt.show()
    '''

if __name__ == "__main__":
        main()
# --- TODO

# Write code to extract average/worst-case time complexity for your sorting routines.
