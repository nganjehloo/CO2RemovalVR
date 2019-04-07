#ifndef _MODEL_H
#define _MODEL_H

// TODO: turn Model into an abstract class that Factory or CO2 class derives from?

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>

#include "mesh.h"

using namespace std;

class Model
{
public:
	Model(GLchar* path);
	Model() {}
	~Model();

	void draw(GLuint shaderProgram);

	vector<Mesh> getMeshes() { return meshes; }

protected:
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		string typeName);
	GLint textureFromFile(const char* path, string directory);
};

#endif