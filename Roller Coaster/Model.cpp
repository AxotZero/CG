#include "model.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>

#include <QtOpenGL/QtOpenGL>

Model::Model(const QString &filePath, int s, Point3d p)
	: m_fileName(QFileInfo(filePath).fileName())
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
		return;

	Point3d boundsMin( 1e9, 1e9, 1e9);
	Point3d boundsMax(-1e9,-1e9,-1e9);

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString input = in.readLine();
		if (input.isEmpty() || input[0] == '#')
			continue;

		QTextStream ts(&input);
		QString id;
		ts >> id;
		if (id == "v") {
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
				boundsMin[i] = qMin(boundsMin[i], p[i]);
				boundsMax[i] = qMax(boundsMax[i], p[i]);
			}
			m_points << p;
		}
		else if (id == "f" || id == "fo") {
			QVarLengthArray<int, 4> p;

			while (!ts.atEnd()) {
				QString vertex;
				ts >> vertex;
				const int vertexIndex = vertex.split('/').value(0).toInt();
				if (vertexIndex)
					p.append(vertexIndex > 0 ? vertexIndex - 1 : m_points.size() + vertexIndex);
			}

			for (int i = 0; i < p.size(); ++i) {
				const int edgeA = p[i];
				const int edgeB = p[(i + 1) % p.size()];

				if (edgeA < edgeB)
					m_edgeIndices << edgeA << edgeB;
			}

			for (int i = 0; i < 3; ++i)
				m_pointIndices << p[i];

			if (p.size() == 4)
				for (int i = 0; i < 3; ++i)
					m_pointIndices << p[(i + 2) % 4];
		}
	}

	const Point3d bounds = boundsMax - boundsMin;
	const qreal scale = s / qMax(bounds.x, qMax(bounds.y, bounds.z));
	for (int i = 0; i < m_points.size(); ++i)
		m_points[i] = (m_points[i] + p - (boundsMin + bounds * 0.5)) * scale;

	m_normals.resize(m_points.size());
	for (int i = 0; i < m_pointIndices.size(); i += 3) {
		const Point3d a = m_points.at(m_pointIndices.at(i));
		const Point3d b = m_points.at(m_pointIndices.at(i+1));
		const Point3d c = m_points.at(m_pointIndices.at(i+2));

		const Point3d normal = cross(b - a, c - a).normalize();

		for (int j = 0; j < 3; ++j)
			m_normals[m_pointIndices.at(i + j)] += normal;
	}

	for (int i = 0; i < m_normals.size(); ++i)
		m_normals[i] = m_normals[i].normalize();


	Init();
}

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

void Model::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];

	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	//pass projection matrix to shader
	shaderProgram->setUniformValue("projection", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("modelview", MV);

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
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	cvbo.release();

	//Draw a plane with 3 indices starting from the 0th index
	//glDrawElements(
	//	GL_TRIANGLES,      // mode
	//	m_points.size(),    // count
	//	GL_UNSIGNED_INT,   // type
	//	(void*)0           // element array buffer offset
	//);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao.release();
	//unbind vao
	shaderProgram->release();
}

void Model::Init()
{
	InitShader("./Shader/BP.vs", "./Shader/BP.fs");
	InitVAO();
	InitVBO();
}
void Model::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Model::InitVBO()
{
	//Set each vertex's position
	/*vertices << QVector3D(5.0f, 2.0f, -10.0f)
		<< QVector3D(5.0f, 2.0f, 10.0f)
		<< QVector3D(15.0f, 2.0f, 0.0f);*/
	// Create Buffer for position


	for (int i = 0; i < m_pointIndices.size(); i++) {
		QVector3D(m_points[m_pointIndices[i]].x, m_points[m_pointIndices[i]].y, m_points[m_pointIndices[i]].z);
			vertices << QVector3D(m_points[m_pointIndices[i]].x, m_points[m_pointIndices[i]].y, m_points[m_pointIndices[i]].z);
	}
	vvbo.create();
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));


	for (int i = 0; i < m_pointIndices.size(); i++) {
		normals << QVector3D(m_normals[m_pointIndices[i]].x, m_normals[m_pointIndices[i]].y, m_normals[m_pointIndices[i]].z);
	}
	//Set each vertex's color
	//colors << QVector3D(0.0f, 1.0f, 0.0f)
	//	<< QVector3D(1.0f, 0.0f, 0.0f)
	//	<< QVector3D(0.0f, 0.0f, 1.0f);
	// Create Buffer for color
	cvbo.create();
	// Bind the buffer so that it is the current active buffer.
	cvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	cvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));

}
void Model::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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