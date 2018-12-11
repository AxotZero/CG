#pragma once

#include <vector>
#include <iostream>
#include <qtgui/qopenglfunctions_4_3_core>
#include <qtgui/qopenglvertexarrayobject>
#include <qtgui/qopenglbuffer>
#include <qtgui/qopenglshader>
#include <qtgui/qopenglshaderprogram>
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <qopenglfunctions.h>
#include <qvector>
#include <qvector3d>
#include <qfileinfo>
#include <qdebug>
#include <qstring>
#include "qvector2d.h"
#include "stb\stb_image.h"

using namespace std;

class skybox : protected QOpenGLFunctions
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	//qvector<qvector3d> colors;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer skycubeVBO;
	//qopenglbuffer cvbo;

	vector<string> path = {
		"./Textures/right.jpg",
		"./Textures/left.jpg",
		"./Textures/top.jpg",
		"./Textures/bottom.jpg",
		"./Textures/back.jpg",
		"./Textures/front.jpg"
	};

	unsigned int textureID;

	skybox();
	~skybox();
	unsigned int loadtexture(char const * path);
	unsigned int loadcubemap(vector<std::string> faces);
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);

	float skyboxvertices[108] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	
};

