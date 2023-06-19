
#include"xmlparser.h"

using namespace std;
using namespace tinyxml2;               //xmlparsing helper

System  ParserXML::import_xmlfile(string filename)
{
	ParserXML x;
	x.file_name = filename.c_str(); // convert string to char for XML file reading

//initialisation
	XMLDocument doc;

	//load xml file
	doc.LoadFile(x.file_name);


	//get title from root element
	XMLElement* p_root_element = doc.RootElement();
	p_root_element->QueryStringAttribute("name", &title);
	std::cout << "\t \t" << title << endl;

	//information on mission time
	XMLElement* time_count = doc.FirstChildElement()->FirstChildElement("mission_time");
	time_count->QueryDoubleAttribute("duration", &mission_time);
	std::cout << "Missiontime = " << mission_time << endl;
	system.mission_time = mission_time;
	//information of components considered



	XMLElement* comp_count = doc.FirstChildElement()->FirstChildElement("component");
	while (comp_count)
	{// get component type (node or component itself)and name
		comp_count->QueryStringAttribute("name", &comp);
		comp_count->QueryStringAttribute("type", &typ);
		c_n += 1;

		//information of number of states and state names
		XMLElement* state_count = comp_count->FirstChildElement("states");
		while (state_count)
		{

			state_count->QueryStringAttribute("name", &name);
			state_count->QueryStringAttribute("type", &st);
			state_count->QueryStringAttribute("functioning", &f);
			state_count->QueryDoubleAttribute("initial_probability", &initial_probability);
			s_n += 1;
			state_count = state_count->NextSiblingElement("states");
			State s = State(st, name, f, initial_probability);
			states.push_back(s);   //stored in list states

		}

		//information of transiton type, parameters and state flows
		XMLElement* trans_count = comp_count->FirstChildElement("transition");

		while (trans_count)
		{
			t_n += 1;
			trans_count->QueryStringAttribute("id", &id);
			trans_count->QueryStringAttribute("source", &source);
			trans_count->QueryStringAttribute("target", &target);
			trans_count->QueryStringAttribute("transition_type", &type);

			string str_type = type;    //type cast const char to string 
			string str_source = source;
			string str_target = target;
			string str_id = id;
			if (str_type == "discrete")
			{
				trans_count->QueryDoubleAttribute("interval", &interval);
				Event* u = new DiscreteEvent(str_id, str_type, str_source, str_target, interval);
				u->interval = interval;
				events.push_back(std::move(*u));
				delete u;

			}
			else if (str_type == "uniform")
			{
				trans_count->QueryDoubleAttribute("lower", &lower);
				trans_count->QueryDoubleAttribute("upper", &upper);
				Event* u = new UniformEvent(str_id, str_type, str_source, str_target, lower, upper);
				u->lower = lower;
				u->upper = upper;
				events.push_back(std::move(*u));
				delete u;

			}
			else if (str_type == "normal")
			{
				trans_count->QueryDoubleAttribute("mu", &mu);
				trans_count->QueryDoubleAttribute("sigma", &sigma);
				Event* u = new NormalEvent(str_id, str_type, str_source, str_target, mu, sigma);
				u->mu = mu;
				u->sigma = sigma;
				events.push_back(std::move(*u));
				delete u;

			}
			else if (str_type == "exponential")
			{
				trans_count->QueryDoubleAttribute("rate", &rate);
				Event* u = new ExponentialEvent(str_id, str_type, str_source, str_target, rate);
				u->rate = rate;

				events.push_back(std::move(*u));
				delete u;
			}


			trans_count = trans_count->NextSiblingElement("transition");//get next event
		}
		//information on  components reward function considered
		XMLElement* cht = comp_count->FirstChildElement("rewards");
		while (cht)
		{
			XMLElement* presence = cht->FirstChildElement("reward");
			//information of transition rewards
			presence->QueryStringAttribute("name", &p);
			std::string str_p = p;
			XMLElement* transition_reward = presence->FirstChildElement("transition_reward");
			while (transition_reward)
			{
				transition_reward->QueryStringAttribute("transition", &tr);

				transition_reward->QueryDoubleAttribute("value", &va);
				transition_reward->QueryStringAttribute("type", &ty);
				Reward_parameter ctrp = Reward_parameter(str_p, "transition_reward", string(tr), string(ty), va);
				transition_reward = transition_reward->NextSiblingElement("transition_reward");
				reward_parameter_list.push_back(ctrp);

			}
			//component state rewards
			XMLElement* state_reward = presence->FirstChildElement("state_reward");
			while (state_reward)
			{

				state_reward->QueryStringAttribute("state", &sr);
				state_reward->QueryDoubleAttribute("value", &sva);
				state_reward->QueryStringAttribute("type", &sty);
				Reward_parameter csrp = Reward_parameter(str_p, "state_reward", string(sr), string(sty), sva);
				state_reward = state_reward->NextSiblingElement("state_reward");
				reward_parameter_list.push_back(std::move(csrp));

			}

			break;
		}
		//information on component attributes to be calculated such as uptime ,downtime ,rewards and also all results are are stored here after calculation
		XMLElement* calculate = comp_count->FirstChildElement("calculate");
		while (calculate)
		{// reward details
			XMLElement* calc_reward = calculate->FirstChildElement("reward");


			while (calc_reward)
			{
				calc_reward->QueryStringAttribute("name", &r_name);
				calc_reward->QueryStringAttribute("time", &missiontime);

				calc_reward->QueryDoubleAttribute("time", &r_t);
				if (std::string(missiontime) == "mission_time")
				{
					r_t = system.mission_time;
				}
				string reward = "reward";
				Calculate c = Calculate(reward, string(r_name), r_t);
				calculate_list.push_back(c);
				calc_reward = calc_reward->NextSiblingElement("reward");


			}
			//attribute details
			XMLElement* calc_attribute = calculate->FirstChildElement("attribute");
			while (calc_attribute)
			{
				calc_attribute->QueryStringAttribute("name", &at_name);
				calc_attribute->QueryStringAttribute("time", &missiontime);
				calc_attribute->QueryDoubleAttribute("time", &at_t);
				if (std::string(missiontime) == "mission_time")
				{
					at_t = system.mission_time;
				}
				Calculate a = Calculate("attribute", string(at_name), at_t);
				calculate_list.push_back(a);
				calc_attribute = calc_attribute->NextSiblingElement("attribute");


			}
			break;
		}

		//insert the component's states and events for each component into component list
		//Component *c=new  Component();
		std::shared_ptr<Component>c(new Component{ });
		c->name = comp;
		c->type = typ;
		c->events = events;
		c->states = states;
		c->component_calculate = calculate_list;
		c->component_rewards = reward_parameter_list;
		system.components.push_back(std::move(c));
		states.clear();
		events.clear();
		calculate_list.clear();
		reward_parameter_list.clear();
		comp_count = comp_count->NextSiblingElement("component");//get next component
	}
	//display component name and their corresponding events and states
	std::cout << "No of Components in system is " << c_n << endl;
	for (auto& i : system.components)
	{
		std::cout << i->name << endl;
		for (auto& l : i->states)
		{
			std::cout << "state  \t" << l.name << "\t " << l.type << l.functioning << endl;
		}
		for (auto& l : i->events)
		{
			std::cout << l.id << "\tstate transits from \t" << l.source << "\t to \t" << l.target << "\t" << "type " << "\t" << l.type << endl;
		}
		std::cout << endl;
	}

	// information about path and connection between components in system
	XMLElement* path_count = doc.FirstChildElement()->FirstChildElement("path");
	while (path_count)
	{

		path_count->QueryStringAttribute("source", &src);
		path_count->QueryStringAttribute("target", &dst);
		s_n += 1;
		path_count = path_count->NextSiblingElement("path");


		Path l = Path(src, dst);
		system.paths.push_back(l);   //stored in list paths
		//delete l;
	}
	for (auto& l : system.paths)
	{
		std::cout << "paths from \t" << l.get_SourceNode() << "\t to \t" << l.get_TargetNode() << "\t" << endl;
	}
	//to parse the system rewards and system attributes details
	XMLElement* system_rewards = doc.FirstChildElement()->FirstChildElement("rewards");
	while (system_rewards)
	{

		XMLElement* sys_reward = system_rewards->FirstChildElement("reward");

		sys_reward->QueryStringAttribute("name", &sysre);
		XMLElement* system_state_reward = sys_reward->FirstChildElement("system_state_reward");
		while (system_state_reward)
		{
			system_state_reward->QueryStringAttribute("state", &systr);
			system_state_reward->QueryDoubleAttribute("value", &sysva);
			system_state_reward->QueryStringAttribute("type", &syty);
			Reward_parameter ssrp = Reward_parameter(string(sysre), "system_state_reward", string(systr), string(syty), sysva);

			system.system_rewards.push_back(ssrp);
			system_state_reward = system_state_reward->NextSiblingElement("system_state_reward");

		}
		break;
	}
	XMLElement* system_calculate = doc.FirstChildElement()->FirstChildElement("calculate");
	while (system_calculate)
	{
		XMLElement* system_calculate_reward = system_calculate->FirstChildElement("reward");
		while (system_calculate_reward)
		{
			system_calculate_reward->QueryStringAttribute("name", &sys_r_name);
			system_calculate_reward->QueryStringAttribute("time", &missiontime);
			system_calculate_reward->QueryDoubleAttribute("time", &sys_r_t);
			if (std::string(missiontime) == "mission_time")
			{
				sys_r_t = system.mission_time;
			}
			XMLElement* system_rewards = doc.FirstChildElement()->FirstChildElement("rewards");
			while (system_rewards)
			{

				XMLElement* sys_reward = system_rewards->FirstChildElement("reward");

				sys_reward->QueryStringAttribute("name", &sysre);
				XMLElement* system_state_reward = sys_reward->FirstChildElement("system_state_reward");
				while (system_state_reward)
				{
					system_state_reward->QueryDoubleAttribute("value", &sysva);
					system_state_reward->QueryStringAttribute("type", &syty);

					Calculate s = Calculate("reward", string(sys_r_name), syty, sysva, sys_r_t);
					system.system_calculation.push_back(s);
					system_state_reward = system_state_reward->NextSiblingElement("system_state_reward");
				}
				break;


			}

			system_calculate_reward = system_calculate_reward->NextSiblingElement("reward");



		}

		XMLElement* system_calculate_attribute = system_calculate->FirstChildElement("attribute");

		while (system_calculate_attribute)
		{
			system_calculate_attribute->QueryStringAttribute("name", &sys_at_name);
			system_calculate_attribute->QueryStringAttribute("time", &missiontime);
			system_calculate_attribute->QueryDoubleAttribute("time", &sys_at_t);
			if (std::string(missiontime) == "mission_time")
			{
				sys_at_t = system.mission_time;
			}
			Calculate sa = Calculate("attribute", string(sys_at_name), sys_at_t);
			system.system_calculation.push_back(sa);
			system_calculate_attribute = system_calculate_attribute->NextSiblingElement("attribute");

		}
		break;

	}
	std::cout << endl;
	doc.Clear();
	return system;

}
