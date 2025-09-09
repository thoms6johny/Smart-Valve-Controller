# Smart Valve Controller Project

(This is a personal learning project created independently in my own time.)

## 📌 Overview
This repository contains two related embedded systems projects:
1. **Water Valve Controller**  
   Controls two solenoid valves based on AC power availability and coordinates with a pump controller via wireless communication. Includes backup battery failover for reliability.

2. **Gas Valve Controller**  
   Uses a non-contact liquid level sensor to detect low water levels in a container and shut down a gas line valve for safety.

Both systems are based on **ESP32 microcontrollers** and were developed as part of a learning/prototyping journey in embedded systems, power management, and IoT communication.

---

## 🛠 Features
- Automatic **AC/Battery failover** using a power-path controller.
- **Water valve switching** between primary (pump) and backup (gravity-fed) sources.
- **Wireless communication** between controllers for pump coordination.
- **Gas line shutoff** triggered by non-contact water level sensing.
- Status indicators for power availability and system readiness.
- Open-source design: schematics, PCB layouts, Gerbers, and firmware included.

---

## 📂 Repository Structure
- `water-valve-controller/` → Code, schematics, PCB & Gerbers for the water system.  
- `gas-valve-controller/` → Code, schematics, PCB & Gerbers for the gas system.  
- `Images/` → Prototype photos and diagrams.  
- `Docs/` → Additional documentation and notes.  

---

## 🚀 Getting Started
1. Flash the code in `Code/` to an ESP32-C6 or ESP32-S3 Mini.  
2. Assemble hardware as per the schematics.  
3. Power using 12V SMPS or 3S Li-ion battery pack.  
4. Test AC failover by toggling AC input and monitoring valve behavior.

---

## 📷 Prototype
![IMG20250825220529](https://github.com/user-attachments/assets/ed84ee83-baf1-4ad7-a9ab-3d0e0d52c419)
![IMG20250825220536](https://github.com/user-attachments/assets/9a97c550-537c-4db8-8acb-4bdbd9eebe59)
![IMG20250825220821](https://github.com/user-attachments/assets/3d2d70bf-929e-4c19-8255-78b0a606b6c6)


---
 

---

## 📜 License
This project is shared under the MIT License for learning and exploration. See LICENSE for details.
