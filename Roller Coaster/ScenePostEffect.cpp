#include "ScenePostEffect.h"
#include "TrainView.h"
ScenePostEffect::ScenePostEffect(TrainView * Tp)
{
	this->Tp = Tp;
}


void ScenePostEffect::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void ScenePostEffect::Paint()
{
	if (preShader != Tp->posteffect) {
		preShader = Tp->posteffect;
		InitShader(vs_fs[preShader][0].c_str() , vs_fs[preShader][1].c_str());
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glBindFramebuffer(GL_FRAMEBUFFER, FramBufferName);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Tp->paintBeforeEffect();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Set up the view port
	glViewport(0, 0,Tp->width(), Tp->height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, 0.3f, 0);

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Tp->setProjection();		// put the code to set up matrices here

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	// Bind the buffer so that it is the current active buffer
	cvbo.bind();
	// Enable Attribute 1
	shaderProgram->enableAttributeArray(1);
	// Set Attribute 0 to be color
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
	//unbind buffer
	//use render texture
	//Active Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	shaderProgram->setUniformValue("sceneTex", 0);


	shaderProgram->setUniformValue("rt_w", Tp->width());
	shaderProgram->setUniformValue("rt_h", Tp->height());
	shaderProgram->setUniformValue("vx_offset",(GLfloat) 0.5);

	//Draw a ScenePostEffect with 3 indices starting from the 0th index
	glDrawArrays(GL_QUADS, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao.release();
	//unbind vao
	shaderProgram->release();
}

void ScenePostEffect::Init()
{
	initializeOpenGLFunctions();
	preShader = Tp->posteffect;
	InitShader(vs_fs[preShader][0].c_str(), vs_fs[preShader][1].c_str());
	//InitShader("./Shader/pix.vs", "./Shader/frost.fs");
	
	InitVAO();
	InitVBO();
	InitFramBuffer();
}
void ScenePostEffect::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void ScenePostEffect::InitVBO()
{
	//Set each vertex's position
	vertices << QVector3D(-1.0,-1.0,0.0)
		<< QVector3D(-1.0, 1.0, 0.0)
		<< QVector3D(1.0, 1.0, 0.0)
		<< QVector3D(1.0, -1.0, 0.0);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	//Set each vertex's color
	uvs << QVector2D(0.0f, 0.0f)
		<< QVector2D(0.0f, 1.0f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(1.0f, 0.0f);
	// Create Buffer for color
	cvbo.create();
	// Bind the buffer so that it is the current active buffer.
	cvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	cvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

}
void ScenePostEffect::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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
void ScenePostEffect::InitFramBuffer() {
	glGenFramebuffers(1, &FramBufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramBufferName);

	glGenTextures(1, &renderedTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture 
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	// Give an empty image to OpenGL ( the last "0" ) 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Tp->width(), Tp->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed ! 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	// Set the list of draw buffers. 
	DrawBuffers << GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &DrawBuffers[0]); // "1" is the size of DrawBuffers
	

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Tp->width(), Tp->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}