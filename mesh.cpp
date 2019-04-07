#include "mesh.h"
#include "Window.h"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures,
		   glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;

	this->toWorld = glm::mat4(1.0f);
	this->toWorld = glm::scale(toWorld, glm::vec3(0.5f, 0.5f, 0.5f));
	this->toWorld = glm::translate(toWorld, origin);

	this->setupMesh();
}

Mesh::~Mesh()
{
	// clean up buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	vertices.clear();
	indices.clear();
	textures.clear();
}

void Mesh::setupMesh()
{
	// Create array object and buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the vertex array object (VAO) first, then bind the associated buffers to it.
	glBindVertexArray(VAO);
	setupGLBuffers();
	glBindVertexArray(0);
}

void Mesh::setupGLBuffers()
{
	// copy vertices into vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// copy the face indices unto element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Pass the vertex position data to OpenGL
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// same as number in "layout (location = x)" in vertex shader
		3,				// x, y, z components per vertex
		GL_FLOAT,		// component type
		GL_FALSE,		// don't normalize
		sizeof(Vertex),	// offset between consecutive indices
		(GLvoid*)0		// offset of the vertex's component
	);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Vertex Texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
}

void Mesh::draw(GLuint shaderProgram)
{
	GLuint diffuseNum = 1;
	GLuint specularNum = 1;

	// Bind the textures
	// NOTE: this is not needed in this assignment, but may be later
	//for (GLuint i = 0; i < this->textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);

	//	std::string name = this->textures[i].type;
	//	std::string number = (name == "texture_diffuse") ? std::to_string(diffuseNum++) : std::to_string(specularNum++);

	//	glUniform1i(glGetUniformLocation(shaderProgram, (name + number).c_str()), i);
	//	glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	//}

	// calculate the modelview
	glm::mat4 modelview = Window::V * toWorld;

	// propagate matrices to the shader program
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	GLuint mvLoc = glGetUniformLocation(shaderProgram, "modelview");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, &modelview[0][0]);

	// pass the material properties to the shader
	GLint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");

	glUniform3fv(matAmbientLoc, 1, &ambient[0]);
	glUniform3fv(matDiffuseLoc, 1, &diffuse[0]);
	glUniform3fv(matSpecularLoc, 1, &specular[0]);
	glUniform1f(matShineLoc, shininess);

	// draw the mesh
	glBindVertexArray(VAO);
	setupGLBuffers();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	// NOTE: this is not needed in this assignment, but may be later
	//for (GLuint i = 0; i < this->textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
}