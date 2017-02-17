#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

using namespace std;

inline GLint TextureFromFile(const char* path, string directory, bool gamma = false);

class Model
{
	//friend std::ostream & operator<<(std::ostream &os, const Model &gp);
	//friend class boost::serialization::access;
	//template<class Archive>
	//void serialize(Archive & ar, const unsigned int /* file_version */){
	//	ar & meshes;
	//}

public:
	vector<Mesh> meshes;

	Model(string const & path)
	{
		this->loadModel(path);
		
	}

	Model()
	{
		;
	}

private:
	vector<Texture> textures_loaded;
	string directory;
	void loadModel(string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		this->directory = path.substr(0, path.find_last_of('/'));
		this->processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else{ vertex.TexCoords = glm::vec2(0.0f, 0.0f); }
			if (mesh->HasTangentsAndBitangents()){
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			vertices.push_back(vertex);
		}
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			std::vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		// Dump vertex and index data to the output VBO file
		const char* env_p = std::getenv("MESH_PATH");
		char str_dat[250];
		strcpy(str_dat, env_p);
		strcat(str_dat, "\\");
		strcat(str_dat, "mesh.dat");
		ofstream vboFile(str_dat, ofstream::out | ofstream::binary);

		unsigned int numVertices = vertices.size();
		unsigned int numIndices = indices.size();
		unsigned int numTextures = textures.size();
		vboFile.write(reinterpret_cast<char*>(&numVertices), sizeof(unsigned int));
		vboFile.write(reinterpret_cast<char*>(&numIndices), sizeof(unsigned int));
		vboFile.write(reinterpret_cast<char*>(&numTextures), sizeof(unsigned int));
		vboFile.write(reinterpret_cast<char*>(&vertices[0]), sizeof(Vertex)* vertices.size());
		vboFile.write(reinterpret_cast<char*>(&indices[0]), sizeof(int)* indices.size());
		vboFile.write(reinterpret_cast<char*>(&textures[0]), sizeof(Texture)* textures.size());

		vboFile.close();
		cout << numVertices << endl; //294975
		cout << numIndices << endl; //294975
		cout << numTextures << endl; //1
		cout << vertices.size() << endl; //294975
		cout << indices.size() << endl;//294975
		cout << textures.size() << endl;//1
		return Mesh(vertices, indices, textures);
	}
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;

			mat->GetTexture(type, i, &str);
			cout << str.C_Str() << endl;
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
};

GLint TextureFromFile(const char* path, string directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}
//std::ostream & serialize(std::ostream &os, const Model &gp)
//{
//	vector<Mesh> meshes_vector = gp.meshes;
//	for (vector<Mesh>::iterator it = meshes_vector.begin(); it != meshes_vector.end(); ++it) {
//		vector<Vertex> v = it->vertices;
//		vector<GLuint> i = it->indices;
//		vector<Texture> t = it->textures;
//		for (std::vector<Vertex>::iterator it = v.begin(); it != v.end(); ++it) {
//			os << it->Position.x << "," << it->Position.y << "," << it->Position.z << "," << it->Normal.x << "," << it->Normal.y << "," << it->Normal.z << "," << it->TexCoords.x << "," << it->TexCoords.y << "," << it->Tangent.x << "," << it->Tangent.y << "," << it->Tangent.z << "," << it->Bitangent.x << "," << it->Bitangent.y << "," << it->Bitangent.z << ",";
//		}
//		for (std::vector<Texture>::iterator it = t.begin(); it != t.end(); ++it) {
//			os << it->id << "," << it->type << "," << it->path.data << ",";
//		}
//		for (std::vector<GLuint>::iterator it = i.begin(); it != i.end(); ++it) {
//			os << *it << endl;
//		}
//	}
//	return os;
//}

#endif