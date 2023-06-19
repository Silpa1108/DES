#include"Normal.h"
#include"system.h"
NormalEvent::NormalEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double mu = 0, double sigma = 0) : Event(id, "normal", source, target)
, mu(mu),
sigma(sigma) {

}
double NormalEvent::get_event_time(double mu, double sigma, double Simulation_time) {


	double u = Event::ran();
	double ret = mu + (std::erfc(2u - 1)) * (sigma * (sqrt(2)));
	execution_time = ret + Simulation_time;
	return execution_time;

}
NormalEvent::~NormalEvent() {}
void NormalEvent::execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event) {
	//component state 

	Component_Event->first->state = Component_Event->second.target;


}