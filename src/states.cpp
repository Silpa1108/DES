
#include"states.h"

State::State(const std::string& type, const std::string& name, const std::string& functioning, const double& initial_probability)
{
	this->type = type;
	this->name = name;
	this->functioning = functioning;
	this->initial_probability = initial_probability;
}

TransientState::TransientState(std::string name, std::string functioning, double initial_probability) : State("transient", name, functioning, initial_probability)
{
}

AbsorbingState::AbsorbingState(std::string name, std::string functioning, double initial_probability) : State("absorbing", name, functioning, initial_probability)
{
}

HiddenState::HiddenState(std::string name, std::string functioning, double initial_probability) : State("hidden", name, functioning, initial_probability)
{
}
