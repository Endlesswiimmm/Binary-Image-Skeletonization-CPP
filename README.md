# Binary-Image-Skeletonization-CPP




# Morphological Image Processing: Binary Skeletonization and Iterative Pruning

This repository contains a high-fidelity C++ implementation of morphological thinning and noise reduction algorithms, developed as part of the **Analysis and Design of Algorithms** curriculum at **Universidad de Monterrey (UDEM)**.

## 1. Project Overview

The objective of this system is to extract the topological skeleton of binary shapes within a 2D discrete grid. The process involves two primary stages: 
1. **Topological Thinning**: Reducing the shape to a unitary width while preserving connectivity.
2. **Greedy Pruning**: An optimization stage designed to eliminate peripheral noise and artifacts.



## 2. Algorithmic Foundation

### 2.1 Zhang-Suen Thinning Algorithm
The core engine utilizes the **Zhang-Suen algorithm**, a parallel iterative method. It evaluates the 8-neighborhood of each pixel $(P_1)$ to determine its redundancy.

A pixel is marked for removal only if it satisfies the following criteria across two distinct sub-iterations:
* **Connectivity Number ($A(P_1) = 1$):** Ensures the global topology and holes remain intact.
* **Neighbor Density ($2 \le B(P_1) \le 6$):** Prevents the deletion of endpoints or interior regions.
* **Directional Boundary Conditions:** Alternates between North-East and South-West boundary checks to maintain structural symmetry.

### 2.2 Iterative Greedy Pruning
To refine the resulting skeleton, a **Greedy approach** is applied. The algorithm identifies "leaf nodes" (pixels with degree $\le 1$) and prunes them iteratively. This local optimization ensures that any parasitic branches or digital noise are retracted until the skeleton reaches a stable state of convergence.

## 3. Complexity Analysis
The system's efficiency is defined by a temporal complexity of $O((K + P) \cdot N \cdot M)$, where $N \times M$ represents the discrete grid dimensions, and the coefficients $K$ and $P$ account for the iterative depth of the thinning and pruning phases, respectively. This demonstrates a linear dependency on the total pixel count and the morphological thickness of the input shapes. Spatially, the algorithm operates within an $O(N \cdot M)$ bound, necessitated by the 2D matrix representation and the auxiliary coordinate buffers used to ensure atomic pixel updates during each iteration.

## 4. Technical Specifications

* **Language**: C++11 (or higher).
* **Frameworks**: None (Pure Standard Template Library - STL).
* **Memory Management**: Utilizes `std::vector` for dynamic memory allocation and efficient 2D matrix manipulation.

---

## Author
**Gabriel Ortiz Sepúlveda** *Student of Computer Technologies Engineering (ITC)* *Universidad de Monterrey (UDEM)* **Academic Supervisor**: Dr. Eduardo Emmanuel Rodríguez López  
**Date**: March 24, 2026
