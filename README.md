DoF-Hands
This project is a DoF implement of the human hand  ( GPU-TF version && CPU-OpenGL version ). 

**Note**: In this project, hand has 27 DoFs = 4*4+5+6
=========================
- **Finger*4**: 4 in each finger, where 3 for extension and flexion and one for abduction and adduction.
- **Thumb*1**: the thumb is complicated and has 5 dofs, where 3 for extension and flexion and 2 for abduction and adduction.
- **Wrist*1**: rotation and translation of the wrist.
please note that our model is different from https://biology.stackexchange.com/questions/30857/does-the-human-hand-have-27-degrees-of-freedom, but you can change our model easily.
