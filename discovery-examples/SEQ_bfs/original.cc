//Modified version of http://www.geeksforgeeks.org/breadth-first-traversal-for-a-graph/

// compile with: -std=c++11

# include <iostream>
# include <algorithm>
# include <vector>
# include <list>

using namespace std;

class Graph
{
    vector< vector<int> > Edge;
    vector<unsigned char> visited; //We can use vector<bool>, but it's slow and broken :( , CPP's byte array http://stackoverflow.com/questions/10077771/what-is-the-correct-way-to-deal-with-medium-sized-byte-arrays-in-modern-c
public:

    Graph(int V)
    {
        Edge.resize(V);
        visited.resize(V);
    }

    void add_edge(int v,int w)
    {
        Edge[v].push_back(w);
    }

    void BFS(int s)
    {
        for(auto i : visited) i = false;
        list<int> q;
        visited[s] = true;
        q.push_back(s);
	// Annotation: This loop visits each node in the graph exactly one
	// Furthermore: cout<<s<<" " is the loop payload, the rest is the iterator
	// Possible annotation: This is actually a graph traversal, BFS specifically
	// Depending on the payload, other traversal schemes or implementations could be used
        while (!q.empty())
        {
            s = q.front();
            cout<<s<<" ";
            q.pop_front();
            for(auto i : Edge[s])
            {
                if(!visited[i])
                {
                    visited[i] = true;
                    q.push_back(i);
                }
            }
        }

    }

};

int main(int argc, char ** argv)
{
    Graph g(6);
    g.add_edge(0,1);
    g.add_edge(0,2);
    g.add_edge(1,2);
    g.add_edge(2,0);
    g.add_edge(2,3);
    g.add_edge(3,3);
    g.BFS(2);
}
