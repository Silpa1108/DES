# Use an official g++ runtime main image
FROM gcc:latest
# Set the working directory to /DES
WORKDIR /DES
#Copy the current directory contents into the container at /DES
COPY . /DES
#to compile the application using gcc.
RUN  g++ -o main ../DES/DiscreteEventSimulation/*.cpp 
# the default command to execute when the container starts
CMD ["./main"]
