# DES
Discrete event simulation for OpenPRA

DES for OpenPRA is project developed  for the Probabilistic Risk Analysis (PRA) of complex systems.The project is developed in C++ using an Object-oriented programming approach, is capable of computing performance metrics such as uptime, downtime,availability, Mean time between failures(MTBF) and reward value calculated for a wide range of system models.The tool takes an input file in XML format, which contains information about the system and its components, as well as the performance metrics that need to be calculated. The tool generates two output files in JSON format: system_output.json contains system-level performance metrics, and component_output.json contains component-level performance metrics obtained over Monte Carlo simulation.

Input File:

The input file should be in XML format and contains information about the system and its components and also has the information about performance metrics that has to be calculated . A sample of input file can be seen in input_test.xml.

Output File: 

The tool will generate two output files in JSON format: system_output.json contains system-level performance metrics, and component_output.json: contains component-level performance metrics obtained over montecarlo simulation. These file can be used at the frontend to do performance evaluation of sysstem.
 

Docker:
The project is also dockerized. To run using docker first a docker image has to build
Build the Docker image using the following command:

docker build -t <image_name> . 

To build using docker-compose.yaml file in Visual studio: 

In command prompt give: 

docker-compose up --build

Command for running the docker container in VScode:

docker run -it <image_name>

To run docker using docker-compose.yaml file in visual studio:

docker-compose -it <image_name>

Command to start the container if exited:

docker start <container_id or name>

Running the Program:
When the program prompts to enter the input, enter the path to the input file. The program will read the input file and check if it is in the correct format. If the input file is not in the correct format the program exists. If the input file is in the correct format, the program will prompt you to enter whether you want to implement state space optimization techniques. If  "y" is entered the program will provide a list of 2 optimization techniques that can be applied. Select the desired technique, and the program will implement it.
After the optimization technique is applied, a reduced system object is created. Then montecarlo simulation of engine on system object is carried out. The program will finally generate two output files in JSON format: system_output.json and component_output.json. These files will be saved in the working directory inside the Docker container.

To copy the output files from the Docker container to local machine, the docker cp command can be used. For example, if the output files are saved in the working directory inside the container,the following command can be used to copy them to the current directory on local machine:


docker cp <container_id or name>:/Container_Working_directory/system.json /path_to/local_folder
docker cp <container_id or name>:/Container_Working_directory/component.json /path_to/local_folder

Note: Replace <containerId> with the ID of the Docker container ,also the container should be running to access the output files from container directory. 
The output files can then be used for performance analysis at the front end.

Dependencies:
This program also uses the TinyXML and nlohmann json libraries for parsing the input file and generating the output files, respectively. These libraries are linked statically in the program.

Components of DES:

The project is structured with several classes, including a Event class and a State class, both of which are abstract base classes with multiple child classes. The project also includes a System class, which maintains a list of Component classes and Engine class and PES (pending event list ) class to manage the events in the system.
Event Classes
The Event class is an abstract base class with several child classes that implement specific types of transitions of different probability distributions such as exponential, uniform, normal and events at discrete time intervals.
State Classes
The State class is an abstract base class with several child classes that represent specific states in the system, such as “transient” or “absorbing” or “hidden”states.
System Class
The System class is responsible for maintaining a list of Component object , pathsets and results of statistical calculations.
Component Classes 
Class for the components inside the system.
DFS Class
The DFS class is responsible for finding pathsets in the system using depth first search algorithm.
StatisticalCalculation Class
The Calculation class is responsible for performing staistical calculations on sojourn information of the system and its components obtained over monte carlo simualtion.
The performance metrices such as availability, uptime,downtime,MTBF and rewards at system level and component level are considered here.
