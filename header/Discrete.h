#ifndef DISCRETEL_H
#define DISCRETEL_H
#include "event.h"
#include"component.h"

class DiscreteEvent : public Event
{
public:
	double  interval = 0;

	int count = 0;

	int* cP = NULL;

	DiscreteEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double interval);
	DiscreteEvent() {  }                  //default constructor
	double  get_event_time(double);			//get the execution time
	std::pair<std::shared_ptr<Component>, Event> execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event); //declaration of execute event of discrete type

};

#endif /* DISCRETE_H */