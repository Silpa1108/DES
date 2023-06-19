#include"Uniform.h"
#include"system.h"
UniformEvent::UniformEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double lower, double upper) : Event(id, "uniform", source, target),
lower(lower),
upper(upper) { }

double UniformEvent::get_event_time(double lower, double upper, double Simulation_time) {

	double q = Event::ran();    //get random number between 0 and 1
	double t = lower * (1 - q) + upper * q;
	if (t >= upper) t = lower - 1;
	if (t < lower) t = lower;
	execution_time = t + Simulation_time;
	return (t + Simulation_time);

}
UniformEvent::~UniformEvent() {}
void UniformEvent::execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event) {

	//set component state
	Component_Event->first->state = Component_Event->second.target;


}
