#pragma once
//#define MYDEBUG 1
//#define DEBUG_WEIGHT 1
#define DEBUG_DF


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <math.h>
#include "mathlib.h"
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>



#define NUMDOF 25
#define PI 3.14159265358979323846 
#define	Debug_MCP 1
#define	Debug_PCP 1
#define	Debug_DIP 1
#define RANDOMMAX 888

class lbsPose
{
public:

	lbsPose();
	~lbsPose();
	std::vector<float> bone_length;// 15
	std::vector<std::vector<float>>  weight_matrix;  // 4406 16
	
	

	std::vector<glm::vec4> init_joint_pose;
	std::vector<glm::vec4> cur_joint_pose_R;
	std::vector<glm::vec4> cur_joint_pose_L;

	float dis_LR;


	std::vector<glm::vec4> init_vertexs;
	std::vector<glm::vec4> init_palm_pos;
	std::vector<glm::vec4> cur_vertexs;
	std::vector <glm::mat4> RT0;
	bool RAND_NOT;

	std::vector<float> init_dofs;


	std::vector<float> dof; // 25 = (3.1.1)*5
	std::vector<float> dof_max_limits;
	std::vector<float> dof_min_limits;

	std::vector<float> rand_dof();
	

	void random_pose_R(std::vector<float> dof, std::vector<glm::vec3>& m_vertices);
	void random_pose_L(std::vector<float> dof, std::vector<glm::vec3>& m_vertices);

	bool initial_pose(const char * filepath);
	bool load_weight_matrix_from_txt(const char * filepath);

	std::vector <glm::mat4> getRT_from_DF(std::vector<float> dof);



	std::vector<glm::vec3> param_with_df(std::vector<float> dof);
	std::vector<glm::vec3> parma_with_vector3d(std::vector<glm::vec3> vec3d);




	glm::mat4 eulerAngles2matrix(const std::vector<float>& angles);
	glm::mat4 vec2transmatrix(const std::vector<float>& trans);
	glm::mat4 matxmat(const glm::mat4 & m1, const glm::mat4 & m2);






};