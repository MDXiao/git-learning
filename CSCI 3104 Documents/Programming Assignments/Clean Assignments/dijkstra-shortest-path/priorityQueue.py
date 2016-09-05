from random import uniform,randrange
class PriorityQueue:
    ''' 
      A very naive implementation of a priority queue using a dictionary
      pq.Inf refers to  the value INFINITY
      pq.n : number of vertices
      pq.d : the actual map from vertex ids to distance estimates
      
      Methods.
        pq.set(i,dist): set node i value to dist
        pq.hasKey(i): does node i exist in pq
        pq.get(i): get the value associated with node i
        pq.extractMin(): extract the minimum value and remove it from the queue
        pq.isEmpty(): is the priority queue empty?
    '''
    
    def __init__ (self, nNodes):
        # 
        # Create a priority code with nNodes vertices in the graph
        #
        self.n = nNodes
        self.d = {}
        self.Inf = float('inf') # A very large number
        
    def set(self, i, dist):
        # Set the distance of vertex i to dist
        assert(i >= 0 and i < self.n)
        self.d[i] = min(dist, self.Inf)

    def hasKey(self,i):
        # Check if the vertex i belongs to the priority queue
        return i in self.d
    
    def get(self,i):
        # Get the value associated with vertex i in the queue
        assert(i >= 0 and i < self.n)
        assert(i in self.d)
        return self.d[i]

    def extractMin(self):
        # Extract the minimum distance node from priority queue along with the value
        # of the minimum distance.
        # Remove the node from the queue as well.

        m = len(self.d)
        assert (m > 0), 'You are attempting to extract min from an empty queue'
        
        minDist = self.Inf 
        minKey = -1
        
        for (i,dist) in self.d.items(): # Iterate through all the items in the dictionary d
            if (dist <= minDist): # If it is smaller than minimal distance so far
                minDist = dist    # update the minimal distance
                minKey = i
        assert (minKey >=  0.0), ('minKey = '+str(minKey)+' dist='+str(dist))
        self.d.pop(minKey,None)   # Remove from the queue
        

        return (minKey, minDist)  # Return the minimum key and the distance.

    def isEmpty(self):
        # Is the priority queue empty? If yes, return TRUE otherwise, return FALSE.
        m = len(self.d)   
        return (m <= 0)

    
#TEST priority Queue

def test1():
    pq = PriorityQueue(100) # Create a priority queue for at most 100 ids
    for i in range(0,100):  # Set each key to some random value
        v = uniform(0.0,10.0)
        pq.set(i,v)
    for j in range(0,50):   # Generate 50 keys at random and modify their values
        i = randrange(0,99)
        v = uniform(0.0,10.0)
        pq.set(i,v)
        
    while(not pq.isEmpty()): # repeatedly do extractMin and print until empty
        (minKey,minValue) = pq.extractMin()
        print(minKey,',',minValue) # This will now be sorted in increasing order.
    
if __name__=='__main__':
    test1()
