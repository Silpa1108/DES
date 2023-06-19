#include "Discrete.h"
#include "pes.h"
#include "system.h"
DiscreteEvent::DiscreteEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double interval) : Event(id, "discrete", source, target)
, interval(interval) { }
double DiscreteEvent::get_event_time(double interval) {
	execution_time = (interval);
	return (double(execution_time));

}
std::pair<std::shared_ptr<Component>, Event> DiscreteEvent::execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event) {
	count = 1;
	//new state get changed to target state only if current state and source states are same and previous event time is less than the discrete time interval.
	if (Component_Event->second.source == Component_Event->first->getstate())
	{
		//set new component state

		Component_Event->first->state = Component_Event->second.target;
	}

	PES pes;
	Component_Event->second.interval = Component_Event->second.execution_time;
	cP = &count;                             //increment the counter for the new interval (for discrete event))
	(*cP)++;
	//get the new discrete event object 
	DiscreteEvent* new_event = new DiscreteEvent(Component_Event->second.id, Component_Event->second.type, Component_Event->second.source, Component_Event->second.target, Component_Event->second.interval);
	new_event->interval = double(count) * Component_Event->second.interval;

	new_event->execution_time = new_event->get_event_time(new_event->interval);
	previous_time = Component_Event->second.Simulation_time;
	double time = new_event->execution_time;
	new_event->set_executiontime(time);       //execution time for new discrete event
	pes.new_discrete_event = std::make_pair(Component_Event->first, *new_event);
	delete new_event;
	//return the new discrete event that has to be added to pes
	return pes.new_discrete_event;

}
