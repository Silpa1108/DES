#ifndef NORMAL_H
#define NORMAL_H
#include"event.h"
#include"component.h"
class NormalEvent : public Event
{
public:

	double mu = 0;
	double sigma = 0;
	NormalEvent() {  }

	NormalEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, double mu, double sigma);
	//double  get_event_time(double, double,double);
	double  get_event_time(double, double, double);
	~NormalEvent();
	void execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event);
};



#endif /* NORMAL */
