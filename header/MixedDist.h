#ifndef MIXEDDIST
#define MIXEDDIST
#include"event.h"
#include"component.h"
//class for mixed distribution
class MixedEvent : public Event
{

public:

	std::vector<std::pair<double, double>>dist_pt;
	MixedEvent(const std::string& id, const std::string& type, const std::string& source, const std::string& target, std::vector<std::pair<double, double>>);
	double get_event_time(std::vector<std::pair<double, double>>, double);
	MixedEvent() {};//copy constructor
	void execute(std::pair<std::shared_ptr<Component>, Event>* Component_Event);
	~MixedEvent();
};

#endif /* MIXEDDIST */
