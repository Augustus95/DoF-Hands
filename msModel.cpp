///////////////////////////////////////////////////////////////////////
// title:          MilkShape 3D Model Viewer Sample
//
// copyright:      The programs and associated files contained in this
//                 distribution were developed by Mete Ciragan.  The
//                 programs are not in the public domain, but they are
//                 freely distributable without licensing fees.  These
//                 programs are provided without guarantee or warrantee
//                 expressed or implied.  Use at your own risk!
//
// email:          mciragan@gmx.net
// web:            http://www.milkshape3d.com
///////////////////////////////////////////////////////////////////////
#include "msModel.h"
#include "mathlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

msModel::msModel()
{
	Clear();
}

msModel::~msModel()
{
	Clear();
}

bool msModel::Load(const char *filename)
{
	m_dof.resize(20);
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return false;

	Clear();

	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char id[10];
	fread(id, sizeof(char), 10, fp);
	if (strncmp(id, "MS3D000000", 10) != 0)
	{
		fclose(fp);
		// "This is not a valid MS3D file format!"
		return false;
	}

	int version;
	fread(&version, sizeof(int), 1, fp);
	if (version != 4)
	{
		fclose(fp);
		// "This is not a valid MS3D file version!"
		return false;
	}

	int i, j;

	// vertices
	unsigned short numVertices;
	fread(&numVertices, sizeof(unsigned short), 1, fp);
	m_vertices.resize(numVertices);
	for (i = 0; i < numVertices; i++)
	{
		fread(&m_vertices[i].flags, sizeof(unsigned char), 1, fp);
		fread(&m_vertices[i].vertex, sizeof(float), 3, fp);
		fread(&m_vertices[i].boneId, sizeof(char), 1, fp);
		fread(&m_vertices[i].referenceCount, sizeof(unsigned char), 1, fp);
	}
	
	// triangles
	unsigned short numTriangles;
	fread(&numTriangles, sizeof(unsigned short), 1, fp);
	m_triangles.resize(numTriangles);
	for (i = 0; i < numTriangles; i++)
	{
		fread(&m_triangles[i].flags, sizeof(unsigned short), 1, fp);
		fread(m_triangles[i].vertexIndices, sizeof(unsigned short), 3, fp);
		fread(m_triangles[i].vertexNormals, sizeof(float), 3 * 3, fp);
	    fread(m_triangles[i].s, sizeof(float), 3, fp);
	    fread(m_triangles[i].t, sizeof(float), 3, fp);
		m_triangles[i].t[0] = 1 - m_triangles[i].t[0]; m_triangles[i].t[1] = 1 - m_triangles[i].t[1]; m_triangles[i].t[2] = 1 - m_triangles[i].t[2];
		fread(&m_triangles[i].smoothingGroup, sizeof(unsigned char), 1, fp);
		fread(&m_triangles[i].groupIndex, sizeof(unsigned char), 1, fp);

		// TODO: calculate triangle normal
	}
	
	// groups
	unsigned short numGroups;
	fread(&numGroups, sizeof(unsigned short), 1, fp);
	m_groups.resize(numGroups);
	for (i = 0; i < numGroups; i++)
	{
		fread(&m_groups[i].flags, sizeof(unsigned char), 1, fp);
		fread(m_groups[i].name, sizeof(char), 32, fp);

		unsigned short numGroupTriangles;
		fread(&numGroupTriangles, sizeof(unsigned short), 1, fp);
		m_groups[i].triangleIndices.resize(numGroupTriangles);
		if (numGroupTriangles > 0)
			fread(&m_groups[i].triangleIndices[0], sizeof(unsigned short), numGroupTriangles, fp);

		fread(&m_groups[i].materialIndex, sizeof(char), 1, fp);
	}
	
	// materials
	unsigned short numMaterials;
	fread(&numMaterials, sizeof(unsigned short), 1, fp);
	m_materials.resize(numMaterials);
	for (i = 0; i < numMaterials; i++)
	{
		fread(m_materials[i].name, sizeof(char), 32, fp);
		fread(&m_materials[i].ambient, sizeof(float), 4, fp);
		fread(&m_materials[i].diffuse, sizeof(float), 4, fp);
		fread(&m_materials[i].specular, sizeof(float), 4, fp);
		fread(&m_materials[i].emissive, sizeof(float), 4, fp);
		fread(&m_materials[i].shininess, sizeof(float), 1, fp);
        fread(&m_materials[i].transparency, sizeof(float), 1, fp);
		fread(&m_materials[i].mode, sizeof(unsigned char), 1, fp);
        fread(m_materials[i].texture, sizeof(char), MAX_TEXTURE_FILENAME_SIZE, fp);
        fread(m_materials[i].alphamap, sizeof(char), MAX_TEXTURE_FILENAME_SIZE, fp);

		// set alpha
		m_materials[i].ambient[3] = m_materials[i].transparency;
		m_materials[i].diffuse[3] = m_materials[i].transparency;
		m_materials[i].specular[3] = m_materials[i].transparency;
		m_materials[i].emissive[3] = m_materials[i].transparency;
	}

	// animation
	fread(&m_animationFps, sizeof(float), 1, fp);
	if (m_animationFps < 1.0f)
		m_animationFps = 1.0f;
	fread(&m_currentTime, sizeof(float), 1, fp);
	fread(&m_totalFrames, sizeof(int), 1, fp);

	// joints
	unsigned short numJoints;
	fread(&numJoints, sizeof(unsigned short), 1, fp);
	m_joints.resize(numJoints);
	for (i = 0; i < numJoints; i++)
	{
		fread(&m_joints[i].flags, sizeof(unsigned char), 1, fp);
		fread(m_joints[i].name, sizeof(char), 32, fp);
		fread(m_joints[i].parentName, sizeof(char), 32, fp);
        fread(m_joints[i].rot, sizeof(float), 3, fp);
        fread(m_joints[i].pos, sizeof(float), 3, fp);
    
		unsigned short numKeyFramesRot;
		fread(&numKeyFramesRot, sizeof(unsigned short), 1, fp);
		m_joints[i].rotationKeys.resize(numKeyFramesRot);

		unsigned short numKeyFramesPos;
		fread(&numKeyFramesPos, sizeof(unsigned short), 1, fp);
		m_joints[i].positionKeys.resize(numKeyFramesPos);

		// the frame time is in seconds, so multiply it by the animation fps, to get the frames
		// rotation channel
		for (j = 0; j < numKeyFramesRot; j++)
		{
			fread(&m_joints[i].rotationKeys[j].time, sizeof(float), 1, fp);
			fread(&m_joints[i].rotationKeys[j].key, sizeof(float), 3, fp);
			m_joints[i].rotationKeys[j].time *= m_animationFps;
		}

		// translation channel
		for (j = 0; j < numKeyFramesPos; j++)
		{
			fread(&m_joints[i].positionKeys[j].time, sizeof(float), 1, fp);
			fread(&m_joints[i].positionKeys[j].key, sizeof(float), 3, fp);
			m_joints[i].positionKeys[j].time *= m_animationFps;
		}
	}

	// comments
	long filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
			int numComments = 0;
			size_t commentSize = 0;

			// group comments
			fread(&numComments, sizeof(int), 1, fp); 
			for (i = 0; i < numComments; i++)
			{
				int index;
				fread(&index, sizeof(int), 1, fp);
				std::vector<char> comment;
				fread(&commentSize, sizeof(size_t), 1, fp);
				comment.resize(commentSize);
				if (commentSize > 0)
					fread(&comment[0], sizeof(char), commentSize, fp);
				if (index >= 0 && index < (int) m_groups.size())
					m_groups[index].comment = comment;
			}

			// material comments
			fread(&numComments, sizeof(int), 1, fp); 
			for (i = 0; i < numComments; i++)
			{
				int index;
				fread(&index, sizeof(int), 1, fp);
				std::vector<char> comment;
				fread(&commentSize, sizeof(size_t), 1, fp);
				comment.resize(commentSize);
				if (commentSize > 0)
					fread(&comment[0], sizeof(char), commentSize, fp);
				if (index >= 0 && index < (int) m_materials.size())
					m_materials[index].comment = comment;
			}

			// joint comments
			fread(&numComments, sizeof(int), 1, fp); 
			for (i = 0; i < numComments; i++)
			{
				int index;
				fread(&index, sizeof(int), 1, fp);
				std::vector<char> comment;
				fread(&commentSize, sizeof(size_t), 1, fp);
				comment.resize(commentSize);
				if (commentSize > 0)
					fread(&comment[0], sizeof(char), commentSize, fp);
				if (index >= 0 && index < (int) m_joints.size())
					m_joints[index].comment = comment;
			}

			// model comments
			fread(&numComments, sizeof(int), 1, fp);
			if (numComments == 1)
			{
				std::vector<char> comment;
				fread(&commentSize, sizeof(size_t), 1, fp);
				comment.resize(commentSize);
				if (commentSize > 0)
					fread(&comment[0], sizeof(char), commentSize, fp);
				m_comment = comment;
			}
		}
		else
		{
			// "Unknown subversion for comments %d\n", subVersion);
		}
	}

	// vertex extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 2)
		{
			for (int i = 0; i < numVertices; i++)
			{
				fread(&m_vertices[i].boneIds[0], sizeof(char), 3, fp);
				fread(&m_vertices[i].weights[0], sizeof(unsigned char), 3, fp);
				fread(&m_vertices[i].extra, sizeof(unsigned int), 1, fp);
			}
		}
		else if (subVersion == 1)
		{
			for (int i = 0; i < numVertices; i++)
			{
				fread(&m_vertices[i].boneIds[0], sizeof(char), 3, fp);
				fread(&m_vertices[i].weights[0], sizeof(unsigned char), 3, fp);
			}
		}
		else
		{
			// "Unknown subversion for vertex extra %d\n", subVersion);
		}
	}

	// joint extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
			for (int i = 0; i < numJoints; i++)
			{
				fread(&m_joints[i].color, sizeof(float), 3, fp);
			}
		}
		else
		{
			// "Unknown subversion for joint extra %d\n", subVersion);
		}
	}

	// model extra
	filePos = ftell(fp);
	if (filePos < fileSize)
	{
		int subVersion = 0;
		fread(&subVersion, sizeof(int), 1, fp);
		if (subVersion == 1)
		{
			fread(&m_jointSize, sizeof(float), 1, fp);
			fread(&m_transparencyMode, sizeof(int), 1, fp);
			fread(&m_alphaRef, sizeof(float), 1, fp);
		}
		else
		{
			//"Unknown subversion for model extra %d\n", subVersion);
		}
	}

	fclose(fp);

	return true;
}

void msModel::Clear()
{
	m_vertices.clear();
	m_triangles.clear();
	m_groups.clear();
	m_materials.clear();
	m_animationFps = 24.0f;
	m_currentTime = 1.0f;
	m_totalFrames = 30;
	m_joints.clear();
	m_comment.clear();
	m_jointSize = 1.0f;
	m_transparencyMode = TRANSPARENCY_MODE_SIMPLE;
	m_alphaRef = 0.5f;
}

int msModel::GetNumGroups() const
{
	return (int) m_groups.size();
}

ms3d_group_t *msModel::GetGroup(int index)
{
	return &m_groups[index];
}

int msModel::GetNumTriangles() const
{
	return (int) m_triangles.size();
}

ms3d_triangle_t *msModel::GetTriangle(int index)
{
	return &m_triangles[index];
}

int msModel::GetNumVertices() const
{
	return (int) m_vertices.size();
}

ms3d_vertex_t *msModel::GetVertex(int index)
{
	return &m_vertices[index];
}

int msModel::GetNumMaterials() const
{
	return (int) m_materials.size();
}

ms3d_material_t *msModel::GetMaterial(int index)
{
	return &m_materials[index];
}

int msModel::GetNumJoints() const
{
	return (int) m_joints.size();
}

ms3d_joint_t *msModel::GetJoint(int index)
{
	return &m_joints[index];
}

float msModel::GetJointSize() const
{
	return m_jointSize;
}

int msModel::GetTransparencyMode() const
{
	return m_transparencyMode;
}

float msModel::GetAlphaRef() const
{
	return m_alphaRef;
}

int msModel::FindJointByName(const char *name)
{
	for (size_t i = 0; i < m_joints.size(); i++)
	{
		if (strcmp(m_joints[i].name, name) == 0)
			return i;
	}

	return -1;
}

void msModel::SetupJoints()
{
	for (size_t i = 0; i < m_joints.size(); i++)
	{
		ms3d_joint_t *joint = &m_joints[i];
		joint->parentIndex = FindJointByName(joint->parentName);
	}

	for (size_t i = 0; i < m_joints.size(); i++)
	{
		ms3d_joint_t *joint = &m_joints[i];
		AngleMatrix(joint->rot, joint->matLocalSkeleton);
		joint->matLocalSkeleton[0][3]= joint->pos[0];
		joint->matLocalSkeleton[1][3]= joint->pos[1];
		joint->matLocalSkeleton[2][3]= joint->pos[2];
		
		if (joint->parentIndex == -1)
		{
			memcpy(joint->matGlobalSkeleton, joint->matLocalSkeleton, sizeof(joint->matGlobalSkeleton));
		}
		else
		{
			ms3d_joint_t *parentJoint = &m_joints[joint->parentIndex];
			R_ConcatTransforms(parentJoint->matGlobalSkeleton, joint->matLocalSkeleton, joint->matGlobalSkeleton);
		}

		SetupTangents();
	}
}

void msModel::SetupTangents()
{
	for (size_t j = 0; j < m_joints.size(); j++)
	{
		ms3d_joint_t *joint = &m_joints[j];
		int numPositionKeys = (int) joint->positionKeys.size();
		joint->tangents.resize(numPositionKeys);

		// clear all tangents (zero derivatives)
		for (int k = 0; k < numPositionKeys; k++)
		{
			joint->tangents[k].tangentIn[0] = 0.0f;
			joint->tangents[k].tangentIn[1] = 0.0f;
			joint->tangents[k].tangentIn[2] = 0.0f;
			joint->tangents[k].tangentOut[0] = 0.0f;
			joint->tangents[k].tangentOut[1] = 0.0f;
			joint->tangents[k].tangentOut[2] = 0.0f;
		}

		// if there are more than 2 keys, we can calculate tangents, otherwise we use zero derivatives
		if (numPositionKeys > 2)
		{
			for (int k = 0; k < numPositionKeys; k++)
			{
				// make the curve tangents looped
				int k0 = k - 1;
				if (k0 < 0)
					k0 = numPositionKeys - 1;
				int k1 = k;
				int k2 = k + 1;
				if (k2 >= numPositionKeys)
					k2 = 0;

				// calculate the tangent, which is the vector from key[k - 1] to key[k + 1]
				float tangent[3];
				tangent[0] = (joint->positionKeys[k2].key[0] - joint->positionKeys[k0].key[0]);
				tangent[1] = (joint->positionKeys[k2].key[1] - joint->positionKeys[k0].key[1]);
				tangent[2] = (joint->positionKeys[k2].key[2] - joint->positionKeys[k0].key[2]);

				// weight the incoming and outgoing tangent by their time to avoid changes in speed, if the keys are not within the same interval
				float dt1 = joint->positionKeys[k1].time - joint->positionKeys[k0].time;
				float dt2 = joint->positionKeys[k2].time - joint->positionKeys[k1].time;
				float dt = dt1 + dt2;
				joint->tangents[k1].tangentIn[0] = tangent[0] * dt1 / dt;
				joint->tangents[k1].tangentIn[1] = tangent[1] * dt1 / dt;
				joint->tangents[k1].tangentIn[2] = tangent[2] * dt1 / dt;

				joint->tangents[k1].tangentOut[0] = tangent[0] * dt2 / dt;
				joint->tangents[k1].tangentOut[1] = tangent[1] * dt2 / dt;
				joint->tangents[k1].tangentOut[2] = tangent[2] * dt2 / dt;
			}
		}
	}
}

void msModel::SetFrame(float frame)
{
	if (frame < 0.0f)
	{
		for (size_t i = 0; i < m_joints.size(); i++)
		{
			ms3d_joint_t *joint = &m_joints[i];
			memcpy(joint->matLocal, joint->matLocalSkeleton, sizeof(joint->matLocal));
			memcpy(joint->matGlobal, joint->matGlobalSkeleton, sizeof(joint->matGlobal));
		}
	}
	else
	{
		for (size_t i = 0; i < m_joints.size(); i++)
		{
			EvaluateJoint(i, frame);
		}
	}

	m_currentTime = frame;
	for (size_t i = 0; i < m_joints.size(); i++)
	{
		ms3d_joint_t *joint = &m_joints[i];
		memcpy(joint->matInitLocal, joint->matLocal, sizeof(joint->matLocal));
		memcpy(joint->matInitGlobal, joint->matGlobal, sizeof(joint->matGlobal));
		joint->initPos.x = joint->matInitGlobal[0][3]; joint->initPos.y = joint->matInitGlobal[1][3]; joint->initPos.z = joint->matInitGlobal[2][3];
	}
}

void msModel::EvaluateJoint(int index, float frame)
{
	ms3d_joint_t *joint = &m_joints[index];

	//
	// calculate joint animation matrix, this matrix will animate matLocalSkeleton
	//
	vec3_t pos = { 0.0f, 0.0f, 0.0f };
	int numPositionKeys = (int) joint->positionKeys.size();
	if (numPositionKeys > 0)
	{
		int i1 = -1;
		int i2 = -1;

		// find the two keys, where "frame" is in between for the position channel
		for (int i = 0; i < (numPositionKeys - 1); i++)
		{
			if (frame >= joint->positionKeys[i].time && frame < joint->positionKeys[i + 1].time)
			{
				i1 = i;
				i2 = i + 1;
				break;
			}
		}

		// if there are no such keys
		if (i1 == -1 || i2 == -1)
		{
			// either take the first
			if (frame < joint->positionKeys[0].time)
			{
				pos[0] = joint->positionKeys[0].key[0];
				pos[1] = joint->positionKeys[0].key[1];
				pos[2] = joint->positionKeys[0].key[2];
			}

			// or the last key
			else if (frame >= joint->positionKeys[numPositionKeys - 1].time)
			{
				pos[0] = joint->positionKeys[numPositionKeys - 1].key[0];
				pos[1] = joint->positionKeys[numPositionKeys - 1].key[1];
				pos[2] = joint->positionKeys[numPositionKeys - 1].key[2];
			}
		}

		// there are such keys, so interpolate using hermite interpolation
		else
		{
			ms3d_keyframe_t *p0 = &joint->positionKeys[i1];
			ms3d_keyframe_t *p1 = &joint->positionKeys[i2];
			ms3d_tangent_t *m0 = &joint->tangents[i1];
			ms3d_tangent_t *m1 = &joint->tangents[i2];

			// normalize the time between the keys into [0..1]
			float t = (frame - joint->positionKeys[i1].time) / (joint->positionKeys[i2].time - joint->positionKeys[i1].time);
			float t2 = t * t;
			float t3 = t2 * t;

			// calculate hermite basis
			float h1 =  2.0f * t3 - 3.0f * t2 + 1.0f;
			float h2 = -2.0f * t3 + 3.0f * t2;
			float h3 =         t3 - 2.0f * t2 + t;
			float h4 =         t3 -        t2;

			// do hermite interpolation
			pos[0] = h1 * p0->key[0] + h3 * m0->tangentOut[0] + h2 * p1->key[0] + h4 * m1->tangentIn[0];
			pos[1] = h1 * p0->key[1] + h3 * m0->tangentOut[1] + h2 * p1->key[1] + h4 * m1->tangentIn[1];
			pos[2] = h1 * p0->key[2] + h3 * m0->tangentOut[2] + h2 * p1->key[2] + h4 * m1->tangentIn[2];
		}
	}

	vec4_t quat = { 0.0f, 0.0f, 0.0f, 1.0f };
	int numRotationKeys = (int) joint->rotationKeys.size();
	if (numRotationKeys > 0)
	{
		int i1 = -1;
		int i2 = -1;

		// find the two keys, where "frame" is in between for the rotation channel
		for (int i = 0; i < (numRotationKeys - 1); i++)
		{
			if (frame >= joint->rotationKeys[i].time && frame < joint->rotationKeys[i + 1].time)
			{
				i1 = i;
				i2 = i + 1;
				break;
			}
		}

		// if there are no such keys
		if (i1 == -1 || i2 == -1)
		{
			// either take the first key
			if (frame < joint->rotationKeys[0].time)
			{
				AngleQuaternion(joint->rotationKeys[0].key, quat);
			}

			// or the last key
			else if (frame >= joint->rotationKeys[numRotationKeys - 1].time)
			{
				AngleQuaternion(joint->rotationKeys[numRotationKeys - 1].key, quat);
			}
		}

		// there are such keys, so do the quaternion slerp interpolation
		else
		{
			float t = (frame - joint->rotationKeys[i1].time) / (joint->rotationKeys[i2].time - joint->rotationKeys[i1].time);
			vec4_t q1;
			AngleQuaternion(joint->rotationKeys[i1].key, q1);
			vec4_t q2;
			AngleQuaternion(joint->rotationKeys[i2].key, q2);
			QuaternionSlerp(q1, q2, t, quat);
		}
	}

	// make a matrix from pos/quat
	float matAnimate[3][4];
	QuaternionMatrix(quat, matAnimate);
	matAnimate[0][3] = pos[0];
	matAnimate[1][3] = pos[1];
	matAnimate[2][3] = pos[2];

	// animate the local joint matrix using: matLocal = matLocalSkeleton * matAnimate
	R_ConcatTransforms(joint->matLocalSkeleton, matAnimate, joint->matLocal);

	// build up the hierarchy if joints
	// matGlobal = matGlobal(parent) * matLocal
	if (joint->parentIndex == -1)
	{
		memcpy(joint->matGlobal, joint->matLocal, sizeof(joint->matGlobal));
	}
	else
	{
		ms3d_joint_t *parentJoint = &m_joints[joint->parentIndex];
		R_ConcatTransforms(parentJoint->matGlobal, joint->matLocal, joint->matGlobal);
	}
}

float msModel::GetAnimationFps() const
{
	return m_animationFps;
}

float msModel::GetCurrentFrame() const
{
	return m_currentTime;
}

int msModel::GetTotalFrames() const
{
	return m_totalFrames;
}

void msModel::TransformVertex(const ms3d_vertex_t *vertex, vec3_t out) const
{
	int jointIndices[4], jointWeights[4];
	FillJointIndicesAndWeights(vertex, jointIndices, jointWeights);

	if (jointIndices[0] < 0 || jointIndices[0] >= (int) m_joints.size() || m_currentTime < 0.0f)
	{
		out[0] = vertex->vertex[0];
		out[1] = vertex->vertex[1];
		out[2] = vertex->vertex[2];
	}
	else
	{
		// count valid weights
		int numWeights = 0;
		for (int i = 0; i < 4; i++)
		{
			if (jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int) m_joints.size())
				++numWeights;
			else
				break;
		}

		// init
		out[0] = 0.0f;
		out[1] = 0.0f;
		out[2] = 0.0f;

		float weights[4] = { (float) jointWeights[0] / 100.0f, (float) jointWeights[1] / 100.0f, (float) jointWeights[2] / 100.0f, (float) jointWeights[3] / 100.0f };
		if (numWeights == 0)
		{
			numWeights = 1;
			weights[0] = 1.0f;
		}
		// add weighted vertices
		for (int i = 0; i < numWeights; i++)
		{
			const ms3d_joint_t *joint = &m_joints[jointIndices[i]];
			vec3_t tmp, vert;
			VectorITransform(vertex->vertex, joint->matGlobalSkeleton, tmp);
			VectorTransform(tmp, joint->matGlobal, vert);

			out[0] += vert[0] * weights[i];
			out[1] += vert[1] * weights[i];
			out[2] += vert[2] * weights[i];
		}
	}
}

void msModel::TransformNormal(const ms3d_vertex_t *vertex, const vec3_t normal, vec3_t out) const
{
	int jointIndices[4], jointWeights[4];
	FillJointIndicesAndWeights(vertex, jointIndices, jointWeights);

	if (jointIndices[0] < 0 || jointIndices[0] >= (int) m_joints.size() || m_currentTime < 0.0f)
	{
		out[0] = normal[0];
		out[1] = normal[1];
		out[2] = normal[2];
	}
	else
	{
		// count valid weights
		int numWeights = 0;
		for (int i = 0; i < 4; i++)
		{
			if (jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int) m_joints.size())
				++numWeights;
			else
				break;
		}

		// init
		out[0] = 0.0f;
		out[1] = 0.0f;
		out[2] = 0.0f;

		float weights[4] = { (float) jointWeights[0] / 100.0f, (float) jointWeights[1] / 100.0f, (float) jointWeights[2] / 100.0f, (float) jointWeights[3] / 100.0f };
		if (numWeights == 0)
		{
			numWeights = 1;
			weights[0] = 1.0f;
		}
		// add weighted vertices
		for (int i = 0; i < numWeights; i++)
		{
			const ms3d_joint_t *joint = &m_joints[jointIndices[i]];
			vec3_t tmp, norm;
			VectorIRotate(normal, joint->matGlobalSkeleton, tmp);
			VectorRotate(tmp, joint->matGlobal, norm);

			out[0] += norm[0] * weights[i];
			out[1] += norm[1] * weights[i];
			out[2] += norm[2] * weights[i];
		}
	}
}

void msModel::FillJointIndicesAndWeights(const ms3d_vertex_t *vertex, int jointIndices[4], int jointWeights[4]) const
{
	jointIndices[0] = vertex->boneId;
	jointIndices[1] = vertex->boneIds[0];
	jointIndices[2] = vertex->boneIds[1];
	jointIndices[3] = vertex->boneIds[2];
	jointWeights[0] = 100;
	jointWeights[1] = 0;
	jointWeights[2] = 0;
	jointWeights[3] = 0;
	if (vertex->weights[0] != 0 || vertex->weights[1] != 0 || vertex->weights[2] != 0)
	{
		jointWeights[0] = vertex->weights[0];
		jointWeights[1] = vertex->weights[1];
		jointWeights[2] = vertex->weights[2];
		jointWeights[3] = 100 - (vertex->weights[0] + vertex->weights[1] + vertex->weights[2]);
	}
}

void msModel::Render(bool withMaterial, bool flatShaded)
{
	int numGroups = GetNumGroups();

	if (isWaveframe == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(0.5f);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0; i < numGroups; i++)
	{
		ms3d_group_t *group = GetGroup(i);
		
		if (withMaterial)
			BindMaterial(group->materialIndex);
		else
			BindMaterial(-1);
		glEnable(GL_COLOR);
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < group->triangleIndices.size(); j++)
		{
			ms3d_triangle_t *triangle = GetTriangle(group->triangleIndices[j]);
			for (int v = 0; v < 3; v++)
			{
				ms3d_vertex_t *vertex = GetVertex(triangle->vertexIndices[v]);

				glTexCoord2f(triangle->s[v], triangle->t[v]);

				float normal[3];
				if (flatShaded)
					TransformNormal(vertex, triangle->normal, normal);
				else
					TransformNormal(vertex, triangle->vertexNormals[v], normal);
				glNormal3fv(normal);

				float pos[3];
				TransformVertex(vertex, pos);
				if (renderjoint&&(vertex->renderColor[0]!= 0 || vertex->renderColor[1]!= 0 || vertex->renderColor[2]!= 0))
					glColor3f(vertex->renderColor[0], vertex->renderColor[1], vertex->renderColor[2]);
				else if (int(vertex->boneId) == 0)
					glColor3f(1, 1, 1);
				else if(int(vertex->boneId) == 1|| int(vertex->boneId) == 5|| int(vertex->boneId) == 9|| int(vertex->boneId) == 13|| int(vertex->boneId) == 17)
					glColor3f(1, 0, 0);
				else if (int(vertex->boneId) == 2 || int(vertex->boneId) == 6 || int(vertex->boneId) == 10 || int(vertex->boneId) == 14 || int(vertex->boneId) == 18)
					glColor3f(0, 1, 0);
				else if (int(vertex->boneId) == 3 || int(vertex->boneId) == 7 || int(vertex->boneId) == 11 || int(vertex->boneId) == 15 || int(vertex->boneId) == 19)
					glColor3f(0, 0, 1);
				glVertex3fv(pos);
			}
		}
		glEnd();
	}
}
void msModel::RenderJoint() {
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < GetNumJoints(); i++)
	{
		ms3d_joint_t *joint = GetJoint(i);
		if (joint->parentIndex == -1)
		{
			glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
			glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
		}
		else
		{
			ms3d_joint_t *parentJoint = GetJoint(joint->parentIndex);
			glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
			glVertex3f(parentJoint->matGlobal[0][3], parentJoint->matGlobal[1][3], parentJoint->matGlobal[2][3]);
		}
	}
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < GetNumJoints(); i++)
	{
		ms3d_joint_t *joint = GetJoint(i);
		glVertex3f(joint->matGlobal[0][3], joint->matGlobal[1][3], joint->matGlobal[2][3]);
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}
void msModel::reset() {
	for (int i = 0; i < 20; i++)
		m_dof[i] = 0;
	for (size_t i = 0; i < m_joints.size(); i++)
	{
		ms3d_joint_t *joint = &m_joints[i];
		memcpy( joint->matLocal, joint->matInitLocal, sizeof(joint->matLocal));
		memcpy(joint->matGlobal, joint->matInitGlobal, sizeof(joint->matGlobal));
	}
}
void msModel::getRandomAngle(std::vector<float>dof) {

	//get random angle
	m_dof = dof;
	Vector3 vec[16];
	Vector3 axis[21];
	float quaternion[21][4] = { 0 };
	float randomRotMat[21][3][4] = { 0 };
	//calculate axis
	for (int i = 0; i < 5; i++) {
		vec[3 * i + 1] = m_joints[4*i+1].initPos - m_joints[0].initPos;
		for (int j = 1; j < 3; j++) {
			vec[3 * i + j + 1] = m_joints[4 * i + j + 1].initPos - m_joints[4 * i + j ].initPos;
		}
	}


	for (int i = 0; i < 5; i++) {
		axis[4 * i + 1] = vec[3 * i + 1].normalize();
		for (int j = 1; j <3; j++) {
			axis[4 * i + j + 1] = vec[3 * i + j].crossProduct(vec[3 * i + j + 1]);
			axis[4 * i + j + 1].y = 0;
			axis[4 * i + j + 1].normalize();
		}
		axis[4 * i + 4] = axis[4 * i + 3];
	}
	//axis-angle to rot matrix
	//convert to quartertion
	for (int i = 0; i < 20; i++) {
		quaternion[i + 1][3] = cos(dof[i] / 2);
		quaternion[i + 1][0] = axis[i + 1].x*sin(dof[i] / 2);
		quaternion[i + 1][1] = axis[i + 1].y*sin(dof[i] / 2);
		quaternion[i + 1][2] = axis[i + 1].z*sin(dof[i] / 2);
	}
	//convert to rotation maxtrix
	for (int i = 1; i < 21; i++)
		QuaternionMatrix(quaternion[i], randomRotMat[i]);

	//re-calculate local-global matrix
	for (int i = 0; i < 5; i++) {
		float tmp[3][4] = { 0 };
		R_ConcatTransforms2(randomRotMat[4 * i + 1], m_joints[4 * i + 1].matInitGlobal, tmp);
		R_ConcatTransforms2(randomRotMat[4 * i + 2], tmp, m_joints[4 * i + 1].matGlobal);
		R_ConcatTransforms(m_joints[4 * i + 1].matGlobal, m_joints[4 * i + 2].matLocal, m_joints[4 * i + 2].matGlobal);
		memcpy(tmp, m_joints[4 * i + 2].matGlobal, sizeof(m_joints[4 * i + 2].matGlobal));
		R_ConcatTransforms2(randomRotMat[4 * i + 3], tmp, m_joints[4 * i + 2].matGlobal);
		R_ConcatTransforms(m_joints[4 * i + 2].matGlobal, m_joints[4 * i + 3].matLocal, m_joints[4 * i + 3].matGlobal);
		memcpy(tmp, m_joints[4 * i + 3].matGlobal, sizeof(m_joints[4 * i + 3].matGlobal));
		R_ConcatTransforms2(randomRotMat[4 * i + 4], tmp, m_joints[4 * i + 3].matGlobal);
		R_ConcatTransforms(m_joints[4 * i + 3].matGlobal, m_joints[4 * i + 4].matLocal, m_joints[4 * i + 4].matGlobal);
	}
}
void msModel::BindMaterial(int materialIndex)
{
	if (materialIndex < 0 || materialIndex >= GetNumMaterials())
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glColor4f(1, 1, 1, 1);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, 0);
		float ma[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float md[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		float ms[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float me[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float mss = 0.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ma);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, md);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ms);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, me);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mss);
	}
	else
	{
		ms3d_material_t *material = GetMaterial(materialIndex);
		glEnable(GL_TEXTURE_2D);

		if (material->transparency < 1.0f || material->mode & HASALPHA)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0f, 1.0f, 1.0f, material->transparency);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

			if (GetTransparencyMode() == TRANSPARENCY_MODE_SIMPLE)
			{
				glDepthMask(GL_FALSE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.0f);
			}
			else if (GetTransparencyMode() == TRANSPARENCY_MODE_ALPHAREF)
			{
				glDepthMask(GL_TRUE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, GetAlphaRef());
			}
		}
		else
		{
			glDisable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		}

		if (material->mode & SPHEREMAP)
		{
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
		else
		{
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
		glBindTexture(GL_TEXTURE_2D, material->id);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->emissive);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
	}
}
void msModel::reAlignJoint() {
	for (int i = 0; i < m_vertices.size(); i++) {
		int tmp_id = m_vertices[i].boneId;
		m_vertices[i].boneId = 0;
		float pos[3];
		Vector3 vec(m_vertices[i].vertex[0], m_vertices[i].vertex[1], m_vertices[i].vertex[2]);
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 3; k++) {
				if ((vec - m_joints[0].initPos).length() > (m_joints[4 * j + 1 + k].initPos - m_joints[0].initPos).length()
					&& (vec - m_joints[4 * j + 1 + k].initPos).length() < (vec - m_joints[0].initPos).length()) {
					if ((4 * j + 1 + k - 1) / 4 == (tmp_id - 1) / 4) {
						m_vertices[i].boneId = (char)(4 * j + 1 + k);

					}
				}
				bool debug = true;
				float ratio = 1.0 / 15;
				if (k == 1)ratio = 1.0 / 10;
				if (k == 2)ratio = 1.0 / 10;
				float len1 = (vec - m_joints[0].initPos).length(), len2 = (m_joints[4 * j + 1 + k].initPos - m_joints[0].initPos).length();
				float len3 = (vec - m_joints[4 * j + 1 + k].initPos).length();
				float thred = (k == 0) ? ratio*(m_joints[4 * j + k + 1].initPos - m_joints[0].initPos).length() :
										 ratio*(m_joints[4 * j + k + 1].initPos - m_joints[4 * j + k].initPos).length();
				if (debug &&abs(len1 - len2)< thred&&
					(vec - m_joints[4 * j + 1 + k].initPos).length() < (vec - m_joints[0].initPos).length()
						&& (4 * j + 1 + k - 1) / 4 == (tmp_id - 1) / 4) {
					m_vertices[i].boneId= (k == 0) ? (char)0 : (char)(4 * j + 1 + k - 1); 
					m_vertices[i].boneIds[0] = (char)(4 * j + 1 + k);
					m_vertices[i].weights[0] = 100-(50 + (len1- len2) / thred * 50);
					m_vertices[i].weights[1] = 100 -m_vertices[i].weights[0];
					m_vertices[i].renderColor[0] = 1;
					m_vertices[i].renderColor[1] = 1;
					m_vertices[i].renderColor[2] = 0;

				}
			}
		}
	}
	
	int numGroups = GetNumGroups();
	std::vector<float>flags(GetNumVertices());
	std::vector<float>flags2(GetNumVertices());
	for (int i = 0; i < numGroups; i++)
	{
		ms3d_group_t *group = GetGroup(i);
		for (size_t j = 0; j < group->triangleIndices.size(); j++)
		{
			ms3d_triangle_t *triangle = GetTriangle(group->triangleIndices[j]);
			for (int v = 0; v < 3; v++)
			{
				ms3d_vertex_t *vertex = GetVertex(triangle->vertexIndices[v%3]);
				ms3d_vertex_t *vertex2 = GetVertex(triangle->vertexIndices[(v+1)%3]);
				ms3d_vertex_t *vertex3 = GetVertex(triangle->vertexIndices[(v+2)%3]);
				if (vertex->boneIds[0]!=0&&vertex2->boneIds[0]==0&&vertex3->boneIds[0]==0) {
					flags[triangle->vertexIndices[v]] = vertex2->boneId;
					flags2[triangle->vertexIndices[v]] = vertex->boneIds[0];
					break;
				}	
			}
		}
	}
	
	for (int i = 0; i < flags.size(); i++) {
		if (flags[i] != 0) {
			m_vertices[i].boneId = flags[i];
			m_vertices[i].boneIds[0] = flags2[i];
			m_vertices[i].weights[0] = 50; m_vertices[i].weights[1] = 100- m_vertices[i].weights[0];
		}
	}
}
void msModel::saveOBJ(const char *filename) {
	FILE* fp = fopen(filename,"w");
	if (!fp) {
		fprintf(stderr, "Cannot open input file.\n");
		return;
	}
	//save vertex
	fprintf(fp,"mtllib hand.mtl\n");
	for (int i = 0; i <m_triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			float pos[3];
			TransformVertex(GetVertex(m_triangles[i].vertexIndices[j]), pos);
			fprintf(fp, "v %f %f %f\n", pos[0], pos[1], pos[2]);
		}
	}
	//save vt
	
	for (int i = 0; i < m_triangles.size(); i++) 
		for(int j=0;j<3;j++)
			fprintf(fp, "vt %f %f\n",m_triangles[i].s[j] , m_triangles[i].t[j]);
	
	fprintf(fp,"usemtl alt\n");
	//save face
	for (int i = 0; i < m_triangles.size(); i++) {
		fprintf(fp, "f %d/%d %d/%d %d/%d\n", 3 * i + 1, 3 * i + 1, 3 * i + 2, 3 * i + 2, 3 * i + 3, 3 * i + 3);
	}
	fclose(fp);
}

bool msModel::SaveToMs3d(const char* lpszFileName)
{
	FILE *fp = fopen(lpszFileName, "w");
	if (!fp)
		return false;

	//header
	ms3d_header_t header;
	header.id[0] = 'M';
	header.id[1] = 'S';
	header.id[2] = '3';
	header.id[3] = 'D';
	header.id[4] = '0';
	header.id[5] = '0';
	header.id[6] = '0';
	header.id[7] = '0';
	header.id[8] = '0';
	header.id[9] = '0';
	header.version = 4;
	fwrite(&header, sizeof(ms3d_header_t), 1, fp);

	// vertices
	word nNumVertices = m_vertices.size();
	fwrite(&nNumVertices, sizeof(word), 1, fp);
	fwrite(&m_vertices[0], sizeof(ms3d_vertex_t), nNumVertices, fp);


	// triangles
	word nNumTriangles = m_triangles.size();
	fwrite(&nNumTriangles, sizeof(word), 1, fp);
	fwrite(&m_triangles[0], sizeof(ms3d_triangle_t), nNumTriangles, fp);


	// groups
	word nNumGroups = m_groups.size();
	fwrite(&nNumGroups, sizeof(word), 1, fp);

	word i;
	for (i = 0; i < nNumGroups; i++)
	{
		int num = m_groups[i].triangleIndices.size();
		fwrite(&m_groups[i].flags, sizeof(byte), 1, fp);
		fwrite(&m_groups[i].name, sizeof(char), 32, fp);
		fwrite(&num, sizeof(word), 1, fp);
		fwrite(m_groups[i].triangleIndices.data(), sizeof(word), m_groups[i].triangleIndices.size(), fp);
		fwrite(&m_groups[i].materialIndex, sizeof(char), 1, fp);
	}


	// materials
	word nNumMaterials = m_materials.size();
	fwrite(&nNumMaterials, sizeof(word), 1, fp);
	fwrite(&m_materials[0], sizeof(ms3d_material_t), nNumMaterials, fp);

	fwrite(&m_animationFps, sizeof(float), 1, fp);
	fwrite(&m_currentTime, sizeof(float), 1, fp);
	fwrite(&m_totalFrames, sizeof(int), 1, fp);

	// joints
	word nNumJoints = m_joints.size();
	fwrite(&nNumJoints, sizeof(word), 1, fp);
	for (i = 0; i < nNumJoints; i++)
	{
		int num1 = m_joints[i].rotationKeys.size(); 
		int num2 = m_joints[i].positionKeys.size();
		fwrite(&m_joints[i].flags, sizeof(byte), 1, fp);
		fwrite(&m_joints[i].name, sizeof(char), 32, fp);
		fwrite(&m_joints[i].parentName, sizeof(char), 32, fp);
		fwrite(&m_joints[i].rot, sizeof(float), 3, fp);
		fwrite(&m_joints[i].pos, sizeof(float), 3, fp);
		fwrite(&num1, sizeof(word), 1, fp);
		fwrite(&num2, sizeof(word), 1, fp);
		fwrite(m_joints[i].rotationKeys.data(), sizeof(ms3d_keyframe_t), num1, fp);
		fwrite(m_joints[i].positionKeys.data(), sizeof(ms3d_keyframe_t),num2, fp);
	}

	fclose(fp);
	return true;
}

void msModel::saveJoint(const char *filename) {
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Cannot open input file.\n");
		return;
	}
	fprintf(fp, "dof:\n");
	for (int i = 0; i < 20; i++) {
		fprintf(fp, "%f ", m_dof[i]);
	}
	fprintf(fp, "\njoint position:\n");
	for (int i = 0; i < 21; i++) {
		fprintf(fp, "%s %f %f %f\n", m_joints[i].name,m_joints[i].matGlobal[0][3], m_joints[i].matGlobal[1][3], m_joints[i].matGlobal[2][3]);
	}
	fclose(fp);
}

void msModel::renderWord() {

	glMatrixMode(GL_PROJECTION);  
	glPushMatrix();               
	glLoadIdentity();             
	glOrtho(0, 511, 0, 511, -1, 1);    
	glMatrixMode(GL_MODELVIEW);   
	glPushMatrix();               
	glLoadIdentity();             

	char sz[128];
	int fontHeight = 12,line=512;
	sprintf(sz, "%d vertices", GetNumVertices());
	print(2, line, sz);
	line -= (fontHeight + 2);

	sprintf(sz, "%d triangles", GetNumTriangles());
	print(2, line, sz);
	line -= (fontHeight + 2) ;

	sprintf(sz, "%d groups", GetNumGroups());
	print(2, line, sz);
	line -= (fontHeight + 2) ;

	sprintf(sz, "%d materials", GetNumMaterials());
	print(2, line, sz);
	line -= (fontHeight + 2) ;

	sprintf(sz, "%d joints", GetNumJoints());
	print(2, line, sz);
	line -= (fontHeight + 2) ;

	line = (fontHeight + 2);
	sprintf(sz, "Hit SPACE to change model,Hit E to recover,Hit S to save model!");
	print(2, line, sz);

	glMatrixMode(GL_PROJECTION);  
	glPopMatrix();                
	glMatrixMode(GL_MODELVIEW);   
	glPopMatrix();                
	glEnable(GL_DEPTH_TEST);
}

void msModel::print(int x, int y, const char *string)
{
	size_t len = (int)strlen(string);
	int fontHeight = 12;
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	for (size_t i = 0; i < len; i++)
	{
		glRasterPos2i(x, y - fontHeight);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		x += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
}