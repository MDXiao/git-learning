from myGraph import MyGraph
import sys
def parseTestFromFileAndRun(fName):
    try:
        f = open(fName,'r')
        g = None
        d = None
        pi = None
        status = True
        compl = False
        for line in f:
            line=line.strip()
            lst=line.split(',')
            if (lst[0] == 'X'):
                compl=True
                return (compl,status)
            elif (lst[0] == 'N'):
                n = int(lst[1])
                g = MyGraph(n)
                print('Creating a graph with %d nodes'%(n))
            elif (lst[0] == 'E'):
                i = int(lst[1])
                j = int(lst[2])
                w = float(lst[3])
                g.addEdge(i,j,w)
                print('Added edge %d ---%f--> %d'%(i,w,j))
            elif (lst[0] == 'SSP'):
                srcID = int(lst[1])
                print('Calling single source shortest path:',srcID)
                (d,pi) = g.singleSourceShortestPath(srcID)
            elif (lst[0] == 'D'):
                destID = int(lst[1])
                reqd = float(lst[2])
                if (destID in d):
                    dHat = d[destID]
                    print('Distance check: %d (dist = %f, expected = %f )'%(destID,dHat,reqd))
                    if ( dHat <= reqd +  0.001 and dHat >= reqd - 0.001):
                        print('\t ----> Passed!')
                    else:
                        print('\t ----> FAIL. Computed distance is wrong')
                        status=False
                else:
                    print('Distance check: %d (dist = NOT COMPUTED, expected = %f )'%(destID,reqd))
                    print('\t ---> FAIL: Is your Dijkstra algorithm complete? ')
            elif (lst[0] == 'P'):
                srcID = int(lst[1])
                destID = int(lst[2])
                ePath=[int(lst[j]) for j in range(3,len(lst)) ]
                aPath=g.getShortestPath(d,pi,srcID,destID)
                print('Shortest path check')
                print('\t Expected Path:',ePath)
                print('\t Your code returned path:',aPath)
                if (len(aPath) != len(ePath)):
                    print('\t FAILED')
                    status=False
                else:
                    pLen = len(ePath)
                    for i in range(0,pLen):
                        if (aPath[i] != ePath[i]):
                           print('\t FAILED')
                           status=False
            else:
                print('Invalid command: ',line)
        return (compl,status)
        
    except IOError:
        print ('FATAL ERROR: could not open ',fName)
        return (False,False)


if (__name__ == '__main__'):
    m = len(sys.argv)
    if ( m >= 2):
        fileName=sys.argv[1]
    else:
        fileName='test1.spec'
    if (not fileName.endswith('.spec')):
        fileName=fileName+'.spec'
    print('Testing file:',fileName)
        
    (compl,status)= parseTestFromFileAndRun(fileName)
    if (compl):
        print('Spec file completed ')
    else:
        print('Incomplete')
    if (status):
        print(fileName,' Passed')
    else:
        print('Failed: ',fileName)
