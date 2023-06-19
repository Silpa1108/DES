// DiscreteEventSimulation.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include"Calculate.h"
#include"simulation.h"
#include"StateSpaceOptimization.h"
#include"statistics.h"
#include"DFS.h"
#include"json_writer.h"

#include<thread>
#include<iostream>
#include <stdexcept>
// DES_Silpa.cpp : This file contains the 'main' function. Program execution begins and ends there.
int main()
{
	std::string file; //file variable

	std::cout << "Enter the directory and the name of the file in this format ->C:/Users/silpa/source/repos/DiscreteEventSimulation/input/input_test.xml \n" << endl;
	std::cin >> file; //input the file
	Parser obj; //instantiate Parser Class
	ParserXML xml; //instantiate ParserXML Class
	ParserJSON json; //instantiate ParserJSON Class
	System system = System(); //instantiate System Class

	int store = obj.read(file); // call the class and store the return value from the type of file checked


	// if return value is 1 then XML file is read by the ParserXML class
	if (store == 1)
	{
		std::cout << "Get information about system and components by parsing the file\n ";
		system = xml.import_xmlfile(file); //read XML file and store in system object	

	}
	//if return value is 2 then JSON file is read by the ParserJSON class
	else if (store == 2)
	{
		json.import_jsonfile(file); //read JSON file 

	}
	else {
		std::cerr << ": Input file does not exist.\n";
		exit(1);
	}
	try
	{
		Digraph g;      //directed graph implementation using digraph class
	//get all paths from xml file and add path  to directed graph object
		for (auto& l : system.paths)
		{
			g.AddPath(l.get_SourceNode(), l.get_TargetNode());
		}
		//start and end node is defined here.
		Component start = Component("node", "start");//start node
		Component end = Component("node", "end");//end node

		std::string target = end.name;
		std::string source = start.name;
		//storage for visited components :to be used in DFS class to get all paths between start and end node.
		std::vector<std::string> visited;
		//push start node into visited list
		visited.push_back(source);//visited has source node now.

		DFS* d = new DFS(); // instantiate Depthfirstsearch class

		//Depthfirst search algorithm (recursive method)to get all pathsets between start and end node
		//Components in each path gets stored in system class member initial_patsetlist and working_pathsetlist inside this function

		d->DepthFirst(&g, visited, target, &system);
		delete d;


		std::string optimize_choice;
		std::cout << "Do you want to optimize the system? (y/n): ";
		std::cin >> optimize_choice;
		//check if state space explosion mitigation has to be applied or not
		if (optimize_choice == "y")
		{
			std::cout << "Choose an optimization method for state space mitigation:\n";
			std::cout << "1. State-transition based optimization\n";
			std::cout << "2. Random walk optimization\n";
			int optimization_method;
			std::cin >> optimization_method;
			StateSpaceOptimization O;
			switch (optimization_method) {
			case 1:
				//apply state_transition reduction technique on system object
				system = O.state_reduction(&system);
				break;
			case 2:
				//apply random_walk reduction technique on system object
				system = O.Random_walk_partial_search(&system);
				break;
			default:
				std::cerr << "Error: Invalid optimization method.\n";
				return 1;
			}
		}
		System sys1 = system;
		MCSimulation sim;
		sim.run(&sys1);//for montecarlo simulation MCS


		//for doing statistics calculations and obtain performance metrics over MCS
		Statistics s;
		sys1 = s.get_statistics(sys1);
		//for writing results into json output file
		json_writer jw;
		jw.write_into_jsonfile(sys1);

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}


	return 0;
}

/// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

