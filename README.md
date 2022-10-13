# Assignment 2

## TODO

### How to run
```
make     // builds project
make run // runs project
```

### 1.0) Simulating the seafloor seismic sensor node
- [x] a) A single MPI process simulates one sensor node
- [x] b) Each node periodically produces a simulated sensor reading.
- [x] c) send a request to its immediate adjacent neighbourhood nodes to acquire their readings for comparison purposes.
- [x] d) Upon receiving the readings from its neighbourhood nodes, the node compares these readings to its own reading to check if the readings are similar
- [ ] e) Should the difference in location and magnitude from at least two or more neighbourhood nodes
match the readings of the local node (within a threshold which you can determine), the node sends a
report (i.e., alert) to the base station. 
- [ ] f) The report sent to the base station should contain as much information as possible about the possible
alert. 
- [ ] g) Each node repeats parts (a) to (f) until upon receiving a termination message from the base station.
Once the node receives a termination message, the node cleans up and exits.
- [ ] h) Simulation should work for dynamic value of m x n nodes and threshold settings. At start up, the
program allows the user to specify the grid size (m x n) and threshold values.
- [ ] i) If you are aiming for HD or upper HD: The node uses a thread (i.e., POSIX or OPENMP) to send or
receive MPI messages between its adjacent nodes. This thread is created by the sensor node and
terminates properly at the end of the program.

### 2.0) Simulating the balloon seismic sensor
- [x] a) (you may opt to use OpenMP as an alternative to POSIX thread). Note that the balloon covers the
same area of the seafloor seismic sensors. For simulation purposes, this thread is created by the base
station node (refer to point 3.0 below for details about the base station).
- [x] b) This thread periodically produces seismic readings. This reading consists of date, time, latitude and
longitude of the earthquake point, magnitude, and depth of the quake from the sensor. However, the
generated magnitude always exceeds the predefined threshold (e.g., magnitude > 2.5). 
- [x] c) The information in (b) is stored in a shared global array, which can also be accessed by the base
station node. The array has a fixed size, and you can decide the size of this array. Once the array is
full, the thread removes the first entered data from the array to make way for the new data (first in, first
out approach).
- [ ] d) The thread repeats (a) to (c) until upon receiving a termination signal from the base station. Once the
thread receives a termination signal, the thread exits.

### 3.0) Simulating the base station
- [ ] a) A single MPI process simulates the base station node.
- [ ] b) The base station node also creates the thread which simulates the balloon seismic sensor (refer to
point 2.0 above).
- [ ] c) The base station node periodically listens for incoming reports from the seafloor seismic sensor
nodes.
- [ ] d) Upon receiving a report from a sensor node, the base station compares the received report with the
data in the shared global array (which is populated by the balloon seismic sensor). i. If there is a match, the base station logs the report as a conclusive event (conclusive alert). ii. If there is no match, the base station logs the report as an inconclusive event (inconclusive
alert).
- [ ] e) The base station writes (or logs) the key performance metrics (e.g., the simulation time, number of
alerts detected, number of messages/events with senders’ adjacency information/addresses, total
number of messages (for this simulation)) to an output file. 
- [ ] f) The base station program allows the user to specify a sentinel value at runtime to allow a proper shutdown of the base station, balloon sensor and sensor nodes. Note: CTRL+C is not allowed.
- [ ] g) Continuing from (f), the base station sends a termination message to the sensor nodes to properly
shutdown. The base station also sends a termination signal to the balloon seismic sensor to properly
shutdown.
- [ ] h) If you are aiming for HD or upper HD: The base station uses a thread (i.e., POSIX or OPENMP) to
send or receive MPI messages from the sensor nodes. This thread is created by the base station and
terminates properly at the end of the program.
