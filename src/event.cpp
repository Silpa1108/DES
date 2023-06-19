#include"event.h"

Event::Event(const std::string& id, const std::string& type, const std::string& source, const std::string& target)
{
	this->id = id;
	this->type = type;
	this->source = source;
	this->target = target;
}

double Event::ran()//random number generation between 0 and 1
{//https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
	//https://stackoverflow.com/questions/9878965/rand-between-0-and-1
	std::mt19937_64 rng;
	// initialize the random number generator with time-dependent seed
	uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
	rng.seed(ss);
	// initialize a uniform distribution between 0 and 1
	std::uniform_real_distribution<double> unif(0, 1);

	double q = unif(rng);
	return q;

}

