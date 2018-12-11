#include "skybox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"
#define SCALE 350.0;
#define OFFSET_X 0.0f
#define OFFSET_Y 0.0f
#define OFFSET_Z 0.0f
namespace
{
	void DimensionTransformation(GLfloat source[], GLfloat target[][4])
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
}

skybox::skybox()
{
	for (int i = 0; i < 36; i++) {
		for (int k = 0; k < 3; k++) {
			skyboxvertices[3*i+k] *= SCALE;
			switch (k)
			{
			case 0:
				skyboxvertices[3*i + k] += OFFSET_X;
				break;
			case 1:
				skyboxvertices[3*i + k] += OFFSET_Y;
				break;
			case 2:
				skyboxvertices[3*i + k] += OFFSET_Z;
				break;
			default:
				break;
			}
		}
	}
	initializeOpenGLFunctions();
}


skybox::~skybox()
{
}


//Unused
unsigned int skybox::loadtexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		stbi_image_free(data);
	}
	else
	{
		std::cout << "texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int skybox::loadcubemap(vector<std::string> faces)
{
	GLuint textureID2 = 0;
	glGenTextures(1,&textureID2);
	glBindTexture(GL_TEXTURE_CUBE_MAP,textureID2);

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

	return textureID2;
}

void skybox::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat V[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, V);

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
	shaderProgram->setUniformValue("skybox", 0);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("projection", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("view", V);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	//Bind SkyCubeVBO
	skycubeVBO.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	skycubeVBO.release();

	//Bind Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
	glDrawArrays(GL_TRIANGLES, 0, 108);

	vao.release();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void skybox::Init() {
	InitShader("./Shader/cubemap.vs", "./Shader/cubemap.fs");
	InitVAO();
	InitVBO();
	textureID = loadcubemap(path);
}

void skybox::InitVAO() {
	vao.create();
	vao.bind();
}

void  skybox::InitVBO() {
	skycubeVBO.create();
	skycubeVBO.bind();
	skycubeVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	skycubeVBO.allocate(skyboxvertices, sizeof(float) * 108);
}

void skybox::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

