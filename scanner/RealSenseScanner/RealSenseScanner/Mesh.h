#ifndef MESH_H
#define MESH_H

#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/common.hpp>
#include <glm/gtx/extend.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

struct Vertex {
	//friend std::ostream & operator<<(std::ostream &os, const Vertex &gp);
	//friend class boost::serialization::access;
	//template<class Archive>
	//void serialize(Archive & ar, const unsigned int /* file_version */){
	//	//ar & Position & Normal & TexCoords & Tangent & Bitangent;
	//}
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	//Vertex(){ ; }
	//Vertex(glm::vec3 Position, glm::vec3 Normal, glm::vec2 TexCoords, glm::vec3 Tangent, glm::vec3 Bitangent) :Position(Position),Normal(Normal),TexCoords(TexCoords),Tangent(Tangent),Bitangent(Bitangent){}
};
//std::ostream & operator<<(std::ostream &os, const Vertex &gp)
//{
//	return os << gp.Position.x << "," << gp.Position.y << "," << gp.Position.z << "," << gp.Normal.x << "," << gp.Normal.y << "," << gp.Normal.z << "," << gp.TexCoords.x << "," << gp.TexCoords.y << "," << gp.Tangent.x << "," << gp.Tangent.y << "," << gp.Tangent.z << "," << gp.Bitangent.x << "," << gp.Bitangent.y << "," << gp.Bitangent.z << endl;
//}

struct Texture {
	//friend std::ostream & operator<<(std::ostream &os, const Texture &gp);
	//friend class boost::serialization::access;
	//template<class Archive>
	//void serialize(Archive & ar, const unsigned int /* file_version */){
	//	//ar & id & type & path;
	//}
	GLuint id;
	string type;
	aiString path;
	//Texture(){ ; }
	//Texture(GLuint id,string type,aiString path):id(id),type(type),path(path){}
};
//std::ostream & operator<<(std::ostream &os, const Texture &gp)
//{
//	return os << gp.id << "," << gp.type << "," << gp.path.C_Str << endl;
//}

class Mesh {

	////friend std::ostream & operator<<(std::ostream &os, const Mesh &gp);
	//friend class boost::serialization::access;
	//template<class Archive>
	//void serialize(Archive & ar, const unsigned int /* file_version */){
	//	ar & vertices & indices & textures;
	//	//if (Archive::is_loading::value)
	//	//{
	//	//	assert(m.vertices == nullptr&&m.indices == nullptr&&m.textures == nullptr);
	//	//	m.vertices = new vector<Vertex>();
	//	//	m.indices = new vector<GLuint>();
	//	//	m.textures = new vector<Texture>();
	//	//}
	//	//ar & make_array<float>(m.vertices, m.vertices.size);
	//	//ar & make_array<float>(m.indices, m.indices.size);
	//	//ar & make_array<float>(m.textures, m.textures.size);

	//}

public:

	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	GLuint VAO;

	Mesh()
	{
		;
	}

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
	}
};

//std::ostream & operator<<(std::ostream &os, const Mesh &gp)
//{
//	vector<Vertex> v = gp.vertices;
//	vector<Texture> t = gp.textures;
//	for (std::vector<Vertex>::iterator it = v.begin(); it != v.end(); ++it) {
//		os << it->Position.x << "," << it->Position.y << "," << it->Position.z << "," << it->Normal.x << "," << it->Normal.y << "," << it->Normal.z << "," << it->TexCoords.x << "," << it->TexCoords.y << "," << it->Tangent.x << "," << it->Tangent.y << "," << it->Tangent.z << "," << it->Bitangent.x << "," << it->Bitangent.y << "," << it->Bitangent.z << ",";
//	}
//	for (std::vector<Texture>::iterator it = t.begin(); it != t.end(); ++it) {
//		os << it->id << "," << it->type << "," << it->path.C_Str << endl;
//	}
//	return os;
//}

#endif