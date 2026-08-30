# GCom

A distributed group communication middleware developed as part of the Distributed Systems course at Umeå University.

GCom provides reliable and non-reliable multicast communication between processes, dynamic group management, and causal message ordering. The system is demonstrated through a distributed chat application and a debugging tool for simulating network conditions.

## Features

- **Distributed group communication** using gRPC
- **Dynamic group management**, including joining and leaving groups
- **Reliable and non-reliable multicast**
- **Unordered and causal message ordering**
- **Causal ordering** using vector clocks
- **Naming service** for group discovery and creation
- **Debugging and visualization** of message propagation, buffers, vector clocks, and system statistics
- **Distributed chat application** for testing and demonstrating the middleware

## Architecture

GCom consists of three main modules:

- **Group Management** – Handles group membership, group discovery, and membership changes.
- **Communication** – Provides reliable and non-reliable multicast communication using gRPC.
- **Message Ordering** – Ensures that messages are delivered according to the selected ordering semantics.

A separate **Naming Service** is used to facilitate group creation and discovery.

## Technologies

- **Java**
- **gRPC**
- **JavaFX**
- **Maven**
- **Git**

## Repository
[GitHub Repository](https://github.com/algotgraner/GCom)

## Authors
Joakim Wiksten, Algot Eriksson Graner and Jonatan Westling

