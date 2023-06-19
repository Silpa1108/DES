#ifndef STATISTICS
#define STATISTICS
#include"event.h"
#include"component.h"
#include"system.h"
/*all system and component statistical calculation to obtain the performance metrics are done here.Statistical calculations are done on
system and component sojourn data obtained over the montecarlo simualtion of the engine*/
class Statistics
{
private:
	System sys;
	int transition_visit_count = 0;
	int state_visit_count = 0;
	double state_visit_rewards = 0;
	double transition_visit_rewards = 0;
	//vectors to store the statistics calculated
	std::vector<double>system_uptimes;
	std::vector<double>system_downtimes;
	std::vector<double>component_uptime;
	std::vector<double>component_downtimes;
	std::vector<double>component_availability;
	std::vector<double>system_availability;
	std::vector<double>system_mtbf;
	std::vector<double>component_mtbf;
	std::vector<double>state_visit_reward_vector;
	std::vector<double>system_reward_vector;
	std::vector<double>transition_visit_reward_vector;


	//calculate rewards on state visits at component level
	std::vector<double> reward_on_state_visits(std::shared_ptr<Component>c, Reward_parameter r, double);
	//calculate rewards on state visits at system level
	std::vector<double> system_reward_on_state_visits(std::vector<std::vector<std::pair<double, double>>>, Reward_parameter r, double);
	//calculate rewards on transition visits at component level
	std::vector<double> reward_on_transition_visits(std::shared_ptr<Component>c, Reward_parameter r, double);
	std::vector<double> get_system_uptime(std::vector<std::vector<std::pair<double, double>>> system_uptime_intervals, double);
	std::vector<double> get_system_availability_vector(std::vector<double>system_uptimes, double);
	//Mean time between failure at system level
	std::vector<double> get_system_mtbf_vector(std::vector<std::vector<std::pair<double, double>>> system_uptime_intervals, double);
	std::vector< double>get_component_uptimes(std::shared_ptr<Component>c, double);
	std::vector< double>get_component_mtbf(std::shared_ptr<Component>c, double);

public:
	System get_statistics(System);
};



#endif /* STATISTICS */
