# SensorNet

**Description:**  
This project simulates a wireless sensor network where nodes detect events and forward queries to the node that detected them using the rumor-routing algorithm. Each node maintains a routing table with event locations and distances, allowing queries to reach the correct node efficiently. I worked on this project in a group with three other students.


**Features:**  
- Nodes detect events and generate unique IDs with timestamps and coordinates
- Rumor-routing algorithm for efficient event propagation and query forwarding
- Agent messages carry event routing information through the network
- Queries find events and return responses along the query path
- JUnit tests ensure the correctness of key program components
- Supports custom layouts loaded from file
  
**Run Instructions:**
  
1. **Navigate to the source folder**:
   ```bash
   cd path/to/SensorNet/src

3. **Compile the program**
   ```bash
   javac Simulation.java

2. **Run the program**:
   ```bash
   java Simulation testNetwork.txt

**Author:** 
Joakim Wiksten (and classmates)
