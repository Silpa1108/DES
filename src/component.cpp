#include"component.h"


Component::Component(const std::string& type, const std::string& name, const std::list<State>& states, const std::list<Event>& events, const std::list<Calculate>& calculate)
{
    this->type = type;
    this->name = name;
    this->states = states;
    this->events = events;
    this->component_calculate = calculate;
}
Component::Component(const std::string& type, const std::string& name)
{
    this->type = type;
    this->name = name;


}
