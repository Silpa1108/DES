#ifndef COMPONENT_H
#define COMPONENT_H
#include"Header.h"
#include"states.h"
#include"path.h"
#include"Calculate.h"


class Component
{
public:
	std::string name;
	std::string type;		//type can be node or component itself
	std::list<State>states;
	std::list<Event>events;
	std::list<Calculate>component_calculate;
	std::list<Reward_parameter>component_rewards;

	double Simulation_time = 0;
	double execution_time = 0;
	double previous_time = 0;
	double uptime = 0;
	double downtime = 0;
	double availability = 0;
	double mission_time = 0;

	int failed_absorbed_count = 0;
	std::string state = "0";
	std::string functioning = "1";
	std::string absorbed = "0";
	std::vector<std::vector<std::tuple<std::string, std::string, double, double>>>state_sojourn_completedata;
	std::vector<std::tuple<std::string, std::string, double, double>>state_sojourn_info;
	Component() = default;
	~Component() {};
	const std::string getname() const { return name; }
	const std::string gettype() const { return type; }
	const std::string getstate() const { return state; }

	void set_state(std::string s) { state = s; }
	Component(const std::string& type, const std::string& name);
	Component(const std::string& type, const std::string& name, const std::list<State>& states, const std::list<Event>& events, const std::list<Calculate>& calculate);

};



#endif /* COMPONENT */
