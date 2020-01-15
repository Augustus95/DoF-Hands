#pragma once

#define  _CRT_SECURE_NO_WARNINGS 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#include "mathlib.h"
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
//#include <GL/glew.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
//#include <GLFW/glfw3.h>




struct Vertex
{
	glm::vec3 position;
};








void split(const std::string& s, std::vector<std::string>& sv, const char delim = ' ');

class myObjModel
{



public:
	myObjModel();
	//virtual ~myObjModel();
	

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_vert_tex;
	std::vector<glm::vec3> m_vert_norm;
	std::vector<glm::uvec3> m_faces_index_v;
	std::vector<glm::uvec3> m_faces_index_vt;
	std::vector<glm::uvec3> m_faces_index_normal;


	int m_num_faces;



	//GLuint uvmap;
	//GLuint diffuseMap;
	//GLuint specularMap;




public:
	bool Load(const char *filename);
	void Clear();
	void Mirror();
	


};