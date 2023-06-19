#include"Exponential.h"
#include"system.h"
ExponentialEvent::ExponentialEvent(std::string id, std::string type, std::string source, std::string target, double rate = 0) : Event(id, "exponential", source, target)
, rate(rate) {

}
double ExponentialEvent::get_event_time(double rate, double Simulation_time) {


	double u = Event::ran();

	double ret = (-1 / rate * (log(1 - u)));
	execution_time = ret + Simulation_time;
	return execution_time;

}
ExponentialEvent::~ExponentialEvent() {}
void ExponentialEvent::execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event)
{

	// Set component state .

	Component_Event->first->state = Component_Event->second.target;



}