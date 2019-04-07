#include "model.h"
#include <iostream>

Model::Model(GLchar* path)
{
	this->loadModel(path);
}

Model::~Model()
{
	meshes.clear();
	textures_loaded.clear();
}

void Model::draw(GLuint shaderProgram)
{
	// draw all meshes associated with this model
	for (GLuint i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].draw(shaderProgram);
	}
}

void Model::loadModel(string path)
{
	// import the model
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cerr << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	
	// retrieve the directory path of the file
	this->directory = path.substr(0, path.find_last_of('/'));

	// process the nodes of the model
	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	// process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		glm::vec3 temp;

		// Process vertex positions
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		vertex.position = temp;

		// Process vertex normals
		temp.x = mesh->mNormals[i].x;
		temp.y = mesh->mNormals[i].y;
		temp.z = mesh->mNormals[i].z;
		vertex.normal = temp;

		// Process vertex tex coords, if present
		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		// retrieve all indices of each face
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process mesh materials and textures, if present
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Get material properties
		aiColor3D ambientColor, diffuseColor, specularColor;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		material->Get(AI_MATKEY_SHININESS, shininess);
		ambient.x = ambientColor.r; ambient.y = ambientColor.g; ambient.z = ambientColor.b;
		diffuse.x = diffuseColor.r; diffuse.y = diffuseColor.g; diffuse.z = diffuseColor.b;
		specular.x = specularColor.r; specular.y = specularColor.g; specular.z = specularColor.b;
		
		// NOTE: this is not needed in this assignment, but may be later
		//// diffuse maps 
		//vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
		//	aiTextureType_DIFFUSE, "texture_diffuse");
		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//
		//// specular maps
		//vector<Texture> specularMaps = this->loadMaterialTextures(material,
		//	aiTextureType_SPECULAR, "texture_specular");
		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures, ambient, diffuse, specular, shininess);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}

		// If texture hasn't been loaded already, load it
		if (!skip)
		{   			
			Texture texture;
			texture.id = textureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);

			// Store it as texture loaded for entire model, to ensure 
			// we won't unneccessarily load duplicate textures.
			this->textures_loaded.push_back(texture);  
		}
	}
	return textures;
}

GLint Model::textureFromFile(const char* path, string directory)
{
	// generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}
