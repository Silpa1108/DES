#include "DFS.h"
#include<memory>
//function to check if node is already visited
bool DFS::ContainsNode(std::vector<std::string>& nodes, std::string node)
{
    std::vector<std::string>::const_iterator nodes_it;

    for (nodes_it = nodes.begin();
        nodes_it != nodes.end();
        nodes_it++)
    {
        if ((*nodes_it) == node) return true;
    }

    return false;
}
DFS::DFS()
{}

// Algorithm to recursively search Digraph for paths
void DFS::DepthFirst(Digraph* Digraph,
    std::vector<std::string>& visited,
    std::string end, System* system)
{

    std::string back = visited.back();    //reference to last element of visited nodes
    //storage for the adjacent nodes
    std::vector< std::string > adjNode = Digraph->GetAdjNodeIDs(back);




    // Examine the neighbouring nodes
    for (std::vector<std::string>::iterator node_it = adjNode.begin();
        node_it != adjNode.end();
        node_it++)
    {
        std::string node = (*node_it);

        if (ContainsNode(visited, node)) continue;  //check if node is in visited list

        if (node == end)							//check if the node has further leading nodes
        {
            visited.push_back(*node_it);

            int hops = (int)visited.size();
            //print visited node from visited vector
            for (int i = 0; i < hops; i++)
            {
                //iterate through components list obtained after parsing xml files

                for (std::shared_ptr<Component> const& ptr : system->components)
                {
                    /*check if name of visited node is same as component name
                    pointed by iterator node_it*/
                    if (ptr->name == visited[i])
                    {
                        path.push_back((ptr));//path stores individual path from source to destinations

                    }


                }

            }

            int n = (int)visited.size() - 1;
            //delete visited node with no adjacent nodes
            visited.erase(visited.begin() + n);
            //findpath stores all possible paths from source to destination in 2d format
            system->initial_pathsetlist.push_back(path);//pathset list original and no changes are made on this list
            system->working_pathsetlist.push_back(path);//copy of initial pathset list which is used in engine to work on
            path.clear();
            break;
        }
    }



    for (std::vector<std::string>::iterator node_it = adjNode.begin();
        node_it != adjNode.end();
        node_it++)
    {
        std::string node = (*node_it);

        if (ContainsNode(visited, node) || node == end)

            continue;

        visited.push_back(node);
        //recurssion of DFS
        DepthFirst(Digraph, visited, end, system);

        int n = (int)visited.size() - 1;
        visited.erase(visited.begin() + n);


    }


}
//Depth first search algorithm:Create a recursive function that takes the index of the nodeand a visited array.
//Put  the current node in  visited list and print the node.
//Traverse all the adjacentand unmarked nodesand call the recursive function with the index of the adjacent node.