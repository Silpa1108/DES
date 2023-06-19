#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include"json.h"
#include"system.h"
/*The class has function called write_into_jsonfile which takes a System object as input,
 creates JSON objects to represent the system and
its components performance metrics obtained after doing statistical calculation on system and component sojourn data, and writes the resulting JSON objects to
JSON files that can be used at frontend for doing system and component
performance evaluation.*/
namespace nh = nlohmann;
class json_writer
{
public: //nh::json j;
    std::vector <nh::json> alljson_system;
    std::vector <nh::json> alljson_component;

    void write_into_jsonfile(System system);

};


#endif