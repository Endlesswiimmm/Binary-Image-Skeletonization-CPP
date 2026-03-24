# Binary-Image-Skeletonization-CPP

## Technical Overview: Image Skeletonization & Greedy Pruning

This project implements a robust pipeline to reduce binary shapes to their topological skeletons. This process is essential in fields like computer vision, OCR, and even game development (e.g., generating navigation meshes or hitboxes).

### 1. Data Representation
The "image" is treated as a **2D Matrix** (`std::vector<std::vector<int>>`). 
* **0** represents the background (empty space).
* **1** represents the foreground (the object).

### 2. The Zhang-Suen Thinning Algorithm
The core of the program is the **Zhang-Suen algorithm**, an iterative parallel thinning process. It works by "peeling" the outer layers of a shape until only a 1-pixel wide center remains.

It examines the **8-neighborhood** of every pixel to decide if it can be safely removed without breaking the structure.



**The process is divided into two sub-iterations:**
* **Sub-iteration 1:** Identifies and removes pixels on the South-East boundaries and North-West corners.
* **Sub-iteration 2:** Identifies and removes pixels on the North-West boundaries and South-East corners.

**Deletion Criteria:**
For a pixel to be removed, it must satisfy four conditions simultaneously:
1.  **Connectivity:** It must not be a "bridge" (removing it shouldn't split one shape into two).
2.  **Density:** It must have between 2 and 6 neighbors (ensuring we don't delete isolated dots or ends of lines prematurely).
3.  **Boundary Check:** Specific checks on adjacent pixels to ensure the thinning is symmetrical.

### 3. Greedy Pruning (Noise Reduction)
After thinning, some "artifacts" or "hairs" (noise) might remain. We use a **Greedy approach** to clean this up:
* The algorithm scans the skeleton and identifies pixels with **one or zero neighbors**.
* In a "greedy" fashion, it makes an immediate local decision to prune these pixels, as they are likely noise or "dead branches" that don't contribute to the main topology.

### 4. Complexity Analysis
* **Time Complexity:** $O(K \cdot N \cdot M)$, where $N \times M$ is the image size and $K$ is the number of iterations. $K$ is proportional to the thickness of the widest object in the image.
* **Space Complexity:** $O(N \cdot M)$ to store the matrix and the auxiliary list of pixels marked for deletion.


