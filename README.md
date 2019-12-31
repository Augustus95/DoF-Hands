DoF-Hands
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
