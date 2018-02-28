#### Zheng Xiangyue G42416206

#### Assignment 1 - Perspective transform

![Feb-02-2018 16-01-26](<https://raw.githubusercontent.com/Zhengxiangyue/graphicsAssignments/master/result/Knight.gif>)

#### Project description :

The code reads a 3d model file and shows the object on the screen using perspective transformation.

#### The main work includes :

###### 3D point and its operation

-  point subtraction

###### 3D vector and its operation

-  cross production
-  the dot product
-  subtraction
-  addition
-  division
-  mold

###### Matrix and its operation

-  dot production
-  cross production
-  transposition

#### Result :

\+ Own math library

\+ The final object is shown as a mesh object

\+ The back face is removed.

\- The covered front face is not removed.

\- The normal is not perfectly compatible when there are concave polygons.

Some images: <https://github.com/Zhengxiangyue/graphicsAssignments/tree/master/result>

#### Input :

1. The object file, locate at "assets/D/"
2. The camera information, locate at "assets/camera_position"

#### Build instruction :

Use CMake to build



#### Assignment2 - Scan conversion + Z buffer

![bench](https://raw.githubusercontent.com/Zhengxiangyue/graphicsAssignments/master/result/BenchAndCone.gif)

In assignment2, the scan line algorithm and z-buffer algorithm are implemented.

The implementation of simple illumination model:

![knight2.gif](https://github.com/Zhengxiangyue/graphicsAssignments/blob/master/result/Knight2.gif?raw=true)