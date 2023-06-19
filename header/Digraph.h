#ifndef DIGRAPH
#define DIGRAPH

#include"path.h"
#include"Header.h"
/*Digraph class represents a directed graph data structure and used to represent system model as directed graph
and is used in DFS algorithm to get all pathsets in the system */
class Digraph
{
private:
	//map to store source node and target node pairs and their corresponding path pointer
	std::map< std::pair<std::string, std::string>, Path*> path_map;

public:

	Digraph();
	~Digraph();

	void Reset();
	//pair up the source node and target node if PathExists function returns true
	//and get the path pointer for each pair in path_map
	void AddPath(Path* l);
	void AddPath(std::string s, std::string d);
	//check if a path exists between source and target node 
	bool PathExists(std::string sourceID, std::string targetID);
	//Function to get the adjacent nodes and store in a vector
	std::vector< std::string > GetAdjNodeIDs(std::string n);
};


#endif /* DIGRAPH */
