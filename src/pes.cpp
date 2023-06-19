#include"pes.h"
//#include"../include/Header.h"




//function to add new event to pending event set

std::vector<std::pair< std::shared_ptr<Component>, Event>> PES::add_event(std::shared_ptr<Component >c, Event e)
{

	pending_events.push_back(std::make_pair(c, e));
	return pending_events;

}

//function to sort events based on execution time(c++ vector has only pop back)
//sort in ascending order
bool sortByTimea(std::pair<std::shared_ptr<Component>, Event>& lhs, std::pair< std::shared_ptr<Component>, Event>& rhs)
{
	return lhs.second.execution_time < rhs.second.execution_time;
}
//sort in decending order
bool sortByTime(std::pair<std::shared_ptr<Component>, Event>& lhs, std::pair<std::shared_ptr<Component>, Event>& rhs)
{
	return lhs.second.execution_time > rhs.second.execution_time;
}
// get event with minimum transition time after sorting
std::pair<std::shared_ptr<Component>, Event> PES::get_next_event(std::vector<std::pair<std::shared_ptr<Component>, Event>>& pending_events)
{
	std::pair<std::shared_ptr<Component>, Event> itt = *std::min_element(pending_events.begin(), pending_events.end(), sortByTimea);

	return itt;
}
// clean up the pending event set (PES) by removing the event with the smallest time, 
// and also removing all other  events from the same component except for its discrete events.

std::vector<std::pair< std::shared_ptr<Component>, Event>>PES::clean_pes(std::vector<std::pair< std::shared_ptr<Component>, Event>>pending_events, std::pair<std::shared_ptr<Component>, Event>Component_Event)
{

	std::vector<std::pair< std::shared_ptr<Component>, Event>>::iterator it;

	sort(pending_events.begin(), pending_events.end(), sortByTime);// Sort the PES in descending order by time
	pending_events.pop_back();//delete  min time event from sorted pes
	//remove the other events except discrete event of the component  , whose event had minimum time of execution from  the pes,so that the pes now has only the events of other components and discrete events of minimum event component .
	//new events corresponding the the component's new state will be addded later in update_pes
	it = std::remove_if(pending_events.begin(), pending_events.end(), [&](auto r) {return (r.first->name == Component_Event.first->name && r.second.type != "discrete"); });
	// Add the remaining events to a temporary list.Templist is used to temporarily store the elements that are not removed from pending_events before clearing and reassigning it.
	std::vector<std::pair<std::shared_ptr<Component>, Event>>templist;
	for (std::vector<std::pair< std::shared_ptr<Component>, Event>>::iterator p = pending_events.begin();
		p != it; ++p)
	{
		templist.push_back(*p);
	}
	pending_events.clear();

	pending_events.assign(templist.begin(), templist.end());



	return pending_events;
}

//update the pes list by adding new events based on new system state(discrete event is already there in pes so dont have to consider it)
std::vector<std::pair< std::shared_ptr<Component>, Event>> PES::update_pes(std::pair<std::shared_ptr<Component>, Event>Component_Event, double Simulation_time)
{
	Event new_event;
	std::list<Event>::iterator event;


	for (event = Component_Event.first->events.begin(); event != Component_Event.first->events.end(); event++)
	{
		//check if component is in absorbed state then dont update pes and pes contains only discrete events of that component:done
		//implemented in engine
		if (event->source == Component_Event.first->state && event->type != "discrete"/*&& Component_Event.second.type!="discrete"*/)
		{
			if (event->type == "exponential")
			{
				ExponentialEvent* f = new ExponentialEvent(event->id, event->type, event->source, event->target, event->get_rate());
				double now = f->get_event_time(event->get_rate(), Simulation_time);
				f->set_executiontime(now);
				add_event(Component_Event.first, *f);
				delete f;
			}
			else if (event->type == "uniform")
			{
				UniformEvent* f = new UniformEvent(event->id, event->type, event->source, event->target, event->get_lower(), event->get_upper());
				double now = f->get_event_time(event->get_lower(), event->get_upper(), Simulation_time);
				f->set_executiontime(now);
				add_event(Component_Event.first, *f);
				delete f;
			}
			else if (event->type == "normal")
			{
				NormalEvent* f = new NormalEvent(event->id, event->type, event->source, event->target, event->get_mu(), event->get_sigma());
				double now = f->get_event_time(event->get_mu(), event->get_sigma(), Simulation_time);
				f->set_executiontime(now);
				add_event(Component_Event.first, *f);
				delete f;
			}
			else if (event->type == "mixed_distribution")
			{
				MixedEvent* f = new MixedEvent(event->id, event->type, event->source, event->target, event->dist_pt);
				double now = f->get_event_time(event->dist_pt, Simulation_time);
				f->set_executiontime(now);
				add_event(Component_Event.first, *f);
				delete f;
			}
		}


	}
	return pending_events;
}

