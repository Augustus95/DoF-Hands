#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Thu Dec  6 12:56:21 2018

@author: lhc
"""

import numpy as np
import tensorflow as tf
import os
import json
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import scipy.misc

def Bone_Transformation_Matrix(df,length,const_joint):
    """
    df : N 1 25
    length : N 1 15
    const_joint : N 1 15
    
    return : RT N 16 4 4
    """
    
    RT_all = tf.constant([1,0,0,0,  0,1,0,0,   0,0,1,0,   0,0,0,1],shape = [1,1,4,4],dtype=tf.float32)
    RT_all = tf.tile(RT_all,[df.shape[0],1,1,1])
    
    
    sin_df = tf.sin(df)
    cos_df = tf.cos(df)
    
    #N 1 77
    sc_df = tf.concat([tf.constant([0],shape=[df.shape[0],df.shape[1],1],dtype=tf.float32),
                       tf.constant([1],shape=[df.shape[0],df.shape[1],1],dtype=tf.float32),
                       sin_df,cos_df,-sin_df,
                       ],-1)
    sc_df =tf.transpose(sc_df,[2,1,0]) #77 1 N
    
    sc_t = tf.concat([tf.constant([0],shape=[df.shape[0],df.shape[1],1],dtype=tf.float32),
                      tf.constant([1],shape=[df.shape[0],df.shape[1],1],dtype=tf.float32),
                      const_joint,length,
                      ],-1)
    sc_t = tf.transpose(sc_t,[2,1,0]) # 32 1 N
    
    for i in range(5):
    
        
        #coord = tf.constant([0,0,0,1],shape=[1,1,4,1])
        #coord = tf.tile(coord,[df.shape[0],1,1,1])
        
        J_1_mat_x_ind = tf.constant([[1],[0],[0],[0],  [0],[2+i*5+25],[2+i*5+50],[0],   [0],[2+i*5],[2+i*5+25],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_1_mat_y_ind = tf.constant([[2+i*5+1+25],[0],[2+i*5+1+0],[0],  [0],[1],[0],[0],   [2+i*5+1+50],[0],[2+i*5+1+25],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_1_mat_z_ind = tf.constant([[2+i*5+2+25],[2+i*5+2+50],[0],[0],  [2+i*5+2],[2+i*5+2+25],[0],[0],   [0],[0],[1],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_2_mat_x_ind = tf.constant([[1],[0],[0],[0],  [0],[2+i*5+3+25],[2+i*5+3+50],[0],   [0],[2+i*5+3],[2+i*5+3+25],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_3_mat_x_ind = tf.constant([[1],[0],[0],[0],  [0],[2+i*5+4+25],[2+i*5+4+50],[0],   [0],[2+i*5+4],[2+i*5+4+25],[0],  [0],[0],[0],[1]],dtype=tf.int32)
    
        R_1_mat_x = tf.transpose(tf.gather_nd(sc_df,J_1_mat_x_ind),[2,1,0]) #N 1 16
        R_1_mat_y = tf.transpose(tf.gather_nd(sc_df,J_1_mat_y_ind),[2,1,0]) 
        R_1_mat_z = tf.transpose(tf.gather_nd(sc_df,J_1_mat_z_ind),[2,1,0])
        R_2_mat = tf.transpose(tf.gather_nd(sc_df,J_2_mat_x_ind),[2,1,0])
        R_3_mat = tf.transpose(tf.gather_nd(sc_df,J_3_mat_x_ind),[2,1,0])
        
        R_1_mat_x = tf.reshape(R_1_mat_x,[-1,1,4,4]) #N 1 4 4
        R_1_mat_y = tf.reshape(R_1_mat_y,[-1,1,4,4])
        R_1_mat_z = tf.reshape(R_1_mat_z,[-1,1,4,4])
        R_2_mat = tf.reshape(R_2_mat,[-1,1,4,4])
        R_3_mat = tf.reshape(R_3_mat,[-1,1,4,4])
        
        R_1_mat = tf.matmul(R_1_mat_z,tf.matmul(R_1_mat_y,R_1_mat_x))
        
        #mask = tf.constant([1,-1,1,1,  -1,1,-1,1,  1,-1,1,1,   1,1,1,1],shape=[1,1,4,4],dtype=tf.float32)
        #mask = tf.tile(mask, [df.shape[0],1,1,1]) #N 1 4 4
        
        #transpose
        #R_1_mat = tf.multiply(R_1_mat,mask) # N 1 4 4
        #R_2_mat = tf.multiply(R_2_mat,mask)
        #R_3_mat = tf.multiply(R_3_mat,mask)
        
        # R_1_mat = tf.transpose(R_1_mat,[0,1,3,2])
        # R_2_mat = tf.transpose(R_2_mat,[0,1,3,2])
        # R_3_mat = tf.transpose(R_3_mat,[0,1,3,2])
        
        J_0_mat_ind = tf.constant([[1],[0],[0],[2+i*3],  [0],[1],[0],[2+i*3+1],   [0],[0],[1],[2+i*3+2],  [0],[0],[0],[1]],dtype=tf.int32)
        J_1_mat_ind = tf.constant([[1],[0],[0],[0],  [0],[1],[0],[2+i*3+15],   [0],[0],[1],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_2_mat_ind = tf.constant([[1],[0],[0],[0],  [0],[1],[0],[2+i*3+16],   [0],[0],[1],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        J_3_mat_ind = tf.constant([[1],[0],[0],[0],  [0],[1],[0],[2+i*3+17],   [0],[0],[1],[0],  [0],[0],[0],[1]],dtype=tf.int32)
        
        T_0_mat = tf.transpose(tf.gather_nd(sc_t,J_0_mat_ind),[2,1,0]) # N 1 16
        T_1_mat = tf.transpose(tf.gather_nd(sc_t,J_1_mat_ind),[2,1,0])
        T_2_mat = tf.transpose(tf.gather_nd(sc_t,J_2_mat_ind),[2,1,0])
        T_3_mat = tf.transpose(tf.gather_nd(sc_t,J_3_mat_ind),[2,1,0])
        
        T_0_mat = tf.reshape(T_0_mat,[-1,1,4,4])
        T_1_mat = tf.reshape(T_1_mat,[-1,1,4,4])
        T_2_mat = tf.reshape(T_2_mat,[-1,1,4,4])
        T_3_mat = tf.reshape(T_3_mat,[-1,1,4,4])
        
        #RT_all = tf.concat([RT_all,T_0_mat],1) #push back the first joint of finger
        
        RT_1 = tf.matmul(T_0_mat,R_1_mat)
        RT_all = tf.concat([RT_all,RT_1],1)
        
        RT_2 = tf.matmul(T_0_mat,tf.matmul(R_1_mat,tf.matmul(T_1_mat,R_2_mat)))
        RT_all = tf.concat([RT_all,RT_2],1)
        
        RT_3 = tf.matmul(T_0_mat,tf.matmul(R_1_mat,tf.matmul(T_1_mat,tf.matmul(R_2_mat,tf.matmul(T_2_mat,R_3_mat)))))
        RT_all = tf.concat([RT_all,RT_3],1)
    
    #transfer to blender format (x,-z,y)
    # mask_blender = tf.constant([1,0,0,0,   0,0,-1,0,   0,1,0,0,   0,0,0,1],shape=[1,1,4,4],dtype=tf.float32)
    # mask_blender = tf.tile(mask_blender,[df.shape[0],16,1,1])
    
    #RT_all = tf.matmul(mask_blender,RT_all)
  
    return RT_all


def load_weight():
    weights_dict = json.load(open("skin_weight.json"))
    weights =np.zeros((4406,16))
    for vId in weights_dict.keys():
        for infID in weights_dict[vId].keys():
            i=int(vId)
            j=int(infID)
            weights[i][j]=weights_dict[vId][infID]
    return weights

if __name__ == '__main__':
    
   W_input = tf.placeholder(tf.float32,shape=(1,4406,16))
   restpose_input = tf.placeholder(tf.float32,shape=(1,4406,4))
   dof_input = tf.placeholder(tf.float32,shape=(1,1,25))
   R_input = tf.placeholder(tf.float32,shape=(1,3,4))

   #
   init_dof = tf.constant([-1.688597, 2.153835, -0.753169, 0.252418, 0.000000,
                               0.061404, -0.000000, 0.164297, 0.000000, 0.000000,
                               0.039628, 0.000000, 0.014808, 0.000000, 0.000000,
                               0.060280, 0.000000, -0.194745, 0.000000, 0.000000,
                               0.080853, -0.000000, -0.522267, 0.000000, 0.000000],shape=[1,1,25])

   f_length = tf.constant([-2.707528, -2.002970, -2.583603,
                           -2.784939, -2.037804, -2.252724,
                           -3.580668, -2.254245, -2.568150,
                           -3.141463, -2.228459, -2.254165,
                           -2.698370, -1.875399, -1.893391],shape=[1,1,15])
#
   c_joint = tf.constant([2.415248, -0.771598, -0.232259,
                          2.848633, -5.351968, -0.052151,
                          0.844809, -5.531802, 0.253453,
                          -1.070637, -5.270669, 0.270341,
                          -2.574195, -4.721943, 0.158358],shape=[1,1,15])

   K = tf.constant([280.000000, 0.000000, 128.000000,
                    0.000000, 280.000000, 128.000000,
                    0.000000, 0.000000, 1.000000 ],shape=[1,3,3])

   T_restpose = Bone_Transformation_Matrix(init_dof,f_length,c_joint)
   T_ = Bone_Transformation_Matrix(dof_input,f_length,c_joint)
   T = tf.matmul(T_,tf.matrix_inverse(T_restpose)) # N 16 4 4
   #print(T)


   W = tf.reshape(W_input,(W_input.shape[0],W_input.shape[1],W_input.shape[2],1,1))
   W = tf.tile(W,(1,1,1,4,4)) # N 4406 16 4 4
   T_all = tf.tile(tf.expand_dims(T,axis=1),(1,4406,1,1,1)) # N 4406 16 4 4

   weighted_T = tf.reduce_sum(tf.multiply(W,T_all),axis=2) # N 4406 4 4
   restpose = tf.expand_dims(restpose_input,axis=-1) # N 4406 4 1
   curpose = tf.matmul(weighted_T,restpose) # N 4406 4 1
   #mask_blender = tf.constant([1,0,0,0,   0,0,-1,0,   0,1,0,0,   0,0,0,1],shape=[1,1,4,4],dtype=tf.float32)
   #curpose = tf.matmul(tf.tile(mask_blender,(1,4406,1,1)),curpose)

   #curpose = tf.matmul(tf.tile(tf.expand_dims(R_input,axis=1),(1,4406,1,1)),curpose) # N 4406 3 1
   proj_img = curpose
   #proj_img = tf.matmul(tf.tile(tf.expand_dims(K,axis=1),(1,4406,1,1)),curpose) # N 4406 3 1  uv


   pcl = []
   with open('PCL.txt','r') as f:
       lines = f.readlines()
       for line in lines:
           pcl.append(np.asarray([line.split(" ")[1],line.split(" ")[2],line.split(" ")[3],1.0],dtype=np.float32))
   pcl = np.asarray(pcl) # 4406 4

   mesh_data = {}

   vertices = []
   normals = []
   faces = []
   with open('000091.txt','r') as f:
       lines = f.readlines()
       for line in lines:
           content = line.split(" ")
           if content[0] == 'v':
               vertices.append(np.asarray([content[1],-1*float(content[3]),content[2]],dtype=np.float32))
           elif content[0] == 'vn':
               normals.append(np.asarray([content[1],-1*float(content[3]),content[2]],dtype=np.float32))
           elif content[0] =='f':
               faces.append(np.asarray([content[1].split("/")[0],content[2].split("/")[0],content[3].split("/")[0],
                                        content[1].split("/")[2],content[2].split("/")[2],content[3].split("/")[2]],dtype=np.int))
   vertices = np.asarray(vertices)
   normals = np.asarray(normals)
   faces = np.asarray(faces)
   print(vertices.shape)
   faces -= 1
   triangles = faces[:,:3]

   vertex_faces_norm = {}
   for idx in range(triangles.shape[0]):
       v_key = str(triangles[idx][0])
       if vertex_faces_norm.has_key(v_key):
           prev_ = vertex_faces_norm[v_key]
           prev_.append(faces[idx][3])
           vertex_faces_norm[v_key] = prev_
       else:
           vertex_faces_norm[v_key] = [faces[idx][3]]

       v_key = str(triangles[idx][1])
       if vertex_faces_norm.has_key(v_key):
           prev_ = vertex_faces_norm[v_key]
           prev_.append(faces[idx][4])
           vertex_faces_norm[v_key] = prev_
       else:
           vertex_faces_norm[v_key] =[faces[idx][4]]

       v_key = str(triangles[idx][2])
       if vertex_faces_norm.has_key(v_key):
           prev_ = vertex_faces_norm[v_key]
           prev_.append(faces[idx][5])
           vertex_faces_norm[v_key] = prev_
       else:
           vertex_faces_norm[v_key] =[faces[idx][5]]





   vertex_norms = np.zeros_like(vertices)
   for idx in range(vertex_norms.shape[0]):
       vfns = vertex_faces_norm[str(idx)]
       norm_sum=np.array([0.,0.,0.])
       for vfn in vfns:
           norm_sum +=normals[vfn]
       vertex_norms[idx] = norm_sum/len(vfns)

   color_ =np.asarray([[0.,0.,0.],[0.15,0.15,0.15],[0.15,0.15,0.15],[0.15,0.15,0.15],[0.3,0.3,0.3],[0.3,0.3,0.3],
                       [0.3,0.3,0.3],[0.45,0.45,0.45],[0.45,0.45,0.45],[0.45,0.45,0.45],[0.6,0.6,0.6],
                       [0.6,0.6,0.6],[0.6,0.6,0.6],[0.75,0.75,0.75],[0.75,0.75,0.75],[0.75,0.75,0.75]])
   weights_dict = json.load(open("skin_weight.json"))
   weights =np.zeros((4406,16),dtype=np.float32)
   for vId in weights_dict.keys():
       for infID in weights_dict[vId].keys():
           i=int(vId)
           j=int(infID)
           weights[i][j]=weights_dict[vId][infID]
   weights_max = np.argmax(weights,axis=1)
   vertex_diffuse_colors = np.zeros((4406,3))
   for idx in range(weights_max.shape[0]):
       vertex_diffuse_colors[idx]=color_[weights_max[idx]]

   mesh_data = {}
   mesh_data['vertices'] = vertices
   mesh_data['normals'] = vertex_norms
   mesh_data['triangles'] = triangles
   mesh_data['diffuse_clors'] = vertex_diffuse_colors

   np.save("hand_mesh/vertices_90.npy",vertices)
   np.save("hand_mesh/normals_90.npy",vertex_norms)
   np.save("hand_mesh/triangles_90.npy",triangles)
   np.save("hand_mesh/diffuse_clors_90.npy",vertex_diffuse_colors)
   np.save("hand_mesh/weights_90.npy",weights)



   mesh_data = np.asarray(mesh_data)
   np.save("restpose.npy",mesh_data)


   weights_dict = json.load(open("skin_weight.json"))
   weights =np.zeros((4406,16))
   for vId in weights_dict.keys():
       for infID in weights_dict[vId].keys():
           i=int(vId)
           j=int(infID)
           weights[i][j]=weights_dict[vId][infID]

   np.savetxt("weight.txt",weights, delimiter=" ", fmt="%.9f")
   pcl = np.expand_dims(pcl,axis=0)
   weights = np.expand_dims(weights,axis=0)



   cur_dof = np.array([1.680591, 1.327397, 2.464877, 0.549834, 0.000000,
            1.353887, -1.021418, 0.520186, 0.708544, 0.000000,
            0.772618, 0.000000, 0.014808, 0.720134, 0.000000,
            1.118698, 0.000000, -0.194745, 0.794283, 0.409671,
            0.299145, -0.000000, -0.522267, 1.579156, 0.658056 ])
   cur_dof = cur_dof.reshape((1,1,25))

   R = np.array([0.420242, 0.871705, -0.252046, -2.955272,
                 -0.434390, 0.437127, 0.787544, 1.915788,
                 0.796682, -0.221473, 0.562359, 19.687462])

   R = R.reshape((1,3,4))
   with tf.Session() as sess:
       pred_pcl = sess.run([proj_img],feed_dict={W_input:weights,restpose_input:pcl,dof_input:cur_dof,R_input:R})


   pred_pcl = np.asarray(pred_pcl)
   pred_pcl = pred_pcl.reshape((-1,3))

   ax = plt.gca(projection='3d')
   for point in pred_pcl:
       ax.scatter(point[0],point[1],point[2],color='green')



   flat_obj = open("flat.obj","r")
   lines = flat_obj.readlines()  # v:4---4406


   f = open('tmppp.obj','w')
   for i in range(len(lines)):
       if i<=2 or i>=2+4407:
           f.write(lines[i])
       else:
           f.write('v %.6f %.6f %.6f\n'%(pred_pcl[i-3][0], pred_pcl[i-3][1], pred_pcl[i-3][2]))

#   for point in pred_pcl:
#       f.write('v %.6f %.6f %.6f\n'%(point[0], point[1], point[2]))

   f.close()



   #plt.show()
   #plt.savefig('df1.png')




