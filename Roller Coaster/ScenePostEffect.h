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

class TrainView;


class ScenePostEffect : protected QOpenGLFunctions_4_3_Core
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D>uvs;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer cvbo;
	GLuint FramBufferName = 0;
	GLuint renderedTexture = 0;
	QVector<GLenum> DrawBuffers;
	TrainView * Tp = nullptr;

public:
	ScenePostEffect(TrainView * Tp);
	void Draw();
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void InitFramBuffer();
	void Paint();
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
};