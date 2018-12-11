#include "water.h"
#include<iostream>
#define z_delta 4.0
#define x_delta 4.0
#define y_delta 1.0
water::water()
{
}

water::water(QVector3D LT,QVector3D RB):leftTop(LT),rightBottom(RB){
	map = new QOpenGLTexture(QImage("./Textures/HeightMap.png"));
}


water::~water() {

}
void water::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j<4; j++)
		for (int k = 0; k<4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void water::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,QVector3D eyeLoc)
{
	static float T = 0;
	GLfloat P[4][4];
	GLfloat M[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, M);

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	T++;
	GLfloat  amplitude[8] = { .1f ,0.01f ,0.1f ,0.1f ,0.2f ,0.0f,0.15f,0.3f },
		wavelength[8] = { 1000.0f ,730.0f ,86.0f ,60.0f ,48.0f,90.0f,70.0f ,100.0f },
		speed[8] = { 1.0f ,0.1f ,0.08f ,0.07f ,0.02f ,0.06f ,0.01f , 0.03f };
	QVector2D direction[8] = {
		QVector2D(1,0),
		QVector2D(1,1),
		QVector2D(1,3),
		QVector2D(-1,4),
		QVector2D(2,-3),
		QVector2D(-3,-2),
		QVector2D(4,13),
		QVector2D(79,81) };
	//QVector2D direction[8] = {
	//	QVector2D(1,0),
	//	QVector2D(1,0),
	//	QVector2D(1,0),	QVector2D(1,0),	QVector2D(1,0),	QVector2D(1,0),	QVector2D(1,0),	QVector2D(1,0) };

	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("time", T);
	shaderProgram->setUniformValue("ModelViewMatrix", M);
	shaderProgram->setUniformValue("waterHeight", 10.0f);
	shaderProgram->setUniformValue("numWaves", 8);
	shaderProgram->setUniformValueArray("amplitude", amplitude, 8, 1);
	shaderProgram->setUniformValueArray("wavelength", wavelength, 8, 1);
	shaderProgram->setUniformValueArray("speed", speed, 8, 1);
	shaderProgram->setUniformValueArray("direction", direction, 8);
	shaderProgram->setUniformValue("EyePosition", eyeLoc);
	shaderProgram->setUniformValue("hmap", 3);
	shaderProgram->setUniformValue("skybox", 0);
	


	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();


	// Bind the buffer so that it is the current active buffer
	//cvbo.bind();
	// Enable Attribute 1
	//shaderProgram->enableAttributeArray(1);
	// Set Attribute 0 to be color
	//shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	//cvbo.release();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glActiveTexture(GL_TEXTURE0 + 3);
	//glEnable(GL_TEXTURE_2D);
	map->bind();
	//glBindTexture(GL_TEXTURE_2D, HMtexture);
	
	
	//Draw a triangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);


	//unbind vao
	vao.release();
	//unbind vao
	shaderProgram->release();
}
void water::Init()
{
	initializeOpenGLFunctions();
	InitShader("./Shader/water.vs", "./Shader/water.fs");
	InitVAO();
	InitVBO();
	textureID = loadcubemap(path);
}
void water::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void water::InitVBO()
{

	/*float lt[3] = {leftTop.x,leftTop.z,leftTop.y};*/
	//float lt[3];
	//lt[0] = leftTop.x;
	//lt[1] = leftTop.y;
	//lt[2] = leftTop.z;
	//float rb[3] = { rightBottom.x, rightBottom.z, leftTop.y };
	//Set each vertex's position
	float y = leftTop.y();
		for (float x = leftTop.x(), rx = rightBottom.x(); x <  rx; x += x_delta) {
			for (float z = leftTop.z(), rz = rightBottom.z(); z < rz; z += z_delta) {	
				vertices << QVector3D(x, y, z)
					<< QVector3D(x, y, z + z_delta)
					<< QVector3D(x + x_delta, y, z + z_delta)
					<< QVector3D(x, y, z)
					<< QVector3D(x + x_delta, y, z)
					<< QVector3D(x + x_delta, y, z + z_delta);
			}
		}
	//vertices << QVector3D(5.0f, 2.0f, -10.0f)
	//	<< QVector3D(5.0f, 2.0f, 10.0f)
	//	<< QVector3D(15.0f, 2.0f, 0.0f);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's color
	//colors << QVector3D(0.0f, 1.0f, 0.0f)
		//<< QVector3D(1.0f, 0.0f, 0.0f)
		//<< QVector3D(0.0f, 0.0f, 1.0f);
	// Create Buffer for color
	//cvbo.create();
	// Bind the buffer so that it is the current active buffer.
	//cvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	//cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	//cvbo.allocate(colors.constData(), colors.size() * sizeof(QVector3D));


	skyboxVBO.create();
	skyboxVBO.bind();
	skyboxVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	skyboxVBO.allocate(skyboxvertices, sizeof(float) * 108);

}
void water::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}
unsigned int water::loadcubemap(vector<std::string> faces)
{
	glActiveTexture(GL_TEXTURE1);
	GLuint textureID2 = 0;
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID2);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	/*
	*/
	/*GLuint ID2 = 0;
	glGenTextures(1, &ID2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,ID2);
	unsigned char *data = stbi_load("./Textures/HeightMap.png", &width, &height, &nrComponents, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << "./Textures/HeightMap.png" << std::endl;
		stbi_image_free(data);
	}
	HMtexture = ID2;*/


	return textureID2;
}