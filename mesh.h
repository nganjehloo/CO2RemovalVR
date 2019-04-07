#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h>

using namespace std;

static glm::vec3 origin = glm::vec3(0.0f, -5.0f, 0.0f);

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	glm::vec3 ambient, diffuse, specular;
	float shininess;

	glm::mat4 toWorld;

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures,
		 glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
	~Mesh();

	void draw(GLuint shaderProgram);

private:
	GLuint VAO, VBO, EBO;
	void setupMesh();
	void setupGLBuffers();
};

#endif