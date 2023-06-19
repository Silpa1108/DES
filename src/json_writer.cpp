#include"json_writer.h"
/*Two  JSON file are generated as outputfile here. JSON file component has structure of a nested dictionary where the keys are the names of the components, and the values are dictionaries
that contain the array of attributes and rewards of the components across moltecarlo simulations.
system.json file contain  JSON object with key-value pairs for each attribute and reward type in the system and
is stored as array containing values of each simulation in montecarlo*/
void json_writer::write_into_jsonfile(System system)
{
    nh::json j, json_comp, json_system, k;
    //write system details to json file
    std::ofstream os("system_output.json");

    if (!os) {
        throw std::runtime_error("Unable to open the system output file.");
    }
    for (auto& calculate : system.system_calculation)
    {
        //for system object, there are key-value pairs for each attribute and reward type in json file 
        if (calculate.parameter_type == "attribute")
        {

            json_system[calculate.name].push_back(calculate.result);
        }
        if (calculate.name == "profit")
        {//key is the string name here
            std::string name = std::to_string(calculate.value) + "/ " + std::to_string(calculate.time) + "/" + calculate.reward_type;
            json_system["profit"][name].push_back(calculate.result);
        }
    }
    os << std::setw(4) << json_system;
    os.close();

    //the json_comp object is written to the output file stream oc
    // resulting in a JSON file containing information about the components and their attributes/rewards.
    std::ofstream oc("component_output.json");
    //checks whether the file has been successfully opened. 
    if (!oc) {
        throw std::runtime_error("Unable to open the system output file.");
    }
    for (auto i : system.components)
    {
        if (i->type != "node") {
            //for each component object, there are key-value pairs for each attribute ,
            // with the key being a attribute name 
            //and the value being an array of values for that attribute or reward type across multiple Monte Carlo simulations

            for (auto k : i->component_calculate)
            {//for writing component attributes
                if (k.parameter_type == "attribute")
                {
                    json_comp[i->name][k.name].push_back(k.result);
                }

            }
            //for each component object, there are key-value pairs for each reward type,
            // with the key being a concatenation of the reward name, reward type, state transition, and type (e.g. "reward1/reward_type1/state_transition1/type1"), 
            //and the value being an array of values for that attribute or reward type across multiple Monte Carlo simulations
            for (auto j : i->component_rewards)
            {//for writing component rewards to json file
                std::string name = j.name + "/" + j.reward_type + "/" + j.state_transition + "/" + j.type;
                json_comp[i->name][name].push_back(j.result);
            }


        }


    }

    oc << std::setw(4) << json_comp;
    oc.close();
}
