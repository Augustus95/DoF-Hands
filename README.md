[DoF-Hands](https://github.com/JenathanHoo/DoF-Hands)
=========================
This repository is the implements of DoF parameteried hand.
- **GPU-TensorFlow** version.
- **CPU-OpenGL** version.

Feature
=========================
In two versions, a single hand has 27 DoFs = **4*4+5+6**.

- **Finger*4**: where 4 in each finger, 3 for extension and flexion and one for abduction and adduction.
- **Thumb*1**:  the thumb has 5 dofs, where 3 for extension and flexion and 2 for abduction and adduction.
- **Wrist*1**:  rotation and translation of the wrist.

Please note that our model is different from [here](https://biology.stackexchange.com/questions/30857/does-the-human-hand-have-27-degrees-of-freedom) and [here](http://faculty.cs.tamu.edu/jchai/projects/sigasia_2013/grasp_asia_final.pdf), but you can change the code easily.

Usage
=========================
**GPU-TensorFlow** version

 Main function：

 **def Bone_Transformation_Matrix(df, length, const_joint):**
- **df**(Nx1x25): hand dof input.
- **length**(Nx1x15): bone length input.
- **const_joint**(Nx1x15): initial palm position, where wrist is (0, 0, 0).
- **return RT**(Nx16x4x4): dof-converted RT matrix (5x3+1), where 5x3 crossponds 5 fingers, the last one is wrist RT.
    
**CPU-OpenGL** version.
- **All 27 dofs**:  **4*4+5+6**, please refer to [Feature](https://github.com/JenathanHoo/DoF-Hands), keys crosspond to different dof.
- **'Space' key**: change hand appearance, please refer to [Mano-Textures](https://github.com/JenathanHoo/Mano-Textures).
- **Automatically save** img and crossponding GT labels: including 27 dofs + 2D joints + 3D joints + appearance.
- **Hand interaction**: hand interaction dataset generation, updating...

Snapshot
=========================
<center class="half">
    <img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/1.png" width="900"/><img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/223.png" width="900"/><img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/222.png" width="900"/>
</center>


Links
=========================
[1] Official Website of MANO: [https://mano.is.tue.mpg.de.](https://mano.is.tue.mpg.de) 

[2] Official Website of SMPL: [http://smpl.is.tue.mpg.de.](http://smpl.is.tue.mpg.de) 

[3] Official Website of SMPLify: [http://smplify.is.tue.mpg.de.](http://smplify.is.tue.mpg.de) 

[4] DOF-Parameterized Hands: [http://faculty.cs.tamu.edu/jchai/projects/sigasia_2013/grasp_asia_final.pdf](http://faculty.cs.tamu.edu/jchai/projects/sigasia_2013/grasp_asia_final.pdf)
