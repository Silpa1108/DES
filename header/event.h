#ifndef EVENT_H
#define EVENT_H
#include"Header.h"
/*Event class is a base class with several child classes that implement specific types of transitions with different probability distribution
 such as exponential, uniform ,normal and events at discrete time intervals.*/
class Event {
public:
    // Member variables
    std::string type;       // Type of event
    std::string source;     // Source state of event
    std::string target;     // Target state of event
    std::string id;         // ID of event
    double Simulation_time = 0; // Time of event in the simulation
    double execution_time = 0;  // Execution time of event
    double rate = 0;        // Rate of exponential event
    double lower = 0;       // Lower bound of uniform events's time distribution
    double upper = 0;       // Upper bound of uniform events's time distribution
    double previous_time = 0;   // Time of previous event
    double interval = 0;    // Interval at which discrete event take place
    double mu = 0;          // Mean of normal event's time distribution
    double sigma = 0;       // Standard deviation of normal event's time distribution
    std::vector<std::pair<double, double>> dist_pt;  // Distribution points of mixed(merged) event's time distribution
    Event() = default;
    // Member functions
    // Constructor with ID, type, source, and target parameters
    Event(const std::string& id, const std::string& type, const std::string& source, const std::string& target);
    // Generate a random number between 0 and 1
    double ran();
    // Template member function that returns the time of the  event.overridden by derived classes get_event_time()function
    template<typename ...Args>
    double get_event_time(Args... more) { return 0; }
    // Setter functions
    void set_executiontime(double s) { execution_time = s; }
    void set_previoustime(double s) { previous_time = s; }
    void set_rate(double rate) { rate = rate; }
    // Getters for member variables
    const double& get_rate() const { return rate; }
    const double& get_executiontime() const { return execution_time; }
    const double& get_lower() const { return lower; }
    const double& get_upper() const { return upper; }
    const double& get_mu() const { return mu; }
    const double& get_sigma() const { return sigma; }
    const std::string& get_id() const { return id; }
    const std::string& get_state() const { return source; }
    const std::string& get_nextstate() const { return target; }
    const std::string& get_type() const { return type; }
    const double& get_interval() const { return interval; }

    // Virtual destructor
    virtual ~Event() {}
};
#endif /* EVENT */
