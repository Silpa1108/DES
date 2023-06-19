#ifndef SYSTEM_H
#define SYSTEM_H
#include"event.h"
#include"component.h"
#include<list>
#include<memory>
/*The system model read from an XML file is realised as a System class.
The System class has important data structures such as : a list of component objects, a list of Path sets
and a list of system sojourn data statistics to keep track system functioning time intervals
from each simulation.*/
class System
{
public:double mission_time = 0;
	  double Simulation_time = 0;
	  double execution_time = 0;
	  double previous_time = 0;
	  double uptime = 0;
	  double downtime = 0;
	  double availability = 0;

	  std::string functioning = "1";
	  std::string pathfunctioning = "1";

	  std::list<std::shared_ptr<Component>>knownpathset;//the currently considered pathset is stored here
	  std::list<std::shared_ptr<Component>>components;//list of all components in system
	  std::vector< std::vector<std::pair<double, double>>>monte_carlo_system_uptime_interval;
	  std::vector<std::pair<double, double>> system_uptime_intervals;
	  std::list<Path>paths;
	  std::list<std::list<std::shared_ptr<Component>>>initial_pathsetlist;//2d vector to store all pathsets obtained from DFS algorithm
	  std::list<std::list<std::shared_ptr<Component>>>working_pathsetlist;
	  std::list<std::list<std::shared_ptr<Component>>>new_pathsetlist;
	  std::list<Calculate>system_calculation;// the attributes of system to be calculated
	  std::list<Reward_parameter>system_rewards;//to calculate the rewards

	  System() {};
	  ~System() {  }


};



#endif /* SYSTEM */
