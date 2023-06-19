#include"MixedDist.h"
#include"system.h"
MixedEvent::MixedEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, std::vector<std::pair<double, double>>dist_pt) : Event(id, "mixed_distribution", source, target),
dist_pt(dist_pt) {  }

double MixedEvent::get_event_time(std::vector<std::pair<double, double>>dist_pt, double Simulation_time) {
	double y = ran();
	//using simple linear interpolation formula to get the execution time x for random number y
	double x = dist_pt[10].first + ((y - dist_pt[10].second) * (dist_pt[25].first - dist_pt[10].first)) / (dist_pt[25].second - dist_pt[10].second);

	execution_time = x + Simulation_time;
	return (x + Simulation_time);

}
MixedEvent::~MixedEvent() {}
void MixedEvent::execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event)
{
	//set component state
	Component_Event->first->state = Component_Event->second.target;


}