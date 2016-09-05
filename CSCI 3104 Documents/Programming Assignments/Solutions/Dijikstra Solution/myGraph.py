#Name: Michael Xiao
#Student ID Number: 102358826
from priorityQueue import PriorityQueue

class MyGraph:

    # self.n : number of vertices in the graph
    # self.adjList: Adjacency list stored as a list of lists.
    #    self.adjList[i] stores all adjacent nodes to vertex ID i along with edge weights.
    # Eg., if vertex 2 has three edges (2,3) with weight 4.5, (2,4) with weight 3.2, and (2,5) with 2.1
    #    self.adjList[2] is the list [ (3,4.5), (4,3.2), (5,2.12) ]
    # your goal is to implement the singleSourceShortestPath function.


    def __init__(self, nVertices):
        # self.n represents the number of vertices of the graph
        self.n = nVertices
        # Adjacency list is initially emtpy
        # adjList[i] = [ (j1, w1), ..., (jk,wk) ]
        #   is a list of adjacent vertices and associated edge weights w1,...,wk

        self.adjList = [ [] for i in range(0,nVertices) ]


    def getName(self,i):
        # Get the name for vertex i: currently it is just i
        return str(i)


    def addEdge(self,i,j,w):
        # Add an edge to graph from i to j with weight w
        assert( i >= 0 and i < self.n)
        assert( j >= 0 and j < self.n and j != i)
        # Append j to adjacency list of i
        lst = self.adjList[i]
        lst.append( (j,w) ) # Add j as adjacent with weight w



    def prettyPrintAdjacencyList(self):
        # Pretty print the adjacency list
        for i in range(0,self.n): #Iterate over all vertices
            lst = self.adjList[i] # Get the list of adjacent vertices
            print(self.getName(i), '---> [', end='')
            sep='' # Pretty printing stuff
            for (j,w) in self.adjList[i]:
                print(sep, '( ', self.getName(j), ',', w,' )',end='')
                sep=', '
            print(']')



    def singleSourceShortestPath(self, srcID):
        assert (srcID >= 0 and srcID < self.n)
        # Implement Dijkstra's algorithm
        # Input:
        # self --> a reference to a MyGraph instance
        # srcID: the id of the source vertex.
        # Expected Output: (d,pi)
        #    d  --> Map each vertex id v to the distance from srcID
        #    pi --> Map each reachable vertex id v (except for srcID) to a parent.

        # Initialize the priority queue
        pq = PriorityQueue(self.n) #create the priority queue

        for i in range(0,self.n): # Iterate through all vertex ID
            if ( i == srcID):     # If ID is srcID
                pq.set(i,0.0)     # Distance of srcID should be zero
            else:                 # ID is not srcID
                pq.set(i, pq.Inf) # Distance should be infinity

        d = {}  # Initialize the map with distances to nodes
        pi = {} # Initialize the map with parents of vertices

        #Code Starts HERE

        d[srcID] = 0 #Set distances from source equal to 0
        pi[srcID] = None #Set parents to None

        while not pq.isEmpty(): #While the priority queue is not empty
            (currentVertex,currentDistance) = pq.extractMin() #Extract the Min
            for (nextVertex, weight) in self.adjList[currentVertex]:
                newDist = weight + currentDistance
                if pq.hasKey(nextVertex): #Check if nextVertex exists in priority queue
                    if newDist < pq.get(nextVertex): #If the new distance is less
                        d[nextVertex] = newDist #Set new distnace
                        pi[nextVertex] = currentVertex #Set the parents
                        pq.set(nextVertex,newDist) #Set the new distance for the vertex
        return (d,pi)

    def getShortestPath(self,d,pi,srcID, destID):
        # Routine to retreive shortest path
        # d: map for each vertex v to its distance from shortest path
        # pi: map from each reachable vertex id to parent
        # srcID: source id (should be the same source id as call to singleSourceShortestPath function)
        # destID: destination id
        # RETURN
        #  lst : a list of nodes to visit with first element of the list as srcID and last one as destID.
        assert (srcID in d), 'It looks like your Dijkstra code is not complete'
        assert (d[srcID] <= 0.0)
        if (destID not in pi):
            assert False , ' No path from the given source : %d to destination: %d'%(srcID,destID)
        lst = [destID]
        curNode = destID
        while (curNode != srcID):
            assert(curNode in pi)
            curNode = pi[curNode]
            lst.insert(0,curNode)
        return lst


# Test our graph

def test1():
    g = MyGraph(5)

    # Edges
    g.addEdge(0,1,0.1) # a -> b
    g.addEdge(0,2,0.3) # a -> c
    g.addEdge(0,3,0.4) # a -> d
    g.addEdge(1,3,0.2) # b -> d
    g.addEdge(2,3,0.8) # c -> d
    g.addEdge(3,4,0.7) # d -> e
    g.addEdge(4,2,1.0)
    # Pretty Print
    print('Adjacency List')
    g.prettyPrintAdjacencyList()
    (d,pi) = g.singleSourceShortestPath(0)
    print('Single source shortest path from 0')
    for i in range(0,5):
        if (i in d):
            print (i ,' ---> ', d[i])
    lst=g.getShortestPath(d,pi,0,4)
    print('Shortest path from 0 to 4:',lst)
    #Shortest Path from 0 to 4 is
    #The shortest path should be a distance of 1.0 from 0,1,3,4

def test2():
    g = MyGraph(8)
    g.addEdge(0,1,0.1)
    g.addEdge(0,3,0.3)
    g.addEdge(1,2,0.5)
    g.addEdge(1,3,0.4)
    g.addEdge(1,4,0.3)
    g.addEdge(3,4,0.2)
    g.addEdge(2,4,0.1)
    g.addEdge(2,5,0.8)
    g.addEdge(2,6,1.2)
    g.addEdge(4,5,1.3)
    g.addEdge(4,6,2.4)
    g.addEdge(5,7,3.5)
    g.addEdge(6,7,1.2)
    g.addEdge(7,4,0.6)
    print('Adjacency List')
    g.prettyPrintAdjacencyList()
    (d,pi) = g.singleSourceShortestPath(2)
    print('Single source shortest path from 2')
    for i in range(0,8):
        if (i in d):
            print (i ,' ---> ', d[i])
    lst=g.getShortestPath(d,pi,2,7)
    print('Shortest path from 2 to 7:',lst)


if (__name__=='__main__'):
    print('Graph Test # 1')
    test1()
    print('Graph Test # 2')
    test2()
