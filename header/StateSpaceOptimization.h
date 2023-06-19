#ifndef STATESPACEOPTIMIZATION_H
#define STATESPACEOPTIMIZATION_H
#include"system.h"
#include"Header.h"
#include"engine.h"
//this class has functions to implement state space explosion mitigation techniques
class StateSpaceOptimization
{

public:
	System sys;
	//function to implement path reduction technique
	System initial_path_reduction(System*);
	/*function to generate a new transition list for the component based on a random walk approach, where transitions are selected at random
	from the set of available transitions
for each state of each component in the system.*/
	System Random_walk_partial_search(System*);
	//function to do transition and state reduction to remove reduntant state and transition and thus reduce size of state space
	System state_reduction(System* system);

private:
	Event  merge_states(Event, Event);
	std::vector<std::pair<double, double>>get_distribution_points(Event, Event);
	void exp_expdistribution(double, double);
	void exp_uniformdistribution(double, double, double);
	void exp_normaldistribution(double, double, double);
	void uniform_uniformdistribution(double, double, double, double);
	std::pair<double, double>dist_points[1000];
	std::vector<std::pair<double, double>>distribution_points;
};

#endif