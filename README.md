# BPA Triangle Mesher

## Abstract:
Generating a triangle mesh from a point cloud set is a difficult task. 
Different algorithms try to solve this issue in different ways. 
One such approach is the Pivoting Ball Algorithm (PBA), which constructs a mesh by iteratively rolling a virtual ball over a point cloud to detect and connect neighbouring points, forming triangles that approximate the surface of the object. 
The algorithm's computational demands, particularly for large datasets, make it less efficient when implemented serially on a CPU. 
In this guided research it will be attempted to speed up the algorithm by implementing it on the GPU utilising the higher parallel computational performance compared to the CPU.

## Refference Papers

[F. Bernardini, J. Mittleman, H. Rushmeier, C. Silva and G. Taubin, "The ball-pivoting algorithm for surface reconstruction," in IEEE Transactions on Visualization and Computer Graphics, vol. 5, no. 4, pp. 349-359, Oct.-Dec. 1999, doi: 10.1109/2945.817351.
keywords: {Surface reconstruction;Image reconstruction;Shape;Three-dimensional displays;Sampling methods;Geometrical optics;Product design;Data acquisition},](https://ieeexplore.ieee.org/abstract/document/817351)

[Digne, J. (2014). An analysis and implementation of a parallel ball pivoting algorithm. Image Processing On Line, 4, 149-168.](https://www.ipol.im/pub/art/2014/81/)

[Saffi, H., Otberdout, N., Hmamouche, Y., & El Fallah Seghrouchni, A. (2024). Auto-BPA: An Enhanced Ball-Pivoting Algorithm with Adaptive Radius using Contextual Bandits. In Proceedings of the IEEE/CVF Winter Conference on Applications of Computer Vision (pp. 3729-3737).](https://openaccess.thecvf.com/content/WACV2024/html/Saffi_Auto-BPA_An_Enhanced_Ball-Pivoting_Algorithm_With_Adaptive_Radius_Using_Contextual_WACV_2024_paper.html)