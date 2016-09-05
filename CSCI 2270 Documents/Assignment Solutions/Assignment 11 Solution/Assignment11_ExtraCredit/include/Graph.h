#ifndef GRAPH_H
#define GRAPH_H
#include<vector>
#include<iostream>

struct vertex;

struct adjVertex{
    vertex *v;
    int weight;
};

struct vertex{
    int ID;
    std::string name;
    int district;
    bool visited;
    int distance;
    std::vector<adjVertex> adj;
};

struct queueVertex{
    int distance;
    std::vector<vertex *> path;
    // Must override the less operatator so that we get a min value from priority queue.
    bool operator<(const queueVertex& a) const
    {
        return distance > a.distance;
    }
};

// Must override operator so we get a min value from priority queue.
struct Comparator {
public:
    bool operator()(const vertex* a, const vertex* b)
    {
        return (a->distance > b->distance);
    }
};

class Graph
{
    public:
        Graph();
        ~Graph();
        void addEdge(std::string v1, std::string v2, int weight);
        void addVertex(std::string name);
        void displayEdges();
        void assignDistricts();
        void BFTraversalLabel(std::string startingCity, int distID);
        void shortestPath(std::string startingCity,std::string endingCity);
        void shortestDistance(std::string startingCity,std::string endingCity);
        void shortestDistanceRoundTrip(std::string startingCity);
        vertex * findVertex(std::string name);

    protected:
    private:
        //vector<edge> edges;
        std::vector<vertex> vertices;

};

#endif // GRAPH_H
