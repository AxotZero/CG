#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QGLWidget> 
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "plane.h"
#include "water.h"
#include "Model.h"
#include "Model2.h"
#include "point3d.h"
#include "skybox.h"
#include <QtCore/QString>
#include <string>
#include "3DSLoader.h"
using namespace std;
class ScenePostEffect;


class AppMain;
class CTrack;

typedef enum {
	spline_Linear = 0,
	spline_CardinalCubic = 1,
	spline_CubicB_Spline = 2
} spline_t;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();
	void paintBeforeEffect();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);
	void drawTrain(bool);
	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();
	void initializeTexture();


public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene
	unsigned int DIVIDE_LINE = 100;
	unsigned int trainNum = 4;
	float trainDistance = 20.0;
	float speed = 10.0;
	float wheelAngle = 0.0;
	int camera;
	int posteffect=0;
	int curve;
	int track;
	bool isrun;
	float t_time = 0;
	Triangle* triangle;
	Square* square;
	plane * Plane;
	water * Water;
	
	skybox * Skybox;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	ScenePostEffect * SPE;
	QVector<QOpenGLTexture*> Textures;

	bool physical = true;
	Model * train;
	Model * building;
	Model * wheel;
	Model2 tower;
	Model2 house;
	Model2 car;
	Model2 city;
	C3DSLoader statue;
};  
#endif // TRAINVIEW_H 