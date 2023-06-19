#ifndef ENGINE_H
#define ENGINE_H
#include"system.h"
#include"pes.h"
/*Class Engine acts as Event manger and responsible for execution of events */
class Engine
{
private:
	double Simulation_time = 0;
	PES pes;
	//instantiate the the pes with events
	std::vector<std::pair<std::shared_ptr<Component>, Event>> instantiate_system(System system);
	bool check_if_system_functioning(System system);//check if atleast one fully functioning path exists
	bool check_functioning_path(std::list<std::shared_ptr<Component>>components);//check if the considered path has all its members in functioning stae
	bool check_functioning_component(std::shared_ptr<Component> c);//check if component is functioning or not
	bool check_absorbed_component(std::shared_ptr<Component> c);//check if component is in absorbed state
	bool check_non_absorbed_path_presence(System system);//check for presence of non absorbed path
	void execute_event(std::pair<std::shared_ptr<Component>, Event>*);
	//to reset the system components state and other variable (moved to simulation class)
	void reset_component_states(System* system);
	State random_initial_state(std::list<State>);
public:
	Engine(System* system);

	~Engine() { }



};


#endif /* ENGINE */
