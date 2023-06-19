#include "engine.h"
#include"Header.h"
#include "pes.h"
#include"Calculate.h"
#include"StateSpaceOptimization.h"
Engine::Engine(System* system)
{
	//system simulation set to 0 at begining of each simulation.
	system->Simulation_time = 0;
	//reset state of components for next simulation
	reset_component_states(system);
	//state spave explosion mitigitation technique:path reduction to remove path with non functioning and absorbed components
	StateSpaceOptimization O;
	O.initial_path_reduction(system);
	//instantiate the pes with initial events.All the components in system are considered at once here to fill the pes
	pes.pending_events = instantiate_system(*system);
	std::list< std::list<std::shared_ptr<Component>>>::iterator row;//for iterating through pathsets and row is each pathset in the pathset list
	std::vector<std::pair< std::shared_ptr<Component>, Event>>::iterator itr;//iterator for pending eventset.

	//set the system functioning state before simulation starts and going through the pathsets
	std::list<std::shared_ptr<Component>>::iterator r;
	if (check_if_system_functioning(*system))//checks if there is functioning pathset in the list:working_pathsetlist
	{
		system->functioning = "1";
	}

	else
	{
		system->functioning = "0";

	}

	while (system->Simulation_time < system->mission_time && check_non_absorbed_path_presence(*system))
	{
		row = system->working_pathsetlist.begin();
		while (row != system->working_pathsetlist.end() && system->Simulation_time < system->mission_time)//looping through the pathsets until end is reached
		{
			system->knownpathset = *row;

			if (system->functioning == "1")//if system is in functioning mode then make sure that the selected path is also functioning or get another functioning path
			{
				if (check_functioning_path(*row) == false)
				{
					row = std::find_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return check_functioning_path(r); });
				}

			}
			system->knownpathset = *row;

			//get the event with smallest execution time  from pes 
			//combines a Component object and an Event object as pair into componet_event object
			//Component_Event.second is the event object and Component_Event.first is componnet object
			std::pair<std::shared_ptr<Component>, Event>Component_Event = pes.get_next_event(pes.pending_events);

			//system->previous_time= system->Simulation_time;
			Component_Event.second.previous_time = system->Simulation_time;
			system->Simulation_time = Component_Event.second.execution_time;


			std::string current_state;
			current_state = Component_Event.first->getstate();
			std::tuple<std::string, std::string, double, double>p;
			//set new state for component in execute event method
			if (system->Simulation_time < system->mission_time)
			{
				execute_event(&Component_Event);

			}
			else if (system->Simulation_time > system->mission_time)
			{//system execution time is set for the system sojourn data information
				system->execution_time = system->mission_time;
				break;// break from the loop when mission time is reached
			}




			/*check if the component considered belongs to the known pathset and inside the block different conditions for
			component is considered
			I.e (functioning,non absorbed)(functioning,absorbed)(non functioning ,absorbed)and(non functioning ,non absorbed)
			*/
			//check if the component responsible for the excuted event is present in the known_pathset .
			// For ease of filling the component sojourn data, the component time is updated here.
			bool component_exist = (std::find(row->begin(), row->end(), Component_Event.first) != row->end());

			if (component_exist == true)
			{


				//component in functioning and not absorbed state:here clean pes ,update pes and continue simulation with the same pathset

				if (check_functioning_component(Component_Event.first) && !check_absorbed_component(Component_Event.first))
				{	//check if non functioning system goes back to functioning state at this point(for system sojourn data filling)
					if (check_functioning_path(*row) && system->functioning == "0")
					{

						system->pathfunctioning = "1";
						system->functioning = system->pathfunctioning;
						system->previous_time = system->Simulation_time;

					}
					Component_Event.first->functioning = "1";
					//clean the pes
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);
					//set component execution time
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					//make tuple for componnet sojourn information 
					p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
					Component_Event.first->state_sojourn_info.push_back(p);
					Component_Event.first->previous_time = Component_Event.first->execution_time;
					//update pes and add new events based on new state of component
					pes.pending_events = pes.update_pes(Component_Event, system->Simulation_time);
				}
				//functioning but absorbed component.
				else if (check_functioning_component(Component_Event.first) && check_absorbed_component(Component_Event.first))
				{	//just clean the pes but no new events are added here.
					Component_Event.first->failed_absorbed_count++;
					if (check_functioning_path(*row) && system->functioning == "0" && Component_Event.first->failed_absorbed_count == 1)//check if non functioning system goes back to functioning state at this point
					{

						system->pathfunctioning = "1";
						system->functioning = system->pathfunctioning;
						system->previous_time = system->Simulation_time;

					}
					Component_Event.first->functioning = "1";
					Component_Event.first->absorbed = "1";
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);
					//component remains in that state through out the simulatiuon untill mission time.
					//update_pes not required as no new event occurs.
					if (Component_Event.first->failed_absorbed_count == 1 && Component_Event.first->absorbed == "1")
					{

						p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);
						Component_Event.first->previous_time = Component_Event.first->execution_time;
						Component_Event.first->execution_time = system->mission_time;
						p = std::make_tuple(Component_Event.first->state, Component_Event.first->name, Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);

					}
					break;

				}
				//Component not functiong and is in absorbed state :pathset list is modified by deleting pathset having this particular component
				else if (!check_functioning_component(Component_Event.first) && check_absorbed_component(Component_Event.first))
				{
					Component_Event.first->failed_absorbed_count++;
					Component_Event.first->functioning = "0";
					Component_Event.first->absorbed = "1";
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					if (Component_Event.first->failed_absorbed_count == 1 && Component_Event.first->absorbed == "1")
					{

						p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);
						Component_Event.first->previous_time = Component_Event.first->execution_time;
						Component_Event.first->execution_time = system->mission_time;
						p = std::make_tuple(Component_Event.first->state, Component_Event.first->name, Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);

					}
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);
					std::list<std::shared_ptr<Component>>::iterator r;
					std::list< std::list<std::shared_ptr<Component>>>::iterator path;
					//remove the pathsets containing this component from pathset list for rest of the simulation
					path = std::remove_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (std::find(r.begin(), r.end(), Component_Event.first) != r.end()); });

					for (std::list< std::list<std::shared_ptr<Component>>>::iterator p = system->working_pathsetlist.begin();
						p != path; ++p)
					{
						system->new_pathsetlist.push_back(*p);

					}

					system->working_pathsetlist.clear();
					//get the new pathset list into working_pathsetlist
					system->working_pathsetlist.assign(system->new_pathsetlist.begin(), system->new_pathsetlist.end());
					system->new_pathsetlist.clear();

					//get a functioning pathset from pathset list
					path = std::find_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (check_functioning_path(r)); });
					if (path != system->working_pathsetlist.end())
						row = path;
					else
						break;  //break from inner loop if no  functioning path is found

				}
				//Component not functioning and not absorbed .Here we get the next path without this non functioning component
				else if (!check_functioning_component(Component_Event.first) && !check_absorbed_component(Component_Event.first))
				{

					Component_Event.first->functioning = "0";
					//clean pes
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);

					Component_Event.first->execution_time = Component_Event.second.execution_time;

					p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
					Component_Event.first->state_sojourn_info.push_back(p);
					Component_Event.first->previous_time = Component_Event.first->execution_time;
					pes.pending_events = pes.update_pes(Component_Event, system->Simulation_time);
					//get next functioning  path without this particular non functioning component(but pathset list remains same)
					std::list<std::shared_ptr<Component>>::iterator r;
					std::list< std::list<std::shared_ptr<Component>>>::iterator path;
					path = std::find_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (check_functioning_path(r) && !(std::find(r.begin(), r.end(), Component_Event.first) != r.end())); });
					if (path != system->working_pathsetlist.end())
						row = path;
					else
						break;		//break if no such functioning path is found

				}

			}
			else if (component_exist == false)//component is not present in the considered pathset
			{
				//component in functioning and not absorbed state
				if (check_functioning_component(Component_Event.first) && !check_absorbed_component(Component_Event.first))
				{

					Component_Event.first->functioning = "1";
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);

					Component_Event.first->execution_time = Component_Event.second.execution_time;
					p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
					Component_Event.first->state_sojourn_info.push_back(p);
					Component_Event.first->previous_time = Component_Event.first->execution_time;
					pes.pending_events = pes.update_pes(Component_Event, system->Simulation_time);
				}
				//component not functiong and absorbed
				if (!check_functioning_component(Component_Event.first) && check_absorbed_component(Component_Event.first))
				{
					Component_Event.first->failed_absorbed_count++;
					Component_Event.first->functioning = "0";
					Component_Event.first->absorbed = "1";
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					//component remains in this state till mission time is reached
					if (Component_Event.first->failed_absorbed_count == 1 && Component_Event.first->absorbed == "1")
					{
						p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);

						Component_Event.first->previous_time = Component_Event.first->execution_time;
						Component_Event.first->execution_time = system->mission_time;
						p = std::make_tuple(Component_Event.first->state, Component_Event.first->name, Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);

					}
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);

					std::list<std::shared_ptr<Component>>::iterator r;
					std::list< std::list<std::shared_ptr<Component>>>::iterator path;

					//remove pathsets from the list of pathset having this component for rest of the simulation
					path = std::remove_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (std::find(r.begin(), r.end(), Component_Event.first) != r.end()); });

					for (std::list< std::list<std::shared_ptr<Component>>>::iterator p = system->working_pathsetlist.begin();
						p != path; ++p)
					{
						system->new_pathsetlist.push_back(*p);

					}


					system->working_pathsetlist.clear();
					//here new pathset list is created but considered known pathset remains the same.
					system->working_pathsetlist.assign(system->new_pathsetlist.begin(), system->new_pathsetlist.end());
					system->new_pathsetlist.clear();
					path = std::find_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (system->knownpathset == r); });
					row = path;

				}
				//componnet functioning and is absorbed.
				else if (check_functioning_component(Component_Event.first) && check_absorbed_component(Component_Event.first))
				{
					Component_Event.first->absorbed = "1";
					Component_Event.first->failed_absorbed_count++;
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					if (Component_Event.first->failed_absorbed_count == 1 && Component_Event.first->absorbed == "1")
					{
						p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);
						Component_Event.first->previous_time = Component_Event.first->execution_time;
						Component_Event.first->execution_time = system->mission_time;
						p = std::make_tuple(Component_Event.first->state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
						Component_Event.first->state_sojourn_info.push_back(p);

					}

				}
				//component non functiong and not absorbed
				else if (!check_functioning_component(Component_Event.first) && !check_absorbed_component(Component_Event.first))
				{

					Component_Event.first->functioning = "0";
					Component_Event.first->execution_time = Component_Event.second.execution_time;
					pes.pending_events = pes.clean_pes(pes.pending_events, Component_Event);

					p = std::make_tuple(current_state, Component_Event.second.get_id(), Component_Event.first->previous_time, Component_Event.first->execution_time);
					Component_Event.first->state_sojourn_info.push_back(p);
					Component_Event.first->previous_time = Component_Event.first->execution_time;
					pes.pending_events = pes.update_pes(Component_Event, system->Simulation_time);
				}

			}
		}

		//system from functioning goes to non functioning state  after checking all paths
		if (check_if_system_functioning(*system) == false && system->functioning == "1")
		{
			system->execution_time = system->Simulation_time;
			//system goes to nonfunctioning state when no functioning path is present and comes out of inner loop
			system->functioning = "0";
			system->system_uptime_intervals.push_back(std::make_pair(system->previous_time, system->execution_time));
		}
		else if (check_if_system_functioning(*system) == true && system->functioning == "1")//this condition is reached only if simulationtime is greater than mission time
		{


			//system goes to nonfunctioning state when no functioning path is present and comes out of inner loop
			system->functioning = "1";
			system->system_uptime_intervals.push_back(std::make_pair(system->previous_time, system->execution_time));
		}
		else if (check_if_system_functioning(*system) == false && system->functioning == "0")
		{

			system->functioning = "0";

		}


	}

	//updates their componnet state sojourn information at the end of engine.
	for (std::shared_ptr<Component> const& ptr : system->components)
	{
		if (ptr->type != "node")
		{/*the component's state sojourn information is empty, means that the component has just started operating in its initial state.
		and reamains in that initial state till the system's mission time.*/
			if (ptr->state_sojourn_info.empty())
			{
				ptr->state_sojourn_info.push_back(std::make_tuple(ptr->state, ptr->name, ptr->execution_time, system->mission_time));
			}
			else if (std::get<0>(ptr->state_sojourn_info.back()) == ptr->state && std::get<3>(ptr->state_sojourn_info.back()) != system->mission_time)
			{
				ptr->state_sojourn_info.push_back(std::make_tuple(ptr->state, ptr->name, ptr->execution_time, system->mission_time));

			}
			else if (std::get<0>(ptr->state_sojourn_info.back()) != ptr->state && std::get<3>(ptr->state_sojourn_info.back()) != system->mission_time)
			{
				ptr->state_sojourn_info.push_back(std::make_tuple(ptr->state, ptr->name, ptr->execution_time, system->mission_time));

			}

		}

	}


}

//defining the overloaded bool operator "==  "
bool operator==(const std::shared_ptr<Component> t1, const   Component& t2) {
	return (t1->name == t2.name);
}

//method for executing events based on type 
void Engine::execute_event(std::pair<std::shared_ptr<Component>, Event>* Component_Event)
{

	if (Component_Event->second.type == "discrete")
	{
		int count = 1;
		DiscreteEvent d;
		//make the state change in component based on the condition and generate new discrete event
		pes.new_discrete_event = d.execute(Component_Event);
		//add new discrete event to pes
		pes.add_event(pes.new_discrete_event.first, pes.new_discrete_event.second);
	}
	else if (Component_Event->second.type != "discrete")
	{
		if (Component_Event->second.type == "exponential")
		{
			ExponentialEvent e;
			e.execute(Component_Event);	//execute exponential event 

		}
		else if (Component_Event->second.type == "uniform")
		{
			UniformEvent u;
			u.execute(Component_Event);			//execute uniform event
		}
		else if (Component_Event->second.type == "normal")
		{
			NormalEvent u;
			u.execute(Component_Event);			//execute uniform event
		}
		else if (Component_Event->second.type == "mixed_distribution")
		{
			MixedEvent m;
			m.execute(Component_Event);			//execute uniform event
		}

	}

}


//function to get initial events in pes from all components present in the system
std::vector<std::pair< std::shared_ptr<Component>, Event>> Engine::instantiate_system(System system)
{
	std::list<std::shared_ptr<Component>>::iterator col;
	std::list<Event>::iterator event;
	for (std::shared_ptr<Component> const& col : system.components) //iterate through each components in the system 
	{
		if (col->type != "node")
		{
			for (event = col->events.begin(); event != col->events.end(); event++)
			{
				event->execution_time = 0;
				if (event->type == "discrete")
				{
					DiscreteEvent* f = new DiscreteEvent(event->id, event->type, event->source, event->target, event->get_interval());
					double now = f->get_event_time(event->get_interval());
					f->set_executiontime(now);
					pes.add_event(col, *f);
					delete f;

				}
				else if (event->type != "discrete" && event->source == col->state)
				{
					if (event->type == "exponential")
					{
						ExponentialEvent* f = new ExponentialEvent(event->id, event->type, event->source, event->target, event->get_rate());
						double now = f->get_event_time(event->get_rate(), col->Simulation_time);
						f->set_executiontime(now);
						pes.add_event(col, *f);
						delete f;
					}
					else if (event->type == "uniform")
					{
						UniformEvent* f = new UniformEvent(event->id, event->type, event->source, event->target, event->get_lower(), event->get_upper());
						double now = f->get_event_time(event->get_lower(), event->get_upper(), col->Simulation_time);
						f->set_executiontime(now);
						pes.add_event(col, *f);
						delete f;
					}
					else if (event->type == "normal")
					{
						NormalEvent* f = new NormalEvent(event->id, event->type, event->source, event->target, event->get_mu(), event->get_sigma());
						double now = f->get_event_time(event->get_mu(), event->get_sigma(), col->Simulation_time);
						f->set_executiontime(now);
						pes.add_event(col, *f);
						delete f;
					}
					else if (event->type == "mixed_distribution")
					{
						MixedEvent* f = new MixedEvent(event->id, event->type, event->source, event->target, event->dist_pt);
						double now = f->get_event_time(event->dist_pt, col->Simulation_time);
						f->set_executiontime(now);
						pes.add_event(col, *f);
						delete f;
					}
				}
			}
		}

	}


	return pes.pending_events;
}


//check if component is in absorbed state:returns true if absorbed
bool Engine::check_absorbed_component(std::shared_ptr<Component> c) {
	bool found;

	found = false;
	for (auto& state1 : c->states) {


		if (state1.type == "absorbing" && c->state == state1.name)
		{
			found = true;
			break;

		}
	}

	return found;
}

//check if component is functioning state::return true if functioning and false if not functioning
bool Engine::check_functioning_component(std::shared_ptr<Component> c) {
	bool function_found;

	function_found = true;
	for (auto& state1 : c->states) {


		if (state1.functioning == "0" && c->state == state1.name)
		{
			function_found = false;
			break;
			//c->functioning = "0";
		}
	}

	return function_found;
}

//check for the presence of non absorbed path for the system to come back to functioning state
bool Engine::check_non_absorbed_path_presence(System system)
{
	bool found = true;
	std::list< std::list<std::shared_ptr<Component> >>::iterator row;//for iterating through pathsets
	if (system.working_pathsetlist.size() == 0) { found = false; }
	else {
		for (row = system.working_pathsetlist.begin(); row != system.working_pathsetlist.end(); row++) //iterate through each system path
		{
			found = true;
			for (auto& comp : *row)
			{
				if (comp->type == "component")
				{
					for (auto& state1 : comp->states) {


						if ((state1.type == "absorbing" && comp->state == state1.name))
						{

							found = false;
						}

					}
				}
			}
			if (found == true)break;
		}
	}
	return found;
}
//check if system is functioning ,I.e there exists atleast on fully functioning path
bool Engine::check_if_system_functioning(System system)
{
	bool found = true;
	//if no pathsets found then system not functioning
	if (system.working_pathsetlist.size() == 0)
	{
		found = false;

	}
	else
	{
		std::list< std::list<std::shared_ptr<Component> >>::iterator row;//for iterating through pathsets
		for (row = system.working_pathsetlist.begin(); row != system.working_pathsetlist.end(); row++) //iterate through each system path
		{
			found = true;
			for (auto& comp : *row)
			{
				for (auto& state1 : comp->states) {


					if ((state1.functioning == "0" && comp->state == state1.name))
					{

						found = false;

					}

				}
			}
			if (found == true)	break;
		}
	}
	return found;
}
//check if any component in pathset is in non functioning state then that pathset is non functioning and  return false 
bool Engine::check_functioning_path(std::list<std::shared_ptr<Component>>components) {
	bool path_found;
	path_found = true;
	for (auto& comp : components)
	{
		for (auto& state1 : comp->states) {


			if (state1.functioning == "0" && comp->state == state1.name)
			{
				//comp->functioning= "0";
				path_found = false;
				break;
			}

		}
	}
	return path_found;
}
//reset the system components
void Engine::reset_component_states(System* system)
{

	for (auto& comp : system->components)
	{
		if (comp->type == "Component")

		{
			State initial_state = random_initial_state(comp->states);//get the initial state of the component 

			comp->state = initial_state.get_statename();
			comp->uptime = 0;
			comp->downtime = 0;
			comp->execution_time = 0;
			comp->Simulation_time = 0;
			comp->previous_time = 0;
			comp->failed_absorbed_count = 0;

		}
	}



	system->execution_time = 0;
	system->previous_time = 0;
	system->working_pathsetlist = system->initial_pathsetlist;
}
//function "random_initial_state "takes a list of states from component as input and returns a randomly selected initial state based on their initial probabilities.
State Engine::random_initial_state(std::list<State>  states)
{
	std::vector<double>  probability;

	std::vector<double> prob_sum(states.size(), 0);
	// Store initial probabilities of component in probability vector
	for (auto& state : states)
	{
		probability.push_back(state.initial_probability);
	}


	// construct a sum vector from the given probabilities
	// prob_sum vector has sum of all probability[] element added  cumilatively
	prob_sum[0] = probability[0];
	for (int i = 1; i < states.size(); i++) {
		prob_sum[i] = prob_sum[i - 1] + probability[i];
	}

	// generate a random integer q from 0 to 1 and check where it lies in prob_sum[]
	Event e;
	double q = e.ran();//use the random number generator from class event

	// based on the comparison result, return the corresponding
	// element from the nums vector
	// Iterate through states to find the one that matches with  q
	std::list<State>::iterator it = states.begin();


	if (q <= prob_sum[0])
	{  	//the first index
		return *it;
	}

	for (int i = 1; i < states.size(); i++)
	{
		if (q > prob_sum[i - 1] && q <= prob_sum[i])
		{ // Move iterator to the current state and exit loop
			advance(it, i);
			break;

		}
	}
	// Return the selected initial state
	return *it;
}
