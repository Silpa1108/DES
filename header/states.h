#ifndef STATE
#define STATE
#include "event.h"
/*The State class is an abstract base class with several
child classes that represent specific states in the system, such as "transient" or "absorbing" or "hidden" states.*/
class State
{
public:
	std::string type;
	std::string name;
	std::string functioning;
	double initial_probability = 0;


	const  std::string& get_statetype() const { return type; }
	const  std::string& get_statename() const { return name; }
	const  std::string& get_functioning() const { return functioning; }
	const  double& get_initialProbability() const { return initial_probability; }

	State(const std::string& type, const std::string& name, const std::string& functioning, const double& initial_probability);
	virtual ~State() {}
};
// Different derived states from base state class:transient,absorbing,hidden state
class TransientState : public State
{

public:
	TransientState(std::string name, std::string functioning, double initial_probability);
};

class AbsorbingState : public State
{

public:
	AbsorbingState(std::string name, std::string functioning, double initial_probability);
};

class HiddenState : public State
{

public:
	HiddenState(std::string name, std::string functioning, double initial_probability);
};


#endif /* STATE */
