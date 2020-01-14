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

Please note that our model is different from [here](https://biology.stackexchange.com/questions/30857/does-the-human-hand-have-27-degrees-of-freedom), but you can change the code easily.

Usage
=========================
**GPU-TensorFlow** version

Main function：

def Bone_Transformation_Matrix(**df**, **length**, **const_joint**):
- **df**(N*1*25): hand dof input.
- **length**(N*1*15): bone length input.
- **const_joint**(N*1*15): initial palm position, where wrist is (0, 0, 0).
- **return RT**(N*16*4*4): dof-parameteried RT matrix (5*3+1), where 5*3 crossponds 5 fingers, the last one crosspond wrist RT.
    
**CPU-OpenGL** version.
- **All 27 dofs**:  **4*4+5+6**, please refer to [here](https://github.com/JenathanHoo/DoF-Hands), different keys crosspond dof index.
- **'Space' key**: change hand appearance, please refer to [Mano-Textures](https://github.com/JenathanHoo/Mano-Textures).
- Automatically **save** img and crossponding pose GT: including 27 dofs + 2D joints + 3D joints + appearance.
- **Hand interaction**: hand interaction dataset generation, updating...

Snapshot
=========================
<center class="half">
    <img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/1.png" width="900"/><img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/223.png" width="900"/><img src="https://github.com/JenathanHoo/DoF-Hands/blob/master/imgs/222.png" width="900"/>
</center>



