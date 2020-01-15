#include "myObj.h"





myObjModel::myObjModel()
{
	this->m_faces_index_v.clear();
	this->m_faces_index_vt.clear();
	this->m_faces_index_normal.clear();
	this->m_vertices.clear();
	this->m_vert_tex.clear();
	this->m_vert_norm.clear();
}



void myObjModel::Mirror()
{
	int size_v = m_vertices.size();
	int size_f = m_faces_index_v.size();
	//glm::vec3 t_v;
	for (int i = 0; i < size_v; i++)
	{
		m_vertices.emplace_back(glm::vec3(m_vertices[i].x+0.0, m_vertices[i].y,-18.0-m_vertices[i].z));
	}

	for (int i = 0; i < size_f; i++)
	{
		m_faces_index_v.emplace_back(glm::uvec3(m_faces_index_v[i].x+size_v, m_faces_index_v[i].y+size_v, m_faces_index_v[i].z+size_v));
		m_faces_index_vt.emplace_back(glm::uvec3(m_faces_index_vt[i].x, m_faces_index_vt[i].y, m_faces_index_vt[i].z));

	}



	
	

}
bool myObjModel::Load(const char *filename)
{
	//FILE * file = fopen(filename, "r");
	//if (file == NULL)
	//{
	//	printf("Impossible to open the file !\n");
	//	return false;
	//}





	std::ifstream load_obj_file;
	load_obj_file.open(filename, std::ios::binary);
	if (load_obj_file.fail())
	{
		std::cout << "open error!" << std::endl;
		return false;
	}
	std::string line;

	

	while (getline(load_obj_file, line))
	{


        if (line.substr(0,2)== "vt")
		{
			glm::vec2 uv;
			sscanf(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
			m_vert_tex.push_back(glm::vec2(uv.x,  1-uv.y));
		}
		else if (line.substr(0, 2)== "vn")
		{
			glm::vec3 normal;
			sscanf(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
			m_vert_norm.push_back(normal);
		}
		else if (line.substr(0, 1) == "v")
		{
			glm::vec3 vertex;
			sscanf(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			m_vertices.push_back(vertex);
		}
		else if (line.substr(0, 1)=="f")
		{
			//unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			std::vector<std::string> sv;
			split(line, sv, ' ');

			std::vector<int> index_vt(3,0);
			std::vector<int> index_v(3,0);
			std::vector<int> index_normal(3,0);
			//m_faces_index_v.push_back()

			for (int i=1; i<=3; i++)
			{
				std::vector<std::string> tsv;
				split(sv[i], tsv, '/');
				if(tsv[0]!="")
				    index_v[i-1] = std::stoi(tsv[0]);
				if(tsv[1]!="")
  				    index_vt[i-1] = std::stoi(tsv[1]);
				if(tsv[2]!="")
   				    index_normal[i - 1] = std::stoi(tsv[2]);
			}
	
			m_faces_index_v.emplace_back(glm::uvec3(index_v[0], index_v[1], index_v[2]));
			m_faces_index_vt.emplace_back(glm::uvec3(index_vt[0], index_vt[1], index_vt[2]));
			m_faces_index_normal.emplace_back(glm::uvec3(index_normal[0], index_normal[1], index_normal[2]));

			if (sv.size() >= 5)
			{

				std::vector<int> index = { 1,3,4 };
				for (int i = 1; i <= 3; i++)
				{
					std::vector<std::string> tsv;
					split(sv[index[i-1]], tsv, '/');
					if (tsv[0] != "")
						index_v[i - 1] = std::stoi(tsv[0]);
					if (tsv[1] != "")
						index_vt[i - 1] = std::stoi(tsv[1]);
					if (tsv[2] != "")
						index_normal[i - 1] = std::stoi(tsv[2]);
				}

				m_faces_index_v.emplace_back(glm::uvec3(index_v[0], index_v[1], index_v[2]));
				m_faces_index_vt.emplace_back(glm::uvec3(index_vt[0], index_vt[1], index_vt[2]));
				m_faces_index_normal.emplace_back(glm::uvec3(index_normal[0], index_normal[1], index_normal[2]));
			}

		}

	}

	
	printf("Load obj done.\n");
	return true;
}

void myObjModel::Clear()
{
	this->m_faces_index_v.clear();
	this->m_faces_index_vt.clear();
	this->m_faces_index_normal.clear();
	this->m_vertices.clear();
	this->m_vert_tex.clear();
	this->m_vert_norm.clear();
}



void split(const std::string& s, std::vector<std::string>& sv, const char delim ) {
	sv.clear();
	std::istringstream iss(s);
	std::string temp;

	while (std::getline(iss, temp, delim)) {
		sv.emplace_back(std::move(temp));
	}

	return;
}