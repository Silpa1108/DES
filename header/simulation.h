#ifndef MCSIMULATION_H
#define MCSIMULATION_H
#include"system.h"
#include"engine.h"
//class for doing montecarlo simulation of engine
class MCSimulation
{


public:
	void run(System* system);
	MCSimulation() {}
	~MCSimulation() {}
};



#endif /* SIMULATION_H*/
