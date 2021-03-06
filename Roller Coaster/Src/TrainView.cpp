#include "TrainView.h"  
#include "ScenePostEffect.h"
TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
	
}  
TrainView::~TrainView()  
{}  
void TrainView::initializeGL()
{
	initializeOpenGLFunctions();
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();
	Plane = new plane();
	Plane->Init();
	Water = new water(QVector3D(-50, 2, -50), QVector3D(50,2,50));
	Water->Init();
	model = new Model(QString("./arrow.obj"), 30, Point3d(0, 5, 0));

	Skybox = new skybox();
	Skybox->Init();
	//Initialize texture 
	SPE = new ScenePostEffect(this);
	SPE->Init();
	initializeTexture();
	
}
void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/Tupi.bmp"));

	/*QOpenGLTexture* world[7];
	world[0] = new QOpenGLTexture(QImage("./Textures/right.jpg"));
	world[1] = new QOpenGLTexture(QImage("./Textures/left.jpg"));
	world[2] = new QOpenGLTexture(QImage("./Textures/top.jpg"));
	world[3] = new QOpenGLTexture(QImage("./Textures/buttom.jpg"));
	world[4] = new QOpenGLTexture(QImage("./Textures/back.jpg"));
	world[5] = new QOpenGLTexture(QImage("./Textures/front.jpg"));*/
	Textures.push_back(texture);
	//for (int i = 0; i < 6; i++) World.push_back(world[i]);
}

void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::paintGL()
{
	
	SPE->Paint();
	//this->paintBeforeEffect();
}
void TrainView::paintBeforeEffect() {
	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0, 0, width(), height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, 0.3f, 0);

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

							//######################################################################
							// TODO: 
							// you might want to set the lighting up differently. if you do, 
							// we need to set up the lights AFTER setting up the projection
							//######################################################################
							// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	setupFloor();
	glDisable(GL_LIGHTING);

	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();
	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
	//Get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//Get projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
	triangle->Paint(ProjectionMatrex, ModelViewMatrex);
	Plane->Paint(ProjectionMatrex, ModelViewMatrex);
	Quat qAll = arcball.now *arcball.start;
	qAll = qAll.conjugate();
	//Water->Paint(ProjectionMatrex, ModelViewMatrex, QVector3D(arcball.eyeX+qAll.x, arcball.eyeY + qAll.y, arcball.eyeZ + qAll.z));
	//model->Paint(ProjectionMatrex, ModelViewMatrex);
	//we manage textures by Trainview class, so we modify square's render function
	Skybox->Paint(ProjectionMatrex, ModelViewMatrex);

	square->Begin();
	//Active Texture
	glActiveTexture(GL_TEXTURE0);
	//Bind square's texture
	Textures[0]->bind();
	//pass texture to shader
	square->shaderProgram->setUniformValue("Texture", 0);
	//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
	square->Paint(ProjectionMatrex, ModelViewMatrex);
	square->End();

	drawFloor(200, 10);
	drawStuff();
}
//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}
	else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
		update();
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================

Pnt3f Cubic(const Pnt3f& G1, const Pnt3f& G2, const Pnt3f& G3, const Pnt3f& G4,  float matrix[4][4], const float& t)
{
	float G[3][4] =
	{
		{G1.x, G2.x, G3.x, G4.x},
		{ G1.y, G2.y, G3.y, G4.y },
		{ G1.z, G2.z, G3.z, G4.z },
	};
	float tM[4] = { t * t *t, t * t, t, 1 };
	float GM[3][4] = {0};
	for (int i = 0; i < 3; i++) 
		for (int j = 0; j < 4; j++) 
			for (int k = 0; k < 4; k++) 
				GM[i][j] += G[i][k] * matrix[k][j];
			
	float answer[3] = {0};

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			answer[i] += GM[i][j] * tM[j];

	return Pnt3f(answer[0], answer[1], answer[2]);
}

void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) {
		for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}

	spline_t type_spline = (spline_t)curve;

	float MA[4][4] =
	{
		{ -0.5, 1, -0.5, 0 },
		{ 1.5, -2.5, 0, 1 },
		{ -1.5, 2, 0.5, 0 },
		{ 0.5, -0.5, 0, 0 },
	};
	float MB[4][4] =
	{
		{ -1.0 / 6,	0.5, -0.5, 1.0 / 6 },
		{ 0.5, -1, 0, 2.0 / 3 },
		{ -0.5, 0.5, 0.5, 1.0 / 6 },
		{ 1.0 / 6, 0, 0, 0 }
	};

	float boardLength = 0;

	for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
		// pos
		
		float t_time;
		Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
		Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		Pnt3f cp_pos_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].pos;
		Pnt3f cp_pos_p0 = (i == 0) ? m_pTrack->points[m_pTrack->points.size() - 1].pos : m_pTrack->points[i - 1].pos;
		// orient
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p0 = (i == 0) ? m_pTrack->points[m_pTrack->points.size() - 1].orient : m_pTrack->points[i - 1].orient;
		vector<Pnt3f> board;
		//Pnt3f cross_t;
		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt = cp_pos_p1, orient_t, qt0, qt1, cross_t;

		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			qt0 = qt;
			switch (type_spline) {
			case spline_Linear:
				orient_t = (1 - t) * cp_orient_p1 + t * cp_orient_p2;
				break;
			case spline_CardinalCubic:
				orient_t = Cubic(cp_orient_p0, cp_orient_p1, cp_orient_p2, cp_orient_p3, MA, t);
				break;
			case spline_CubicB_Spline:
				orient_t = Cubic(cp_orient_p0, cp_orient_p1, cp_orient_p2, cp_orient_p3, MB, t);
				break;
			}
			t += percent;
			switch (type_spline) {
			case spline_Linear:
				qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
				break;
			case spline_CardinalCubic:
				qt = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MA, t);
				break;
			case spline_CubicB_Spline:
				qt = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MB, t);
				break;
			}
			qt1 = qt;
			orient_t.normalize();
			cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			cross_t = cross_t * 2.5f;

			glLineWidth(4);
			glBegin(GL_LINES);
			if (!doingShadows) { glColor3ub(32, 32, 64); }

			if (j != 0 || type_spline == 0)
			{
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);

				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
			}
			glEnd();
			
			if (j > 1) {
				boardLength += sqrt(pow(qt1.x - qt0.x, 2) + pow(qt1.y - qt0.y, 2) + pow(qt1.z - qt0.z, 2));
				if (boardLength > 7.0) {
					if (!doingShadows) { glColor3ub(255, 255, 255); }
					Pnt3f boardCross = cross_t * 1.4f;
					boardLength = 0;
					Pnt3f next = qt1 - qt0;
					next.normalize();
					next = next * 0.7;

					glBegin(GL_QUADS);
					glVertex3f(qt0.x + boardCross.x, qt0.y + boardCross.y, qt0.z + boardCross.z);
					glVertex3f(qt0.x + next.x + boardCross.x, qt1.y + boardCross.y, qt1.z + boardCross.z);
					glVertex3f(qt0.x + next.x - boardCross.x, qt1.y - boardCross.y, qt1.z - boardCross.z);
					glVertex3f(qt0.x - boardCross.x, qt0.y - boardCross.y, qt0.z - boardCross.z);
					glEnd();
				}
			}
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

	
	
	drawTrain(doingShadows);




#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif
	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}



void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}

void TrainView::
drawTrain(bool doingShadow) {

	float t = t_time;
	spline_t type_spline = (spline_t)curve;
	t *= m_pTrack->points.size();
	size_t i;
	for (i = 0; t > 1; t -= 1)
		i++;

	float MA[4][4] =
	{
		{ -0.5, 1, -0.5, 0 },
		{ 1.5, -2.5, 0, 1 },
		{ -1.5, 2, 0.5, 0 },
		{ 0.5, -0.5, 0, 0 },
	};
	float MB[4][4] =
	{
		{ -1.0 / 6,	0.5, -0.5, 1.0 / 6 },
		{ 0.5, -1, 0, 2.0 / 3 },
		{ -0.5, 0.5, 0.5, 1.0 / 6 },
		{ 1.0 / 6, 0, 0, 0 }
	};
	//pos
	Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
	Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
	Pnt3f cp_pos_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].pos;
	Pnt3f cp_pos_p0 = (i == 0) ? m_pTrack->points[m_pTrack->points.size() - 1].pos : m_pTrack->points[i - 1].pos;

	// orient
	Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
	Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
	Pnt3f qt, orient_t, qt0;

	switch (type_spline) {
	case spline_Linear:
		// Linear
		qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
		qt0 = (1 - t + 0.01) * cp_pos_p1 + (t - 0.01) * cp_pos_p2;
		//orient_t = (1 - t) * cp_orient_p1 + t * cp_orient_p2;
		break;
	case spline_CardinalCubic:
		qt = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MA, t);
		qt0 = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MA, t - 0.01);
		break;
	case spline_CubicB_Spline:
		qt = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MB, t);
		qt0 = Cubic(cp_pos_p0, cp_pos_p1, cp_pos_p2, cp_pos_p3, MB, t - 0.01);
		break;
	}


	if (!doingShadow) { glColor3ub(255, 255, 255); }	
	float Length = sqrt(pow(qt.x - qt0.x, 2) + pow(qt.y - qt0.y, 2) + pow(qt.z - qt0.z, 2));
	if (this->isrun) {
		this->t_time += (1.0 / m_pTrack->points.size() / (this->DIVIDE_LINE / (0.04 / Length * 0.7)));
		if (this->t_time > 1.0f)
			this->t_time -= 1.0f;
	}


	glBegin(GL_QUADS);
	glVertex3f(qt.x - 5, qt.y, qt.z - 5);
	glVertex3f(qt.x + 5, qt.y, qt.z - 5);
	glVertex3f(qt.x + 5, qt.y+10, qt.z - 5);
	glVertex3f(qt.x - 5, qt.y + 10, qt.z - 5);

	if (!doingShadow) { glColor3ub(0, 255, 255); }
	glVertex3f(qt.x - 5, qt.y , qt.z - 5);
	glVertex3f(qt.x - 5, qt.y + 10, qt.z - 5);
	glVertex3f(qt.x - 5, qt.y + 10, qt.z + 5);
	glVertex3f(qt.x - 5, qt.y , qt.z + 5);

	if (!doingShadow) { glColor3ub(255, 0, 255); }
	glVertex3f(qt.x - 5, qt.y , qt.z - 5);
	glVertex3f(qt.x - 5, qt.y , qt.z + 5);
	glVertex3f(qt.x + 5, qt.y , qt.z + 5);
	glVertex3f(qt.x + 5, qt.y , qt.z - 5);

	if (!doingShadow) { glColor3ub(255, 255, 0); }
	glVertex3f(qt.x - 5, qt.y + 10, qt.z - 5);
	glVertex3f(qt.x - 5, qt.y + 10, qt.z + 5);
	glVertex3f(qt.x + 5, qt.y + 10, qt.z + 5);
	glVertex3f(qt.x + 5, qt.y + 10, qt.z  - 5);

	if (!doingShadow) { glColor3ub(0, 0, 255); }
	glVertex3f(qt.x + 5, qt.y, qt.z - 5);
	glVertex3f(qt.x + 5, qt.y + 10, qt.z - 5);
	glVertex3f(qt.x + 5, qt.y + 10, qt.z + 5);
	glVertex3f(qt.x + 5, qt.y, qt.z + 5);
	
	if (!doingShadow) { glColor3ub(255, 0, 0); }
	glVertex3f(qt.x - 5, qt.y, qt.z + 5);
	glVertex3f(qt.x + 5, qt.y, qt.z + 5);
	glVertex3f(qt.x + 5, qt.y + 10, qt.z + 5);
	glVertex3f(qt.x - 5, qt.y + 10, qt.z + 5);

	glEnd();
	

}