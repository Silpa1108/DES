#ifndef PES_H
#define PES_H

#include "system.h"
#include"Header.h"
#include "Exponential.h"
#include "Uniform.h"
#include"MixedDist.h"
#include"Discrete.h"
#include"Normal.h"
/*PES class to hold all future component events from all components in the system */
class PES
{


public:
	std::vector<std::pair<std::shared_ptr<Component>, Event>>pending_events;


	//add event to pending event list
	std::vector<std::pair<std::shared_ptr<Component>, Event>> add_event(std::shared_ptr<Component> c, Event e);
	// sort and get event with smallest transition time;
	std::pair<std::shared_ptr<Component>, Event> get_next_event(std::vector<std::pair<std::shared_ptr<Component>, Event>>&);
	//update the pes based on change in state of system
	std::vector<std::pair< std::shared_ptr<Component>, Event>>update_pes(std::pair<std::shared_ptr<Component>, Event>, double);
	//copy only possible events
	std::vector<std::pair<std::shared_ptr<Component>, Event>> clean_pes(std::vector<std::pair< std::shared_ptr<Component>, Event>>, std::pair<std::shared_ptr<Component>, Event>);

	PES() {};
	~PES() {}
	std::pair<std::shared_ptr<Component>, Event>new_discrete_event;
};


#endif /* PES */
