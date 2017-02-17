# Real-time object scanning and manipulation in the CAVE

# Abstract

Since the introduction of the CAVE in 1993, there hasn’t been much work done on rendering scanned small objects in real time. I am proposing real time application inside the CAVE environment that would allow scanning small objects on a custom made turntable using a Real Sense camera that would capture the objects information creating a mesh file (OBJ) that would be rendered instantly inside the CAVE allowing the user to manipulate the object using a Leap Motion Sensor. The proposed work might open the way for some other endeavors and future work especially in the field of engineering and design.

# Introduction

A lot of work has been done in the area of 3D scanning through high resolution depth cameras specially generating meshes from objects being recognized as iso-surfaces through using the famous Marching Cube algorithm or other algorithms that has been adopted in the recent years by projects like MeshLab. Recently an idea has been introduced to capture scanned objects based on light field rendering through creating billboard rendered images inside the VR environment in real-time, this method is computationally cheap and efficient however due to the lack of resources it will be hard to implement in the current project. This work does not intend to explore the problems in the field of 3D scanning is surfaces but rather to explore and introduce other ways to capture objects and render them in real-time inside cluster based VR environments like the CAVE. 
Thanks to Intel’s Real Sense provided SDK it is made easy to scan a relatively small object and generating an OBJ file that can be parsed and rendered using OpenGL. Integrating this real-time render inside the CAVE using a C++ library called VR-Juggler which provides a stereo synchronized windowing system that operates on cluster based systems like the CAVE which usually run on quad buffered graphics cards giving a smooth and consistent frame rate and viewport for stereo images.
VR-Juggler already allows introducing inputs that can either be tracked using the cameras that are already introduced in the system, however introducing Leap Motion in the CAVE might be a first but will throw a whole set of challenges that I will have to face in order to get the system smoothly running.
My pipeline starts with placing an object on a turntable that is positioned adjacent to the CAVE and by running the program a message will appear directing the user inside the CAVE that an object is currently being scanned, the object will appear instantly inside the environment where a Leap Motion sensor is introduced inside the CAVE, then user will be able to manipulate the object in the virtual space.

# Figures

![CAVESCANNER](https://github.com/iebeid/realtime-cave-scanner/blob/master/1.jpg "CAVESCANNER")

![CAVESCANNER](https://github.com/iebeid/realtime-cave-scanner/blob/master/2.jpg "CAVESCANNER")

![CAVESCANNER](https://github.com/iebeid/realtime-cave-scanner/blob/master/3.png "CAVESCANNER")

![CAVESCANNER](https://github.com/iebeid/realtime-cave-scanner/blob/master/4.png "CAVESCANNER")

# References

1.	CRUZ-NEIRA, C., SANDIN, D. J., & DEFANTI, T. A. (1993). Surround- screen projection-based virtual reality: the design and implementation of the CAVE. SIGGRAPH 1993 Proceedings.
2.	CRUZ-NEIRA, C., Morillo, P., Hartling, P., Bierbaum, A. implementing immersive clustering with VR juggler. ICCSA'05 Proceedings of the 2005 international conference on Computational Science and Its Applications - Volume Part III Pages 1119-1128.
3.	Mark Bolas*, Ashok Kuruvilla, Shravani Chintalapudi, Fernando Rabelo, Vangelis Lympouridis, Christine Barron, Evan Suma, Catalina Matamoros, Cristina Brous, Alicja Jasina, Yawen Zheng, Creating Near-Field VR Using Stop Motion Characters and a Touch of Light-Field Rendering. SIGGRAPH 2015 Poster Proceedings.
4.	Intel Real Sense Technology. http://www.intel.com/content/www/us/en/architecture-and-technology/realsense-depth-camera.html
5.	Leap Motion Sensor. https://www.leapmotion.com/
6.	P. Cignoni, M. Callieri, M. Corsini, M. Dellepiane, F. Ganovelli, G. Ranzugli, MeshLab: an Open-Source Mesh Processing Tool. Eurographics Italian Chapter Conference (2008)
