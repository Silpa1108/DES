
#include "Digraph.h"
#include"Header.h"

using namespace std;

Digraph::Digraph()
{

}

Digraph::~Digraph()
{
	Reset();
}

// Re-set all the Digraph data
void Digraph::Reset()
{
	// Delete the path pointers
	for (map< std::pair<std::string, std::string>, Path*>::iterator it = path_map.begin();
		it != path_map.end();
		it++)
	{
		Path* l = (*it).second;
		delete l;
		l = NULL;
	}

	path_map.erase(path_map.begin(), path_map.end());
}



void Digraph::AddPath(Path* l)
{
	// update one-to-one mapping between each path added
	// and its unique node pair

	std::string s, d;
	l->GetSourceTargetNodes(s, d);
	std::pair<std::string, std::string> p(s, d);            //pair source and dest before putting the pair to path_map 
	path_map[p] = l;									//path pointer for each pair in path_map
}



void Digraph::AddPath(std::string s, std::string d)
{
	if (!PathExists(s, d))
	{
		AddPath(new Path(s, d));       //new path object created 
	}
}

bool Digraph::PathExists(std::string sourceID, std::string targetID)
{
	bool found = false;

	std::map< std::pair<std::string, std::string>, Path* >::iterator it1;

	std::pair<std::string, std::string> p1(sourceID, targetID);


	it1 = path_map.find(p1); //get path pointer to the pair p1
	//if it1 point to end means linkmap is empty and no path found(False)
	found = it1 != path_map.end();


	return found;
}


//Function to get the adjacent nodes and store in a vector
std::vector< std::string > Digraph::GetAdjNodeIDs(std::string n)
{
	vector< std::string > adj;

	std::map< std::pair<std::string, std::string>, Path* >::iterator link_it =
		path_map.begin();

	for (;
		link_it != path_map.end();
		link_it++)
	{
		std::string start = "start";
		std::string target = "start";

		Path* l = (*link_it).second;

		l->GetSourceTargetNodes(start, target);

		if (PathExists(start, target))
		{
			if (start == n)
			{
				adj.push_back(target);
			}
		}
	}

	return adj;
}

