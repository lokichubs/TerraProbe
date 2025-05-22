# TerraProbe - Code Repository

This project's goal is to create an autonomous robot that collects an intact soil core of 12 inches. The primary goal is to be used in agriculture. This repository contains the modeling, motor control, data acquisition, and dashboard code for the same. Below is the product demonstration:

## Product Demonstration

### 🔩 Burrowing Mechanism
Collects intact soil core of up to 12"  
![Burrowing](Documentation/FDR/Gifs/Burrowing.gif)

### 🪫 Core Extraction
Can be operated and collected by a single person. Designed for intuitive use.  
![Extraction](Documentation/FDR/Gifs/Extraction.gif)

### 📊 Data Collection
Slits facilitate depth profiling of NPK and moisture data.  
![Data Collection](Documentation/FDR/Gifs/Data_Collection.gif)

### 📉 Dashboard Interface
Dashboard allows visualization of collected data and prediction for best crop to grow in collected soil.  
![Dashboard](Documentation/FDR/Gifs/Dashboard.gif)

---

The full design for the same was done in 3 stages:

- [PDR](#preliminary-design-review)
- [CDR](#critical-design-review)
- [FDR](#final-design-review)

## Preliminary Design Review (PDR)
This phase involved concept generation, market analysis, initial technical analysis, and economic considerations.  
Documentation can be found [here](Documentation/PDR)

## Critical Design Review
This phase involved refining ideas agreed upon in PDR and creating CAD models with design for manufacture (DFM) and assembly (DFA) in mind. First iterations of mechanisms, electronics, and software were created.  
Documentation can be found [here](Documentation/CDR)

## Final Design Review
This phase revolved around realizing our final prototype — the soil sampler (TerraProbe), the data acquisition system (TerraPal), and the Dashboard (TerraSoilIQ).  
Documentation can be found [here](Documentation/FDR)

The final versions of the motor control,DAQ,and dashboard code can be seen below
- [Motor Control](#motor-control)
- [DAQ](#data-acquisition)
- [Dashboard](#dashboard)