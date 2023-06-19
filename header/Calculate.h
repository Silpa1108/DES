#ifndef CALCULATE_H
#define CALCULATE_H
//#include"system.h"
//#include"Header.h"
#include<string>
#include<vector>
//this class is to get parameters  to do performance metrics calculation and reward calculation at system and component level.
class Calculate
{
public:

	std::string parameter_type;
	std::string name;
	std::string reward_type;
	double time = 0;
	double value = 0;
	std::vector<double>result;
	double& get_time() { return time; }
	//parameter types are reward or attributes
	//constructors for reading the parameters and storing the results.
	Calculate(const std::string& parameter_type, const std::string& name, double& time);
	Calculate(const std::string& parameter_type, const std::string& name, double& time, std::vector<double>& result);
	Calculate(const std::string& parameter_type, const std::string& name, const std::string& reward_type, double& value, double& time);
};
class Reward_parameter
{
public:
	std::string name;
	std::string type;
	std::string reward_type;
	std::string state_transition;
	std::vector<double>result;
	double value;
	const  std::string& get_name() const { return name; }

	Reward_parameter(const std::string& name, const std::string& reward_type, const std::string& state_transition, const std::string& type, double& value);
	Reward_parameter(const std::string& name, const std::string& reward_type, const std::string& state_transition, const std::string& type, double& value, std::vector<double>& result);
};

#endif /* CALCULATE */

