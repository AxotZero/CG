#include "Model2.h"
#include <iostream>

#include "Include/Assimp/Importer.hpp"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/postprocess.h"
#include <GLM/glm/glm.hpp>
using namespace std;
#define DEBUG
Model2::Model2()
{
	this->initPos = glm::vec3(0, 0, 0);
	this->initScale = 1;
}
Model2::~Model2()
{
}

void Model2::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix) {
	for (int i = 0; i < meshes.size(); i++) {
		glActiveTexture(GL_TEXTURE0);
		myTexture->bind();
		meshes[i].shaderProgram->setAttributeValue("texture", 0);
		if (NormF) {
			glActiveTexture(GL_TEXTURE1);
			NTexture->bind();
			meshes[i].shaderProgram->setAttributeValue("normalmap", 1);
		}
		if (SpecF) {
			glActiveTexture(GL_TEXTURE2);
			SpecTexture->bind();
			meshes[i].shaderProgram->setAttributeValue("Specmap", 2);
		}
		meshes[i].Paint(ProjectionMatrix, ModelViewMatrix);
	}

}