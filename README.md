# ipmi_sim_temperature_loggerr

compile using g++:
g++ met.cpp -o met

it accepts arguments: port, IP, sensor name and number of iterations:
Usage: met [-p port] [-H host] [-s sensor] [-i iterations]

for example: 
./met -p 9001 -H 127.0.0.1 -s MBTemp -i 10

by default (if no arguments given)
port: 9001
IP: 127.0.0.1
sensor name: MBTemp
number of iterations: 5
