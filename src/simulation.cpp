#include"simulation.h"
#include"Calculate.h"
void MCSimulation::run(System* system)
{
	// Run the simulation for 2000 iterations
	for (int i = 0; i < 2000; i++)
	{
		// Create an instance of the simulation engine with the given system
		Engine e = Engine(system);
		//  the current iteration number
		std::cout << i << "\n";
		//sojourn information for each component is push into sojourn_completedata 2 dimensional vector after each simulation im montecarlo
		for (std::shared_ptr<Component> const& ptr : system->components)
		{
			ptr->state_sojourn_completedata.push_back(ptr->state_sojourn_info);

			ptr->state_sojourn_info.clear();
		}
		//sojourn information for system is push into monte_carlo_system_uptime_interval 2 dimensional vector after each simulation in montecarlo
		system->monte_carlo_system_uptime_interval.push_back(system->system_uptime_intervals);
		system->system_uptime_intervals.clear();
	}

}
