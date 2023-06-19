#include"statistics.h"
System Statistics::get_statistics(System sys)
{	//get ths system rewards and attributes to be calculated
	for (auto& calculate : sys.system_calculation)
	{//system attributes like uptime, downtime,mtbf and availability are calculated
		while (calculate.parameter_type == "attribute")
		{
			if (calculate.name == "uptime")
			{
				calculate.result = get_system_uptime(sys.monte_carlo_system_uptime_interval, calculate.time);
			}
			else if (calculate.name == "availability")
			{
				system_uptimes.clear();
				system_uptimes = get_system_uptime(sys.monte_carlo_system_uptime_interval, calculate.time);

				calculate.result = get_system_availability_vector(system_uptimes, calculate.time);

			}
			else if (calculate.name == "MTBF")//mean time between failure
			{


				calculate.result = get_system_mtbf_vector(sys.monte_carlo_system_uptime_interval, calculate.time);

			}
			else if (calculate.name == "downtime")
			{
				system_uptimes.clear();
				system_uptimes = get_system_uptime(sys.monte_carlo_system_uptime_interval, calculate.time);

				for (auto& r : system_uptimes)
				{

					system_downtimes.push_back(sys.mission_time - r);
				}
				calculate.result = system_downtimes;
			}
			break;
		}

		while (calculate.parameter_type == "reward")
		{
			double time = calculate.get_time();
			std::list<Reward_parameter>::iterator reward;
			reward = sys.system_rewards.begin();
			while (reward != sys.system_rewards.end()) {
				if (reward->name == calculate.name && reward->value == calculate.value)
				{
					calculate.result = system_reward_on_state_visits(sys.monte_carlo_system_uptime_interval, *reward, time);

				}
				reward++;



			}

			break;
		}

	}
	for (std::shared_ptr<Component>comp : sys.components)
	{

		for (auto& calc : comp->component_calculate)
		{
			double time = calc.get_time();
			while (calc.parameter_type == "reward")
			{

				std::list<Reward_parameter>::iterator reward;
				reward = std::find_if(comp->component_rewards.begin(), comp->component_rewards.end(), [&](Reward_parameter r) {return (r.name == calc.name); });

				if (/*reward->name == "number_of_times" &&*/ reward->reward_type == "state_reward")
				{
					for (auto& rew : comp->component_rewards)
					{

						rew.result = reward_on_state_visits(comp, rew, time);
					}
				}
				else if (/*reward->name == "cost_of_tests" &&*/ reward->reward_type == "transition_reward")
				{
					for (auto& rew : comp->component_rewards)
					{

						rew.result = reward_on_transition_visits(comp, rew, time);
					}
				}
				break;
			}
			while (calc.parameter_type == "attribute")
			{


				if (calc.name == "uptime")
				{
					component_uptime.clear();
					component_uptime = get_component_uptimes(comp, calc.time);
					calc.result = component_uptime;

				}
				else if (calc.name == "MTBF")
				{

					component_mtbf = get_component_mtbf(comp, calc.time);
					calc.result = component_mtbf;

				}
				else if (calc.name == "downtime")
				{
					component_uptime.clear();
					component_downtimes.clear();
					component_uptime = get_component_uptimes(comp, calc.time);
					for (auto& r : component_uptime)
					{

						component_downtimes.push_back(calc.time - r);

					}
					calc.result = component_downtimes;
				}
				else if (calc.name == "availability")
				{
					component_uptime.clear();
					component_availability.clear();
					component_uptime = get_component_uptimes(comp, calc.time);
					for (auto& r : component_uptime)
					{

						component_availability.push_back(r / calc.time);
					}
					calc.result = component_availability;
				}
				break;
			}


		}


	}
	return sys;
}
//  function to calculate the reward on state visits for a given component and reward parameters
// It returns a vector containing the reward for each visit to the specified state
// Inputs are comp: a shared pointer to the component for which rewards are being calculated
// rew: a struct containing reward parameters including state transition and reward value
// time: time limit considered
// Outputs is state_visit_reward_vector: a vector containing the reward for each visit to the specified state
std::vector<double> Statistics::reward_on_state_visits(std::shared_ptr<Component> comp, Reward_parameter rew, double time)
{
	// If reward type is "count", calculate reward based on the number of visits to the specified state
	while (rew.type == "count")
	{
		state_visit_reward_vector.clear();
		for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : comp->state_sojourn_completedata)
		{
			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;
			//get only the considered state details from sojourn component vector into vector intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<0>(r) == rew.state_transition); });


			// to count the number of visits that meet the time criteria
			for (int i = 0; i < intervals1.size(); i++)
			{

				if ((std::get<3>(intervals1.at(i)) <= time || (std::get<2>(intervals1.at(i)) < time && std::get<3>(intervals1.at(i)) == sys.mission_time))/*&& rew.state_transition == std::get<0>(intervals1.at(i))*/)
				{
					state_visit_count++;
				}

			}


			// calculate reward based on number of visits and reward value
			state_visit_rewards = double(state_visit_count * rew.value);
			state_visit_reward_vector.push_back(state_visit_rewards);
			state_visit_count = 0;
		}
		break;
	}
	// If reward type is "rate", calculate reward based on time spent in the considered state
	while (/*state.name == rew.state_transition &&*/ rew.type == "rate")
	{
		state_visit_reward_vector.clear();
		for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : comp->state_sojourn_completedata)
		{// a new vector containing only tuples for the specified state
			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;
			//get only the considered state details from sojourn component vector into vector intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<0>(r) == rew.state_transition); });



			double res = 0;
			if (intervals1.size() >= 1)
			{


				for (const auto& j : intervals1)
				{
					if ((std::get<3>(j) < time) && std::get<2>(j) <= time)
					{
						res += std::get<3>(j) - std::get<2>(j);//sum up difference between interval time

					}
					else if ((std::get<2>(j) < time) && std::get<3>(j) > time)
					{
						res += time - std::get<2>(j);
					}

				}


			}
			// calculate reward based on time spent in the specified state and reward value
			res = res * rew.value;
			state_visit_reward_vector.push_back(res);
			res = 0;
		}
		break;
	}
	return state_visit_reward_vector;
}
/*This function calculates rewards based on the number of transitions or the transition rate, for a given componentand transition parameter
 rew: Reward_parameter object containing the reward type (count or rate), state transition and value
time: double value representing the time limit considered
Output is transition_visit_reward_vector: vector of doubles containing the rewards for each transition visit or transition rate*/
std::vector<double> Statistics::reward_on_transition_visits(std::shared_ptr<Component> comp, Reward_parameter rew, double time)
{// Calculate rewards based on the number of transitions (rew.type == "count")
	while (rew.type == "count") {
		transition_visit_reward_vector.clear();
		for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : comp->state_sojourn_completedata)
		{
			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;
			// Get only the intervals of the desired state transition into intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<1>(r) == rew.state_transition); });

			// Iterate through the intervalsq and count the number of transitions that have occurred before the considered time limit
			for (int i = 0; i < intervals1.size(); i++)
			{
				if (std::get<3>(intervals1.at(i)) <= time /*&& std::get<0>(intervals1.at(i)) == std::get<1>(intervals1.at(i)).substr(0, std::get<1>(intervals1.at(i)).find(' '))*/)
				{
					transition_visit_count++;
				}
			}
			// Calculate reward based on the count of transitions and the specified reward value, and add it to the reward vector
			transition_visit_rewards = double(transition_visit_count * rew.value);
			transition_visit_reward_vector.push_back(transition_visit_rewards);
			// Reset the variables
			transition_visit_count = 0;
			transition_visit_rewards = 0;
		}
		break;

	}
	// Calculate rewards based on the transition rate (rew.type == "rate")
	while (rew.type == "rate")
	{
		transition_visit_reward_vector.clear();
		double res = 0;
		for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : comp->state_sojourn_completedata)
		{
			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;
			//get only the considered state details from sojourn component vector into vector intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<1>(r) == rew.state_transition); });

			for (int i = 0; i < intervals1.size(); i++)
			{


				if ((std::get<3>(intervals1.at(i)) < time) && std::get<2>(intervals1.at(i)) <= time)
				{
					res += std::get<3>(intervals1.at(i)) - std::get<2>(intervals1.at(i));

				}
				else if ((std::get<2>(intervals1.at(i)) < time) && std::get<3>(intervals1.at(i)) > time)
				{
					res += time - std::get<2>(intervals1.at(i));
				}


			}
			//calculate reward based on time spend in the specified transition and reward value.
			transition_visit_rewards = res * rew.value;
			transition_visit_reward_vector.push_back(transition_visit_rewards);
			transition_visit_rewards = 0;
			res = 0;
		}
		break;
	}
	return transition_visit_reward_vector;
}
/*
Calculates the total system uptime for each simulation in the input vector of system uptime intervals.
 system_uptime_intervals : vector of vectors of pairs representing the system uptime intervals for each simulation
time: The time limit for which the system uptime is to be calculated
Output is A vector of doubles representing the total system uptime for each simulation
*/
std::vector<double> Statistics::get_system_uptime(std::vector<std::vector<std::pair<double, double>>> system_uptime_intervals, double time) {
	std::vector<std::pair<double, double>>intervals1;
	system_uptimes.clear();
	// Loop through each simulation in the input vector of system uptime intervals
	for (std::vector<std::pair<double, double>>intervals : system_uptime_intervals)
	{

		double res = 0;
		// Loop through each interval in the current simulation
		for (int i = 0; i < intervals.size(); i++)
		{
			// If the current interval is completely within the time limit, add it to vector intervals1
			if (intervals[i].first < time && intervals[i].second <= time)
			{

				intervals1.push_back(intervals[i]);
			}
			// If the current interval is partially within the time limit, add the portion within the time limit to intervals1
			else if (intervals[i].first < time && intervals[i].second > time)
			{
				intervals1.push_back(std::make_pair(intervals[i].first, time));

			}

		}

		if (intervals1.size() > 0)
		{// Calculate the total uptime for the current simulation using intervals1
			for (const auto& j : intervals1)
			{
				res += j.second - j.first;
			}
		}
		system_uptimes.push_back(res);
		// Clear intervals1 and reset res for the next simulation
		intervals1.clear();
		res = 0;
	}

	return system_uptimes;
}
/*
 function calculates the system availability vector based on the system uptimesand the total time elapsed.
system_uptimes: A vector containing the system uptimes.
time: considered time limit.
return A vector containing the system availabilities.*/

std::vector<double> Statistics::get_system_availability_vector(std::vector<double>system_uptimes, double time)
{
	system_availability.clear();
	// Calculate the system availability for each system uptime and push it to the system availability vector.
	for (auto& r : system_uptimes)
	{

		system_availability.push_back(r / time);
	}


	return system_availability;
}
// This function calculates the Mean Time Between Failures (MTBF) for a system.
// It takes as input a vector of vectors, where each inner vector contains pairs of start and end times for system uptime intervals.
std::vector<double> Statistics::get_system_mtbf_vector(std::vector<std::vector<std::pair<double, double>>> system_uptime_intervals, double time)
{

	std::vector<std::pair<double, double>>intervals1;
	system_mtbf.clear();
	for (std::vector<std::pair<double, double>>intervals : system_uptime_intervals) //intervals is vector for each simulation
	{

		double down_count = 0;
		double res = 0;
		for (int i = 0; i < intervals.size(); i++)
		{

			if (intervals[i].first < time && intervals[i].second <= time)
			{

				intervals1.push_back(intervals[i]);
			}
			else if (intervals[i].first < time && intervals[i].second > time)
			{
				intervals1.push_back(std::make_pair(intervals[i].first, time));

			}

		}
		down_count = intervals1.size();// count of downtime intervals
		if (intervals1.size() > 0)
		{
			if (intervals1.front().first == 0 && intervals1.back().second <= time)//case when number of down intervals is one less than number of up intervals
			{
				down_count = down_count - 1;
			}
			// calculate the total uptime of the system
			for (const auto& j : intervals1)
			{
				res += j.second - j.first;
			}
		}
		// calculate and store the MTBF, if the system experienced failures or downtimes
		if (res != 0 and down_count > 0)
		{
			system_mtbf.push_back(res / down_count);//mtbf=uptime/number of failures
		}
		intervals1.clear();
		res = 0;
		down_count = 0;
	}

	return system_mtbf;
}
//function to get component uptime
std::vector<double> Statistics::get_component_uptimes(std::shared_ptr<Component>component, double time)
{

	component_uptime.clear();
	for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : component->state_sojourn_completedata)
	{
		double res = 0;

		for (auto state : component->states)
		{

			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;

			//get only the considered state details from sojourn component vector into vector intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<0>(r) == state.name && state.functioning == "1"); });


			if (intervals1.size() >= 1)
			{

				for (const auto& j : intervals1)
				{
					if ((std::get<3>(j) < time) && std::get<2>(j) <= time)
					{
						res += std::get<3>(j) - std::get<2>(j);

					}
					else if ((std::get<2>(j) < time) && std::get<3>(j) > time)
					{
						res += time - std::get<2>(j);
					}


				}
			}

		}
		component_uptime.push_back(res);


	}

	return component_uptime;
}
//function to get component mtbf
std::vector<double> Statistics::get_component_mtbf(std::shared_ptr<Component>component, double time)
{
	double down_count = 0;
	component_mtbf.clear();
	for (std::vector<std::tuple<std::string, std::string, double, double>>intervals : component->state_sojourn_completedata)
	{
		double res = 0;
		std::vector<std::tuple<std::string, std::string, double, double>>intervals2;
		for (auto state : component->states)
		{//vector intervals2 size give no of failure counts :non functioning state counts

			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals2), [&](std::tuple<std::string, std::string, double, double> r) {return (std::get<0>(r) == state.name && state.functioning == "0" && std::get<3>(r) <= time && std::get<0>(r) == std::get<1>(r).substr(0, std::get<1>(r).find(' '))); });

		}
		if (intervals2.size() > 0)
			down_count = intervals2.size();
		for (auto state : component->states)
		{

			std::vector<std::tuple<std::string, std::string, double, double>>intervals1;

			//get only the considered state details from sojourn component vector into vector intervals1
			std::copy_if(intervals.begin(), intervals.end(), std::back_inserter(intervals1), [&](auto r) {return (std::get<0>(r) == state.name && state.functioning == "1"); });
			//get total uptime of component

			if (intervals1.size() >= 1)
			{

				for (const auto& j : intervals1)
				{
					if ((std::get<3>(j) < time) && std::get<2>(j) <= time)
					{
						res += std::get<3>(j) - std::get<2>(j);

					}
					else if ((std::get<2>(j) < time) && std::get<3>(j) > time)
					{
						res += time - std::get<2>(j);
					}


				}
			}

		}
		if (down_count > 0 && res != 0)//we need to consider entries in mtbf list when there is failure occurs and also it functions at some point of time
		{
			component_mtbf.push_back(res / down_count);
		}//uptime divided by failure counts give mtbf

		down_count = 0;
	}

	return component_mtbf;
}

//Calculate system reward based on system state visits
std::vector<double> Statistics::system_reward_on_state_visits(std::vector<std::vector<std::pair<double, double>>>uptime_list, Reward_parameter rew, double time)
{
	int system_state_visit_count = 0;
	// Count-based reward for state 1 "functioning or up"
	while (rew.state_transition == "1" && rew.type == "count")
	{
		system_reward_vector.clear();
		//from uptime vectors of all simulations iterate through each vector representing uptime of single simulation
		for (std::vector<std::pair<double, double>>intervals1 : uptime_list)
		{
			for (int i = 0; i < intervals1.size(); i++)
			{
				//from pair of uptimes in intervals1 consider the second part of interval .check if it is less than considered time
				if (std::get<1>(intervals1.at(i)) <= time)
				{
					system_state_visit_count++;

				}
			}
			// Calculate reward for state visits

			state_visit_rewards = double(system_state_visit_count) * rew.value;
			system_reward_vector.push_back(state_visit_rewards);
			system_state_visit_count = 0;
		}
		break;
	}
	// Count-based reward for state 0 "nonfunctioning or down"
	while (rew.state_transition == "0" && rew.type == "count")
	{
		int system_state_visit_count = 0;

		system_reward_vector.clear();
		std::vector<std::pair<double, double>>new_list;
		for (std::vector<std::pair<double, double>>intervals1 : uptime_list)
		{

			for (int i = 0; i < intervals1.size(); i++)
			{

				if (std::get<1>(intervals1.at(i)) <= time)//case when number of up intervals=number of down intervals or system is always up 
				{
					system_state_visit_count++;
					new_list.push_back(intervals1[i]);
				}
			}
			if (new_list.size() > 0)
			{
				if (new_list.size() != 1 && new_list.front().first == 0 && new_list.back().second <= time)//case when number of down intervals is one less than number of up intervals
				{
					system_state_visit_count = system_state_visit_count - 1;
				}
			}
			// Calculate reward for state visits
			state_visit_rewards = double(system_state_visit_count) * rew.value;
			system_reward_vector.push_back(state_visit_rewards);
			new_list.clear();
			system_state_visit_count = 0;

		}
		break;
	}
	while (rew.state_transition == "1" && rew.type == "rate")
	{
		system_reward_vector.clear();
		double r = 0;
		std::vector<std::pair<double, double>>new_list;
		for (std::vector<std::pair<double, double>>intervals1 : uptime_list)
		{
			for (int i = 0; i < intervals1.size(); i++)
			{
				//s = std::find_if(intervals.begin(), intervals.end(), [&](auto r) {return (std::get<0>(r) == state.name); });
				if (std::get<1>(intervals1.at(i)) <= time)
				{
					new_list.push_back(intervals1[i]);

				}
			}
			for (const auto& j : new_list)
			{
				r += j.second - j.first;

			}
			system_reward_vector.push_back(r * rew.value);
			new_list.clear();
			r = 0;


		}
		break;
	}
	while (rew.state_transition == "0" && rew.type == "rate")
	{
		int system_state_visit_count = 0;
		double r = 0;
		system_reward_vector.clear();
		std::vector<std::pair<double, double>>new_list;
		for (std::vector<std::pair<double, double>>intervals1 : uptime_list)
		{

			for (int i = 0; i < intervals1.size(); i++)
			{
				//s = std::find_if(intervals.begin(), intervals.end(), [&](auto r) {return (std::get<0>(r) == state.name); });
				if (std::get<1>(intervals1.at(i)) <= time)
				{

					new_list.push_back(intervals1[i]);
				}
			}
			for (const auto& j : new_list)
			{
				r += j.second - j.first;//uptime r

			}
			system_reward_vector.push_back((time - r) * rew.value);//time - uptime gives downtime and downtime*reward value
			r = 0;
			new_list.clear();
		}
		break;
	}
	return system_reward_vector;
}