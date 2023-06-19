#ifndef DFS_H
#define DFS_H
#include"Header.h"
#include"Digraph.h"
#include"xmlparser.h"
#include"engine.h"

/*DFS class responsible for implementing the depth first search algorithm on system object and finds all possible
paths present in the system*/

class DFS
{
public:
	int count = 0;
	DFS();
	System system;
	double Available_time = 0;

	bool ContainsNode(std::vector<std::string>& nodes, std::string node);

	void DepthFirst(Digraph* Digraph,
		std::vector<std::string>& visited,
		std::string end, System* system);
	std::list<std::shared_ptr<Component>>path;

};

#endif /* DFS */
