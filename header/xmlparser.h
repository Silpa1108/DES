#ifndef XMLPARSER
#define XMLPARSER

#include"tinyxml2.h"
#include"Discrete.h"
#include"Exponential.h"
#include"Uniform.h"
#include"Normal.h"
#include"MixedDist.h"
#include"system.h"
#include"path.h"

using namespace std;
using namespace tinyxml2;
//class parser for parsing the system model from the the input file
class Parser
{
public:

	int read(std::string file)
	{
		//check if the file is XML
		if (file.substr(file.find_last_of(".") + 1) == "xml")
		{
			std::cout << "Yes its an xml" << std::endl;
			return 1;
		}
		//check if file is JSON:for future implementation
		else if (file.substr(file.find_last_of(".") + 1) == "json")
		{
			std::cout << "Yes its an JSON" << std::endl;
			return 2;
		}
		else
		{
			std::cout << "Error";
			return 0;
		}
	}
};
//for future implementation of input file in JSON format
class ParserJSON {
public:
	string file;
	void import_jsonfile(string file) {
		cout << "In class Parser JSON \n" << file << endl;
	}
};
//class for parsing input file in .xml format using tinyxml parser
class ParserXML
{
public:
	const char* file_name;
	const char* title;					//get the title of xml file
	double mission_time = 0;			//mission time
	const char* comp;					// to get component name
	const char* typ;					// to get component type
	const char* name;					//to get state name
	const char* ty;
	const char* sty;
	const char* tr;
	const char* systr;
	const char* sr;
	const char* n;
	const char* node_name;					//node name	
	const char* id;
	const char* source;
	const char* target;
	const char* src;					//source component 
	const char* dst;
	const char* type;					//transition type
	const char* st;						//state type
	const char* f;
	const char* r_name;
	const char* at_name;
	const char* sys_r_name;
	const char* sys_at_name;
	const char* p;
	const char* s;
	//std::string missiontime;
	const char* missiontime;
	const char* sysre;
	const char* syty;
	double rate = 0;
	double initial_probability = 0;
	int present_state = 0;
	double va = 0;
	double sva = 0;
	double sysva = 0;
	int time = 0;
	int n_n = 0;						//no of nodes
	int s_n = 0;						//no of states in each component
	int t_n = 0;						//no of events in each component
	int c_n = 0;						//no of components
	double r_t = 0;
	double at_t = 0;
	double sys_r_t = 0;
	double sys_at_t = 0;
	double interval = 0;
	double lower = 0;
	double upper = 0;
	double mu = 0;
	double sigma = 0;
	System system;
	std::vector<std::string> parameter_list;				//system characteristics considered
	//std::vector<Node>node_list;
	std::list<Calculate>calculate_list;
	std::list<Reward_parameter>reward_parameter_list;
	std::list<State>states;					//list of Class State-states
	std::list<Event> events;				//list of Class Event-events
	std::list<Component>components;			//list of components
	std::list<Path>paths;
	ParserXML() = default;
	//ParserXML(std::list<Component>& components, std::list<Path>&paths);
	System import_xmlfile(std::string);


};



#endif /* XMLPARSER */
