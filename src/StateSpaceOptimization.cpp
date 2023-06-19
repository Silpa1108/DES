#include"StateSpaceOptimization.h"

bool stateinitial_probability(State& lhs, State& rhs)
{
	return lhs.initial_probability < rhs.initial_probability;
}
//this function to remove the pathsets from working pathset list which has got 'absorbed' and 'non functioning' components
//works by iterating through the component list and checking whether each component is not functional and is in absorbed state.
System StateSpaceOptimization::initial_path_reduction(System* system)//
{
	for (auto& comp : system->components)
	{
		if (comp->type == "Component")
		{

			bool found;
			found = false;
			for (auto& state1 : comp->states)
			{
				if (state1.type == "absorbing" && state1.functioning == "0" && comp->state == state1.name)
				{
					found = true;//absorbed and non functioning component so do path reduction
					std::list<std::shared_ptr<Component>>::iterator r;
					std::list< std::list<std::shared_ptr<Component>>>::iterator path;
					//remove the pathsets containing this component from pathset list from the system
					path = std::remove_if(system->working_pathsetlist.begin(), system->working_pathsetlist.end(), [&](std::list<std::shared_ptr<Component>>  r) {return (std::find(r.begin(), r.end(), comp) != r.end()); });

					for (std::list< std::list<std::shared_ptr<Component>>>::iterator p = system->working_pathsetlist.begin();
						p != path; ++p)
					{
						system->new_pathsetlist.push_back(*p);

					}


					system->working_pathsetlist.clear();
					//get the new pathset list into working_pathsetlist

					system->working_pathsetlist.assign(system->new_pathsetlist.begin(), system->new_pathsetlist.end());
					system->new_pathsetlist.clear();

				}
			}



		}
	}
	return *system;
}
// function "Random_walk_partial_search" that takes a pointer to a System object and returns the modified or reduced System object. 
//Random walk  always visits just one successor of a current state and generates a new transition list for the component
System StateSpaceOptimization::Random_walk_partial_search(System* system)
{
	for (auto& comp : system->components)
	{// Iterate over each component in the system
		if (comp->type == "Component")

		{
			std::vector<Event> matches;

			std::list<Event> eventlist;
			// Iterate over each state in the component
			for (auto& state : comp->states)
			{

				// Find all events in the component with the same source state as the current state, except for absorbing states save in vector matches
				std::copy_if(comp->events.begin(), comp->events.end(), std::back_inserter(matches), [&](auto r) {return (state.name == r.source && state.type != "absorbing"); });
				// If there is at least one event in matches
				if (matches.size() >= 1)
				{
					std::random_shuffle(matches.begin(), matches.end()); // shuffle the matches vector
					for (int i = 0; i < std::min((size_t)2, matches.size()); i++) { // select 2 random events from the shuffled list
						eventlist.push_back(matches[i]);
					}
					matches.clear();
				}
				// Check if the target state of the absorbing type is included in the event list
				auto s = std::find_if(eventlist.begin(), eventlist.end(), [&](auto r) {return (state.name == r.target && state.type == "absorbing"); });
				// If not included, find all events in the component with the same target state as the current state and of the absorbing type, and add them to the event list
				if (s == eventlist.end())
					std::copy_if(comp->events.begin(), comp->events.end(), std::back_inserter(eventlist), [&](auto r) {return (state.name == r.target && state.type == "absorbing"); });


			}
			// Replace the events in the component with the modified event list
			comp->events.clear();
			comp->events.assign(eventlist.begin(), eventlist.end());
			eventlist.clear();
		}
	}// Return the modified system
	return *system;
}
//function "state_reduction" reduce the size of the state space by removing
//redundant transitions that have similar sources and targets state
System StateSpaceOptimization::state_reduction(System* system)
{
	sys.mission_time = system->mission_time;
	bool check_if_rewarded_state_transition(Component, State, Event);

	for (auto& comp : system->components)
	{
		std::vector<std::pair<State, State>>merged_state_pair;
		if (comp->type == "Component")

		{

			std::list<Event> eventlist1;
			std::list<Event> eventlist2;
			std::list<Event> eventsource_counter;
			std::list<Event>::iterator event = comp->events.begin();
			//nested while loop to compare each pair of events in the list to see if the events can be merged together to form a single event.

			while (event != comp->events.end())
			{

				Event merged = *event;
				Event e;
				std::list<Event>::iterator it = comp->events.begin();

				while (it != comp->events.end())
				{
					int i = 1;
					std::advance(it, i);//iterator advance to next position
					if (it == comp->events.end())break;


					//2 events having same source state are checked to see if the events can be merged
					//event type that are discrete and mixed_distribution(already merged events) are not considered for merging

					if ((it->get_state() == merged.get_state() && it->id != merged.id && it->type != "discrete" && it->type != "mixed_distribution" && merged.type != "discrete" && merged.type != "mixed_distribution"))  //event from same source state 
					{

						std::list<State>::iterator state_to_merge1;
						std::list<State>::iterator state_to_merge2;
						//get the target state of the 2 events considered
						state_to_merge1 = std::find_if(comp->states.begin(), comp->states.end(), [&](State s) {return (s.name == merged.target); });
						state_to_merge2 = std::find_if(comp->states.begin(), comp->states.end(), [&](State s) {return (s.name == it->target); });
						//check if the target states are of same type and functioning mode
						if (state_to_merge1->type == state_to_merge2->type && state_to_merge1->functioning == state_to_merge2->functioning)
						{	//if both target state do not have any rewards then they can be easily merged
							if (!check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && !check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{	//function to merge events
								e = merge_states(merged, *it);

								merged_state_pair.push_back(std::make_pair(*state_to_merge1, *state_to_merge2));

							}
							//event and states with rewards has priority
							//if first state has  rewards and 2nd one has no reward then use first state to to represent both states
							if (check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && !check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								e = merge_states(*it, merged);
								merged_state_pair.push_back(std::make_pair(*state_to_merge2, *state_to_merge1));


							}
							//if first state has no rewards and 2nd one has reward then use second state to to represent both states
							if (!check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								e = merge_states(merged, *it);

								merged_state_pair.push_back(std::make_pair(*state_to_merge1, *state_to_merge2));



							}
							//if both state or transition are rewarded then cannot be merged
							if (check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								break;

							}

							std::copy_if(comp->events.begin(), comp->events.end(), std::back_inserter(eventlist1), [&](auto r) {return (r.id != merged.id && it->id != r.id); });

							eventlist1.push_front(std::move(e));
							break;

						}
					}
					else if (it->get_nextstate() == merged.get_nextstate() && it->id != merged.id && it->type != "discrete" && it->type != "mixed_distribution" && merged.type != "discrete" && merged.type != "mixed_distribution")  //event going to same target state 
					{
						std::list<State>::iterator state_to_merge1;
						std::list<State>::iterator state_to_merge2;
						//get the source state of the 2 events considered
						state_to_merge1 = std::find_if(comp->states.begin(), comp->states.end(), [&](State s) {return (s.name == merged.source); });
						state_to_merge2 = std::find_if(comp->states.begin(), comp->states.end(), [&](State s) {return (s.name == it->source); });
						//check if the source states are of same type and functioning mode
						if (state_to_merge1->type == state_to_merge2->type && state_to_merge1->functioning == state_to_merge2->functioning)
						{	//if both source state do not have any rewards then they can be easily merged
							if (!check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && !check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								e = merge_states(merged, *it);

								merged_state_pair.push_back(std::make_pair(*state_to_merge1, *state_to_merge2));

							}
							//event and states with rewards has priority
							//if first state has  rewards and 2nd one has no reward then use first state to to represent both states
							if (check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && !check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								e = merge_states(*it, merged);
								merged_state_pair.push_back(std::make_pair(*state_to_merge2, *state_to_merge1));


							}
							if (!check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								e = merge_states(merged, *it);

								merged_state_pair.push_back(std::make_pair(*state_to_merge1, *state_to_merge2));



							}
							if (check_if_rewarded_state_transition(*comp, *state_to_merge1, *it) && check_if_rewarded_state_transition(*comp, *state_to_merge2, merged))
							{
								break;

							}
							std::copy_if(comp->events.begin(), comp->events.end(), std::back_inserter(eventlist2), [&](auto r) {return (r.id != merged.id && it->id != r.id); });

							eventlist2.push_front(std::move(e));
							break;


						}
					}
					else
					{

						continue;

					}





				}
				if (eventlist2.size() > 0) {
					comp->events.clear();

					comp->events.assign(eventlist2.begin(), eventlist2.end());

					eventlist2.clear();
					event = comp->events.begin();


				}
				else if (eventlist1.size() > 0) {
					comp->events.clear();

					comp->events.assign(eventlist1.begin(), eventlist1.end());
					eventlist1.clear();
					event = comp->events.begin();

				}

				event++;


			}


		}

		//after merging the events check if all states of component  still exist in the new events list if not means 
		//states got merged during event merging ,so update the initial probability of state according and delete 
		//the state that is not occuring in the new updated event list of component

		if (merged_state_pair.size() > 0)
		{// Iterate over all the states in the component
			std::list<State>::iterator state = comp->states.begin();
			while (state != comp->states.end()) {
				std::list<State>statelist;
				std::list<Event>::iterator its;
				// Find the event/ transitions associated with the selected state
				its = std::find_if(comp->events.begin(), comp->events.end(), [&](Event e) {return (state->name == e.source || state->name == e.target); });
				// If there are no events associated with the state in the new updated event list, it can be removed
				if (its == comp->events.end())
				{
					std::vector<std::pair<State, State>>::iterator itm;
					itm = std::find_if(merged_state_pair.begin(), merged_state_pair.end(), [&](auto e) {return (state->name == e.first.name || state->name == e.second.name); });
					// Update the probabilities of the merged states
					if (itm->first.name == state->name)
						itm->second.initial_probability = itm->first.initial_probability + itm->second.initial_probability;
					if (itm->second.name == state->name)
						itm->first.initial_probability = itm->first.initial_probability + itm->second.initial_probability;
					// Remove the redundant state from the component
					std::copy_if(comp->states.begin(), comp->states.end(), std::back_inserter(statelist), [&](auto r) {return (r.name != state->name); });

					comp->states.clear();
					//new updated state list
					comp->states.assign(statelist.begin(), statelist.end());
					statelist.clear();
					state = comp->states.begin();
				}
				state++;
			}
		}
		merged_state_pair.clear();
	}


	return *system;
}
bool mergepairfunction(std::pair<State, State> i, std::pair<State, State> j) {
	return ((i.first.name == j.first.name && i.second.name == j.second.name) || (i.first.name == j.second.name && i.second.name == j.first.name));
}
//function to merge the two events 
Event StateSpaceOptimization::merge_states(Event it, Event m)
{
	std::list<Event> eventlist;
	Event mixed;
	//if events to be merged are both exponential then resulting event here takes the rate as some of rates of the two events to be merged

	if (it.type != m.type || (it.type == m.type && it.type == "uniform") || (it.type == m.type && m.type == "exponential"))
	{

		Event* e = new Event(it.id, "mixed_distribution", it.source, it.target);
		e->dist_pt = get_distribution_points(it, m);
		mixed = *e;
	}



	return mixed;
}
//function to get cdf distribution poinst of mixed distribution()
std::vector<std::pair<double, double>> StateSpaceOptimization::get_distribution_points(Event it, Event m)
{   // merge exponetial and uniform distribution
	if ((m.type == "exponential" && it.type == "uniform"))
	{
		exp_uniformdistribution(m.rate, it.lower, it.upper);
	}
	else if ((it.type == "exponential" && m.type == "uniform"))
	{
		exp_uniformdistribution(it.rate, m.lower, m.upper);
	}
	else if ((it.type == "exponential" && m.type == "exponential"))
	{
		exp_expdistribution(it.rate, m.rate);
	}
	else if ((m.type == "exponential" && it.type == "normal"))
	{
		exp_normaldistribution(m.rate, it.mu, it.sigma);
	}
	else if ((it.type == "exponential" && m.type == "normal"))
	{
		exp_normaldistribution(it.rate, m.mu, m.sigma);
	}
	else if ((m.type == "uniform" && it.type == "uniform"))
	{
		uniform_uniformdistribution(m.lower, m.upper, it.lower, it.upper);
	}

	return distribution_points;

}

void StateSpaceOptimization::exp_uniformdistribution(double rate, double a, double b)
{

	distribution_points.clear();
	double x[1000], y[1000];
	for (int i = 0; i < 1000; i++)
	{
		x[i] = i * sys.mission_time / 1000.0;
		if (x[i] < b) {
			y[i] = ((1.0 - exp((0 - x[i]) / (1.0 / rate))) + (x[i] - a) / (b - a)) / 2.0;//cdf of exponential and uniform distributions are averged out
		}
		else if (x[i] > b) {
			y[i] = ((1.0 - exp((0 - x[i]) / (1.0 / rate))) + 1.0) / 2.0;
		}

	}
	for (int i = 0; i < 1000; i++)
	{
		distribution_points.push_back({ x[i],y[i] });

	}
}
void StateSpaceOptimization::exp_expdistribution(double rate, double rt)
{

	distribution_points.clear();
	double x[1000], y[1000];
	for (int i = 0; i < 1000; i++)
	{
		x[i] = i * sys.mission_time / 1000.0;

		y[i] = ((1.0 - exp((0 - x[i]) / (1.0 / rate))) + (1.0 - exp((0 - x[i]) / (1.0 / rate)))) / 2.0;//cdf of exponential and exp distributions are averged out



	}
	for (int i = 0; i < 1000; i++)
	{
		distribution_points.push_back({ x[i],y[i] });

	}
}
void StateSpaceOptimization::exp_normaldistribution(double rate, double mu, double s)
{
	distribution_points.clear();
	double phi(double, double, double);
	double x[1000], y[1000];
	for (int i = 0; i < 1000; i++)
	{
		int sign = 1;
		x[i] = i * sys.mission_time / 1000.0;
		//x[i] = i * 10.0;

		//phi for cdf points of normal distribution points
		y[i] = (phi(x[i], mu, s) + (1.0 - exp((0 - x[i]) / (1.0 / rate)))) / 2.0;


	}
	for (int i = 0; i < 1000; i++)
	{
		distribution_points.push_back({ x[i],y[i] });
		//std::cout << y[i] << "\t" << x[i] << "\n";
	}
}
void StateSpaceOptimization::uniform_uniformdistribution(double a, double b, double c, double d)
{
	distribution_points.clear();
	double x[1000], y[1000];
	for (int i = 0; i < 1000; i++)
	{
		x[i] = i * sys.mission_time / 1000.0;
		//x[i] = i * 10.0;


		if (x[i] < b && x[i] < d) {

			y[i] = ((x[i] - a) / (b - a) + (x[i] - c) / (d - c)) / 2.0;
		}
		else
			y[i] = 1;

	}
	for (int i = 0; i < 1000; i++)
	{
		distribution_points.push_back({ x[i],y[i] });
		//std::cout << y[i] << "\t" << x[i] << "\n";
	}


}
//Cumulative Normal Distribution Function in C/C++ pointing to this implementation given in http://www.johndcook.com/cpp_phi.html.
double phi(double x, double mu, double s)
{
	// constants
	double a1 = 0.254829592;
	double a2 = -0.284496736;
	double a3 = 1.421413741;
	double a4 = -1.453152027;
	double a5 = 1.061405429;
	double p = 0.3275911;

	// Save the sign of x
	int sign = 1;
	if (x < 0)
		sign = -1;
	x = fabs(x - mu) / s * sqrt(2.0);

	// A&S formula 7.1.26
	double t = 1.0 / (1.0 + p * x);
	double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);

	return 0.5 * (1.0 + sign * y);
}
//function "check_if_rewarded_state_transition "that takes in a Component object, a State object, and an Event object.
// It checks if a state transition(event)is rewarded or not.
bool check_if_rewarded_state_transition(Component comp, State st, Event e)
{
	bool reward = false;
	for (auto& rew : comp.component_rewards)
	{
		if (rew.state_transition == st.name || rew.state_transition == e.id)
		{
			reward = true;
		}

	}

	return reward;
}
