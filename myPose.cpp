#include "myPose.h"
#include "myObj.h"


lbsPose::lbsPose()
{


	this->dof_max_limits = std::vector<float>({ 100.0 / 180 , 90.0 / 180 ,	120.0/ 180  ,    100/180.0, 90.0/180.0,//thumb don't change
								                100.0 / 180 ,	0.0 / 180 * 0,	50.0 / 180 ,	130.0 / 180 , 90.0 / 180  ,
							                    100.0 / 180 ,    0.0 / 180 * 0,	30.0 / 180 ,	130.0 / 180 , 90.0 / 180  ,
								                100.0 / 180 ,	0.0 / 180 * 0,	30.0 / 180 ,	130.0 / 180 , 90.0 / 180 ,
								                100.0 / 180 ,	0.0 / 180 * 0,	30.0 / 180 ,	130.0 / 180 , 90.0 / 180  });
	this->dof_min_limits = std::vector<float>({ 0 / 180 ,    -60 / 180 ,    -0.0 / 180 ,    -10.0 / 180, -10.0 / 180,				//thumb don't change
									            0 / 180 * 0,	0 / 180 * 0,	-20.0 / 180,	-10.0 / 180, -10.0 / 180,
									            0 / 180 * 0,    0 / 180 * 0,	-30.0 / 180,	-10.0 / 180, -10.0 / 180,
									            0 / 180 * 0,	0 / 180 * 0,	-30.0 / 180,	-10.0 / 180, -10.0 / 180,
									            0 / 180 * 0,	0 / 180 * 0,	-30.0 / 180,	-10.0 / 180, -10.0 / 180 });

	for (int i = 0; i < dof_max_limits.size(); i++)
	{
		dof_max_limits[i] = dof_max_limits[i]*PI;
		dof_min_limits[i] = dof_min_limits[i] * PI;
	}

	this->bone_length = { -2.707528, -2.002970, -2.583603,
		-2.784939, -2.037804, -2.252724,
		-3.580668, -2.254245, -2.568150,
		-3.141463, -2.228459, -2.254165,
		-2.698370, -1.875399, -1.893391 };

	this->initial_pose("PCL.txt");
	this->dof = std::vector<float>(20, 0);

	this->dis_LR = 12.0;
	std::vector<float> init_palm_pos = { 2.415248, -0.771598, -0.232259,
						  2.848633, -5.351968, -0.052151,
						  0.844809, -5.531802, 0.253453,
						  -1.070637, -5.270669, 0.270341,
						  -2.574195, -4.721943, 0.158358 };

	for (int i = 0; i < 5; i++)	{
		this->init_palm_pos.push_back({ init_palm_pos[i * 3 + 0], init_palm_pos[i * 3 + 1], init_palm_pos[i * 3 + 2],1.0 });
	}


	this->init_dofs = { -1.688597, 2.153835, -0.753169, 0.252418, 0.000000,
							   0.061404, -0.000000, 0.164297, 0.000000, 0.000000,
							   0.039628, 0.000000, 0.014808, 0.000000, 0.000000,
							   0.060280, 0.000000, -0.194745, 0.000000, 0.000000,
							   0.080853, -0.000000, -0.522267, 0.000000, 0.000000 };
	//this->cur_vertexs
	std::vector<float> init_joint_pos = { 0.0, 0.0, 0.0,
		2.415248,-0.771598,-0.232259,
		4.270355,-2.074632,-1.712593,
		5.295273,-3.404013,-2.805377,
		6.617301,-5.118763,-4.214944,

		2.848633,-5.351968,-0.052151,
		3.303276,-8.094225,-0.223051,
		3.635949,-10.100798,-0.348102,
		4.003707,-12.318998,-0.486342,

		0.844809,-5.531802,0.253453,
		0.897788,-9.109267,0.111595,
		0.931142,-11.361495,0.022287,
		0.969140,-13.927347,-0.079457,

		-1.070637,-5.270669,0.270341,
		-1.677459,-8.347152,0.081089,
		-2.107920,-10.529515,-0.053160,
		-2.543346,-12.737053,-0.188958,

		-2.574195,-4.721943,0.158358,
		-3.915869,-7.052955,-0.059574,
		-4.848347,-8.673037,-0.211040,
		-5.789772,-10.308660,-0.363959};

	for (int i = 0; i < init_joint_pos.size() / 3; i++)
	{
		//glm::vec3 t(joint_pose[i * 3 + 1], joint_pose[i * 3 + 0], -joint_pose[i * 3 + 2]);

		this->init_joint_pose.push_back({ init_joint_pos[i * 3 + 0], init_joint_pos[i * 3 + 1], init_joint_pos[i * 3 + 2],1.0 });

	}
	this->cur_joint_pose_R = this->init_joint_pose;
	this->cur_joint_pose_L = this->init_joint_pose;

	//std::vector <glm::mat4> RT0 = this->getRT_from_DF(this->init_dofs);
	this->RAND_NOT =false;
	this->RT0 = std::vector<glm::mat4>(16, glm::mat4(0.0));

	srand((unsigned)time(NULL));
}


lbsPose::~lbsPose()
{
	this->bone_length.clear();

	this->weight_matrix.clear();


}

std::vector<float> lbsPose::rand_dof()
{
	std::vector<float> dof;
	//srand((unsigned)time(NULL));
	for(int i=0; i<NUMDOF;i++)
		dof.push_back(((rand() % RANDOMMAX) *1.0 / RANDOMMAX * (this->dof_max_limits[i] - this->dof_min_limits[i]) + this->dof_min_limits[i]));
		
	return dof;
}

void lbsPose::random_pose_R(std::vector<float> dof, std::vector<glm::vec3>& m_vertices)
{
	//std::vector<float> dof = this->rand_dof();

	//dof = { 1.680591, 1.327397, 2.464877, 0.549834, 0.000000,
	//	    1.353887, -1.021418, 0.520186, 0.708544, 0.000000,
 // 		    0.772618, 0.000000, 0.014808, 0.720134, 0.000000,
	//		1.118698, 0.000000, -0.194745, 0.794283, 0.409671,
	//	    0.299145, -0.000000, -0.522267, 1.579156, 0.658056 };

	//dof = dof * 0;// 180.0 / PI;
	//for (int i = 0; i < dof.size(); i++)
	//{
	//	dof[i] = 0;
	//}


//#ifdef DEBUG_DF
//
//	for (int i = 0; i < dof.size(); i++)
//	{
//		std::cout << "dof[" << i << "]: " << dof[i] << std::endl;
//		if((i+1)%5==0)
//			std::cout << std::endl;
//	}
//#endif
	if (!this->RAND_NOT)
	{
		this->RT0 = this->getRT_from_DF(this->init_dofs);
		this->RAND_NOT = true;
	}
	
#ifdef  MYDEBUG

	for (int i = 0; i < RT0.size(); i++)
	{
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl << std::endl;

	}
#endif //  MYDEBUG

	std::vector <glm::mat4> RT1 = this->getRT_from_DF(dof);
#ifdef  DEBUG_WEIGHT
	for (int i = 0; i < RT1.size(); i++)
	{
		std::cout << "RT1[" << i << "]:" << glm::to_string(RT1[i]) << std::endl << std::endl;

	}
#endif //  DEBUG_WEIGHT




	std::vector<glm::mat4> RT2;//n 16 4 4


	for (int i = 0; i < RT0.size(); i++)
	{




		glm::mat4 RT0i_V = glm::inverse(RT0[i]);
		glm::mat4 t =  RT0i_V* RT1[i];
		RT2.push_back(t);
#ifdef MYDEBUG
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl<<std::endl;
		std::cout << "RT0_v[" << i << "]:" << glm::to_string(RT0i_V) << std::endl << std::endl;
		std::cout << "RT1[" << i << "]:" << glm::to_string(RT1[i]) << std::endl<<std::endl;
		std::cout << "RT2[" << i << "]:" << glm::to_string(t) << std::endl<<std::endl;
		std::cout << std::endl << std::endl;
#endif
	}

#ifdef DEBUG_WEIGHT
	for (int i = 0; i < RT0.size(); i++)
	{
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl << std::endl;
	}
	for (int i = 0; i < RT2.size(); i++)
	{
		std::cout << "RT2[" << i << "]:" << glm::to_string(RT2[i]) << std::endl << std::endl;
	}

#endif // DEBUG_WEIGHT




	//this->weight_matrix;//4406  4 4
	std::vector<glm::mat4> weight_mt;// 4406 16 

	for (int i = 0; i < this->weight_matrix.size(); i++)
	{
		//glm::mat4 wt_mat4 = weight_matrix[i];
		glm::mat4 rs = glm::mat4(0.0);

		for (int j = 0; j < 16; j++)
		{

					//std::cout << i << " " << j << " " << k << " " << l << std::endl;
			rs += this->weight_matrix[i][j]* RT2[j];

		}
		//weight_matrix[i] = rs;
		//std::cout << "rs[" << i << "]:" << glm::to_string(rs) << std::endl << std::endl;
		weight_mt.push_back(rs);
	}


#ifndef DEBUG_DF
	for (int i = 0; i < RT2.size(); i++)
		std::cout << "RT[" << i << "]: " << glm::to_string(RT2[i]) << std::endl;
#endif
	//std::vector<glm::vec4> curpose;
	for (int i = 0; i < weight_mt.size(); i++)
	{
		/*glm::vec4 tpos*/ 
		this->cur_vertexs[i]=  this->init_vertexs[i]* weight_mt[i];
		m_vertices[i] = this->cur_vertexs[i];
		//std::cout << "weighted_mat["<<i<<"]: " << glm::to_string(weight_mt[i]) << std::endl;
		//std::cout << "init_Vertex[" << i << "]: " << glm::to_string(this->init_vertexs[i]) << std::endl;
		//std::cout << "cur_Vertex[" << i << "]: " << glm::to_string(this->cur_vertexs[i]) << std::endl;

		//std::cout << std::endl << std::endl;
		//curpose.push_back(tpos);
	}

	for (int i = 0; i < 5; i++)
	{
		//glm::vec4 t(this->joint_pose[i*4+2].x, this->joint_pose[i * 4 + 2].y, this->joint_pose[i * 4 + 2].z, 1.0);
		this->cur_joint_pose_R[i * 4 + 2] = init_joint_pose[i * 4 + 2] * RT2[i*3+1];

		//t.x = this->joint_pose[i * 4 + 3].x, t.y=this->joint_pose[i * 4 + 3].y, t.z=this->joint_pose[i * 4 +3].z;
		this->cur_joint_pose_R[i * 4 + 3] = init_joint_pose[i * 4 + 3] * RT2[i * 3 + 2];
		
		//t.x=this->joint_pose[i * 4 + 4].x, t.y=this->joint_pose[i * 4 +4].y, t.z=this->joint_pose[i * 4 +4].z;
		this->cur_joint_pose_R[i * 4 + 4] = init_joint_pose[i * 4 + 4] * RT2[i * 3 + 3];

	}


	


	//glm::mat4 global_R = eulerAngles2matrix({dof[0], dof[1], dof[2]});
	//for (int i = 0; i < m_vertices.size()/2; i++)
	//	m_vertices[i] =global_R*this->cur_vertexs[i];

	//for (int i = 0; i < this->cur_joint_pose_R.size(); i++)
	//	this->cur_joint_pose_R[i] =  this->cur_joint_pose_R[i]*global_R;
	//for (int i = 0; i < this->cur_joint_pose_R.size(); i++)
	//	this->cur_joint_pose_R[i].x = this->cur_joint_pose_R[i].x + 10.0;


	this->cur_joint_pose_R[0] = this->init_joint_pose[0];
	this->cur_joint_pose_R[1] = this->init_joint_pose[1];
	this->cur_joint_pose_R[5] = this->init_joint_pose[5];
	this->cur_joint_pose_R[9] = this->init_joint_pose[9];
	this->cur_joint_pose_R[13] = this->init_joint_pose[13];
	this->cur_joint_pose_R[17] = this->init_joint_pose[17];

	





}
void lbsPose::random_pose_L(std::vector<float> dof, std::vector<glm::vec3>& m_vertices)
{
	//std::vector<float> dof = this->rand_dof();

	//dof = { 1.680591, 1.327397, 2.464877, 0.549834, 0.000000,
	//	    1.353887, -1.021418, 0.520186, 0.708544, 0.000000,
 // 		    0.772618, 0.000000, 0.014808, 0.720134, 0.000000,
	//		1.118698, 0.000000, -0.194745, 0.794283, 0.409671,
	//	    0.299145, -0.000000, -0.522267, 1.579156, 0.658056 };

	//dof = dof * 0;// 180.0 / PI;
	//for (int i = 0; i < dof.size(); i++)
	//{
	//	dof[i] = 0;
	//}


//#ifdef DEBUG_DF
//
//	for (int i = 0; i < dof.size(); i++)
//	{
//		std::cout << "dof[" << i << "]: " << dof[i] << std::endl;
//		if((i+1)%5==0)
//			std::cout << std::endl;
//	}
//#endif
	if (!this->RAND_NOT)
	{
		this->RT0 = this->getRT_from_DF(this->init_dofs);
		this->RAND_NOT = true;
	}

#ifdef  MYDEBUG

	for (int i = 0; i < RT0.size(); i++)
	{
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl << std::endl;

	}
#endif //  MYDEBUG

	std::vector <glm::mat4> RT1 = this->getRT_from_DF(dof);
#ifdef  DEBUG_WEIGHT
	for (int i = 0; i < RT1.size(); i++)
	{
		std::cout << "RT1[" << i << "]:" << glm::to_string(RT1[i]) << std::endl << std::endl;

	}
#endif //  DEBUG_WEIGHT




	std::vector<glm::mat4> RT2;//n 16 4 4


	for (int i = 0; i < RT0.size(); i++)
	{




		glm::mat4 RT0i_V = glm::inverse(RT0[i]);
		glm::mat4 t = RT0i_V * RT1[i];
		RT2.push_back(t);
#ifdef MYDEBUG
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl << std::endl;
		std::cout << "RT0_v[" << i << "]:" << glm::to_string(RT0i_V) << std::endl << std::endl;
		std::cout << "RT1[" << i << "]:" << glm::to_string(RT1[i]) << std::endl << std::endl;
		std::cout << "RT2[" << i << "]:" << glm::to_string(t) << std::endl << std::endl;
		std::cout << std::endl << std::endl;
#endif
	}

#ifdef DEBUG_WEIGHT
	for (int i = 0; i < RT0.size(); i++)
	{
		std::cout << "RT0[" << i << "]:" << glm::to_string(RT0[i]) << std::endl << std::endl;
	}
	for (int i = 0; i < RT2.size(); i++)
	{
		std::cout << "RT2[" << i << "]:" << glm::to_string(RT2[i]) << std::endl << std::endl;
	}

#endif // DEBUG_WEIGHT




	//this->weight_matrix;//4406  4 4
	std::vector<glm::mat4> weight_mt;// 4406 16 

	for (int i = 0; i < this->weight_matrix.size(); i++)
	{
		//glm::mat4 wt_mat4 = weight_matrix[i];
		glm::mat4 rs = glm::mat4(0.0);

		for (int j = 0; j < 16; j++)
		{

			//std::cout << i << " " << j << " " << k << " " << l << std::endl;
			rs += this->weight_matrix[i][j] * RT2[j];

		}
		//weight_matrix[i] = rs;
		//std::cout << "rs[" << i << "]:" << glm::to_string(rs) << std::endl << std::endl;
		weight_mt.push_back(rs);
	}


#ifndef DEBUG_DF
	for (int i = 0; i < RT2.size(); i++)
		std::cout << "RT[" << i << "]: " << glm::to_string(RT2[i]) << std::endl;
#endif
	//std::vector<glm::vec4> curpose;
	for (int i = 0; i < weight_mt.size(); i++)
	{
		glm::vec4 tpos  = this->init_vertexs[i] * weight_mt[i];

		m_vertices[i + weight_mt.size()] = glm::vec3({ tpos.x, tpos.y, -dis_LR-tpos.z });
		//this->cur_vertexs[i] = this->init_vertexs[i] * weight_mt[i];
		//m_vertices[i] = this->cur_vertexs[i];
		//std::cout << "weighted_mat["<<i<<"]: " << glm::to_string(weight_mt[i]) << std::endl;
		//std::cout << "init_Vertex[" << i << "]: " << glm::to_string(this->init_vertexs[i]) << std::endl;
		//std::cout << "cur_Vertex[" << i << "]: " << glm::to_string(this->cur_vertexs[i]) << std::endl;

		//std::cout << std::endl << std::endl;
		//curpose.push_back(tpos);
	}

	for (int i = 0; i < 5; i++)
	{
		//glm::vec4 t(this->joint_pose[i*4+2].x, this->joint_pose[i * 4 + 2].y, this->joint_pose[i * 4 + 2].z, 1.0);
		this->cur_joint_pose_L[i * 4 + 2] = init_joint_pose[i * 4 + 2] * RT2[i * 3 + 1];
		this->cur_joint_pose_L[i * 4 + 2].z = -this->cur_joint_pose_L[i * 4 + 2].z - dis_LR;

		//t.x = this->joint_pose[i * 4 + 3].x, t.y=this->joint_pose[i * 4 + 3].y, t.z=this->joint_pose[i * 4 +3].z;
		this->cur_joint_pose_L[i * 4 + 3] = init_joint_pose[i * 4 + 3] * RT2[i * 3 + 2];
		this->cur_joint_pose_L[i * 4 + 3].z = -this->cur_joint_pose_L[i * 4 + 3].z - dis_LR;

		//t.x=this->joint_pose[i * 4 + 4].x, t.y=this->joint_pose[i * 4 +4].y, t.z=this->joint_pose[i * 4 +4].z;
		this->cur_joint_pose_L[i * 4 + 4] = init_joint_pose[i * 4 + 4] * RT2[i * 3 + 3];
		this->cur_joint_pose_L[i * 4 + 4].z = -this->cur_joint_pose_L[i * 4 + 4].z - dis_LR;

	}

	//for(int i=0; i<this->cur_joint_pose_L.size(); i++)
	//	this->cur_joint_pose_L[i].z = -this->cur_joint_pose_L[i].z - dis_LR;

	this->cur_joint_pose_L[0].z = -this->init_joint_pose[0].z - dis_LR;
	this->cur_joint_pose_L[1].z = -this->init_joint_pose[1].z - dis_LR;
	this->cur_joint_pose_L[5].z = -this->init_joint_pose[5].z - dis_LR;
	this->cur_joint_pose_L[9].z = -this->init_joint_pose[9].z - dis_LR;
	this->cur_joint_pose_L[13].z = -this->init_joint_pose[13].z - dis_LR;
	this->cur_joint_pose_L[17].z = -this->init_joint_pose[17].z - dis_LR;
	
	


	//glm::mat4 global_L = eulerAngles2matrix({ dof[0], dof[1], dof[2] });
	//for (int i = 0;  i < weight_mt.size(); i++)
	//	m_vertices[i+ weight_mt.size()] = glm::vec4({ m_vertices[i + weight_mt.size()].x, m_vertices[i + weight_mt.size()].y, m_vertices[i + weight_mt.size()].z,1 })* global_L;

	//for (int i = 0; i < this->cur_joint_pose_L.size(); i++)
	//	this->cur_joint_pose_L[i] = this->cur_joint_pose_L[i]* global_L;
	//for (int i = 0; i < this->cur_joint_pose_L.size(); i++)
	//	this->cur_joint_pose_L[i].x += 10.0;
	
}
bool lbsPose::initial_pose(const char * filepath)
{
	std::ifstream initial_pose_file;
	initial_pose_file.open(filepath, std::ios::binary);
	if (initial_pose_file.fail())
	{
		std::cout << "initial pose error!" << std::endl;
		return  false;
	}

	std::string line;
	while (getline(initial_pose_file, line))
	{
		if (line.substr(0, 1) == "v")
		{
			glm::vec4 vertex;
			sscanf(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1;
			this->init_vertexs.push_back(vertex);
			this->cur_vertexs.push_back(vertex);
		}
	}

	return true;

}



bool lbsPose::load_weight_matrix_from_txt(const char * filepath)
{
	std::ifstream load_wm_file;
	load_wm_file.open(filepath, std::ios::binary);
	if (load_wm_file.fail())
	{
		std::cout << "weight file load error! " << std::endl;
		return false;
	}

	std::string line;

	while (getline(load_wm_file, line))
	{
		std::vector<std::string> tsv;
		std::vector<float> tv(16,0);
		split(line, tsv, ' ');
		for (int i =0; i<16; i++)
			tv[i] = std::stof(tsv[i]);

		this->weight_matrix.push_back(tv);
	}



}

glm::mat4 lbsPose::vec2transmatrix(const std::vector<float>& trans)
{

	return glm::mat4{1.0,0,0,trans[0],
	                 0,1.0,0,trans[1],
	                 0,0,1.0,trans[2],
	                 0,0,0,1.0};
	
}

glm::mat4 lbsPose::eulerAngles2matrix(const std::vector<float>& angles)
{
	float angle;

	float		sr, sp, sy, cr, cp, cy;

	angle = angles[2];
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1];
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0];
	sr = sin(angle);
	cr = cos(angle);

	std::vector<std::vector<float>> matrix;
	for (int i = 0; i < 4; i++)
	{
		std::vector<float> t(4,0);
		matrix.push_back(t);
	}
	

	// matrix = (Z * Y) * X
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;
	matrix[3][0] = 0.0;

	matrix[0][1] = sr * sp*cy - cr * sy;
	matrix[1][1] = sr * sp*sy + cr * cy;
	matrix[2][1] = sr * cp;
	matrix[3][1] = 0.0;

	matrix[0][2] = (cr*sp*cy + sr * sy);
	matrix[1][2] = (cr*sp*sy -sr * cy);
	matrix[2][2] = cr * cp;
	matrix[3][2] = 0.0;

	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	matrix[3][3] = 1.0;
	

	return glm::mat4{ matrix[0][0],matrix[0][1], matrix[0][2], matrix[0][3],
					  matrix[1][0],matrix[1][1], matrix[1][2], matrix[1][3], 
                      matrix[2][0],matrix[2][1], matrix[2][2], matrix[2][3], 
	                  matrix[3][0],matrix[3][1], matrix[3][2], matrix[3][3] };


}
glm::mat4 matxmat(const glm::mat4& m1, const glm::mat4& m2)
{
	return m1 * m2;
}



std::vector <glm::mat4> lbsPose::getRT_from_DF(std::vector<float> dof)
{
	std::vector<glm::mat4>  RT_all;
	RT_all.push_back(glm::mat4(1.0));



	for (int i = 0; i < 5; i++)
	{
		//Rotation
	    glm::mat4 R1 = eulerAngles2matrix({ dof[i * 5 + 0], dof[i * 5 + 1], dof[i * 5 + 2] });
		
		glm::mat4 R2 = eulerAngles2matrix({ dof[i * 5 + 3], 0,  0 });

		glm::mat4 R3 = eulerAngles2matrix({ dof[i * 5 + 4], 0,  0 });
#ifdef  MYDEBUG

		//std::cout << "R1_X: " << glm::to_string(eulerAngles2matrix({ dof[i * 5 + 0],0,0 }))<<std::endl << std::endl;

		//std::cout << "R1_Y: " << glm::to_string(eulerAngles2matrix({ 0,dof[i * 5 + 1],0 })) << std::endl << std::endl;

		//std::cout << "R1_Z: " << glm::to_string(eulerAngles2matrix({ 0,0,dof[i * 5 +2] })) << std::endl << std::endl;


		std::cout << "R1--[" << i << "]:" << glm::to_string(R1) << std::endl << std::endl;

		std::cout << "R2--[" << i << "]:" << glm::to_string(R2) << std::endl << std::endl;

		std::cout << "R3--[" << i << "]:" << glm::to_string(R3) << std::endl << std::endl;

#endif //  MYDEBUG


		//Translation
		glm::mat4 T1 = vec2transmatrix({ this->init_palm_pos[i].x, this->init_palm_pos[i].y, this->init_palm_pos[i].z });
		glm::mat4 T2 = vec2transmatrix({ 0,this->bone_length[i * 3 + 0],0 });
		glm::mat4 T3 = vec2transmatrix({ 0,this->bone_length[i * 3 + 1],0 });
		glm::mat4 T4 = vec2transmatrix({ 0,this->bone_length[i * 3 + 2],0 });

#ifdef MYDEBUG

		std::cout << "T1--[" << i << "]:" << glm::to_string(T1) << std::endl << std::endl;
		std::cout << "T2--[" << i << "]:" << glm::to_string(T2) << std::endl << std::endl;
		std::cout << "T3--[" << i << "]:" << glm::to_string(T3) << std::endl << std::endl;
		std::cout << "T4--[" << i << "]:" << glm::to_string(T4) << std::endl << std::endl;


#endif // MYDEBUG




		glm::mat4  RT1 = R1*T1;
		RT_all.push_back(RT1);

		glm::mat4 RT2 = R2*T2*R1*T1;// T1*(R1*(T2*R2));
		RT_all.push_back(RT2);

		glm::mat4 RT3 = R3*T3*R2*T2*R1*T1;// T1*(R1*(T2*(R2*(T3*R3))));
		RT_all.push_back(RT3);
#ifdef MYDEBUG

		std::cout << "RT1--[" << i << "]:" << glm::to_string(RT1) << std::endl << std::endl;
		std::cout << "RT2--[" << i << "]:" << glm::to_string(RT2) << std::endl << std::endl;
		std::cout << "RT3--[" << i << "]:" << glm::to_string(RT3) << std::endl << std::endl;
		//std::cout << "T4--[" << i << "]:" << glm::to_string(T4) << std::endl << std::endl;
		std::cout << "ver" << std::endl;

#endif // MYDEBUG

	}
	return RT_all;
}



std::vector<glm::vec3> lbsPose::param_with_df(std::vector<float> dof) // dof 25 = 5*(3.1.1)
{
	std::vector<glm::vec3> reparam_pos(21, { 0,0,0 });
	
	/*std::vector<glm::vec3> vv;
	std::vector<glm::vec3> axis;
	
	std::vector<glm::qua<float>> quas;
	std::vector<glm::mat3x4 >mats;*/


	return  reparam_pos;




}
std::vector<glm::vec3> lbsPose::parma_with_vector3d(std::vector<glm::vec3> vec3d)  // vec3d 20  = 4*5
{
	std::vector<glm::vec3> rescale_pos(21, {0,0,0});


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int index = i * 4 + j;
			glm::vec3 t = glm::normalize(vec3d[index]);
			rescale_pos[index+1] = t * this->bone_length[index] + (j==0?rescale_pos[0]:rescale_pos[index]);
		}
	}
	return rescale_pos;

}