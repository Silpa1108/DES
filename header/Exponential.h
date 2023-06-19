#ifndef EXPONENTIAL_H
#define EXPONENTIAL_H
#include"event.h"
#include"component.h"
class ExponentialEvent : public Event
{
public:

	double rate = 0;
	ExponentialEvent() {  }

	ExponentialEvent(std::string id, std::string type, std::string source, std::string target, double rate);

	double  get_event_time(double, double);
	~ExponentialEvent();
	void execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event);
};



#endif /* EXPONENTIAL */
