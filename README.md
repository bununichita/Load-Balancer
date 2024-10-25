
# C Load Balancer Implementation

### Author: Nichita-Adrian Bunu, 323CA Facultatea de Automatica si Calculatoare UNSTPB 
**Contact:** [nichita_adrian.bunu@stud.acs.upb.ro](mailto:nichita_adrian.bunu@stud.acs.upb.ro)

---

## Overview

This project implements a basic load balancer in C, which distributes requests across multiple servers based on a hash function. It includes functionality to add and remove servers, store and retrieve data, and balance the load across servers. The code is modular, with distinct functions for managing the load balancer, servers, and data redistribution.

---

## Features and Functionality

- **Load Balancer Initialization**: Creates and initializes a new load balancer with default settings.
- **Hashing Functions**: Includes custom hash functions for servers and keys.
- **Server Management**:
  - Add and remove servers dynamically.
  - Redistribute data when servers are added or removed.
- **Data Management**:
  - Store data on the appropriate server based on the hash of the key.
  - Retrieve data from the servers using a key.
- **Load Balancing Logic**: Distributes the load across servers and handles data migration during server addition/removal.

---
