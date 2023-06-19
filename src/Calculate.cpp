#include"Calculate.h"
Calculate::Calculate(const std::string& parameter_type, const  std::string& name, double& time)
{
	this->parameter_type = parameter_type;
	this->name = name;
	this->time = time;
}
Calculate::Calculate(const std::string& parameter_type, const  std::string& name, double& time, std::vector<double>& result)
{
	this->parameter_type = parameter_type;
	this->name = name;
	this->time = time;
	this->result = result;
}
Calculate::Calculate(const std::string& parameter_type, const std::string& name, const std::string& reward_type, double& value, double& time)
{
	this->parameter_type = "reward";
	this->name = name;
	this->time = time;
	this->value = value;
	this->reward_type = reward_type;

}
Reward_parameter::Reward_parameter(const std::string& name, const std::string& reward_type, const std::string& state_transition, const std::string& type, double& value)
{

	this->name = name;
	this->reward_type = reward_type;
	this->state_transition = state_transition;
	this->type = type;
	this->value = value;
}
Reward_parameter::Reward_parameter(const std::string& name, const std::string& reward_type, const std::string& state_transition, const std::string& type, double& value, std::vector<double>& result)

{

	this->name = name;
	this->reward_type = reward_type;
	this->state_transition = state_transition;
	this->type = type;
	this->value = value;
	this->result = result;
}