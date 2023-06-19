#ifndef UNIFORM
#define UNIFORM
#include "event.h"
#include"component.h"
class UniformEvent : public Event
{

public:

	double  lower = 0;
	double  upper = 0;
	UniformEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double lower, double upper);
	double get_event_time(double, double, double);
	UniformEvent() {};//copy constructor
	void execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event);
	~UniformEvent();
};


#endif /* UNIFORM */
