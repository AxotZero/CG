#pragma once
#include <QGLWidget> 
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include <QtCore/QString>
#include "point3d.h"

typedef struct tag_PARTICLE
{
	GLfloat xpos;//(xpos,ypos,zpos)為particle的position
	GLfloat ypos;
	GLfloat zpos;
	GLfloat xspeed;//(xspeed,yspeed,zspeed)為particle的speed 
	GLfloat yspeed;
	GLfloat zspeed;
	GLfloat r;//(r,g,b)為particle的color
	GLfloat g;
	GLfloat b;
	GLfloat life;// particle的壽命 
	GLfloat fade;// particle的衰減速度
	GLfloat size;// particle的大小  
	GLbyte bFire;
	GLbyte nExpl;//哪種particle效果  
	GLbyte bAddParts;// particle是否含有尾巴
	GLfloat AddSpeed;//尾巴粒子的加速度  
	GLfloat AddCount;//尾巴粒子的增加量  
	tag_PARTICLE* pNext;//下一particle 
	tag_PARTICLE* pPrev;//上一particle   
} Particle, *pParticle;

typedef struct fountainParticle {
	Point3d pos;
	Point3d speed;
	Point3d color;
	Point3d normal;
	float life, size, fade;

	fountainParticle();

}fParticle;

typedef struct rainParticle {
	Point3d pos1;
	float speed;
	int size;
	rainParticle();
}rain;

typedef struct shootParticle {
	Point3d pos, pos2;
	Point3d speed;
	Point3d color;
	float life, fade;
	bool reflect , shooting;
	shootParticle();
}shoot;

//Particle
void AddParticle(Particle ex);
void DeleteParticle(pParticle* p);
void DeleteAll(pParticle* Part);
void InitParticle(Particle& ep);
void Explosion1(Particle* par);
void Explosion2(Particle* par);
void Explosion3(Particle* par);
void Explosion4(Particle* par);
void Explosion5(Particle* par);
void Explosion6(Particle* par);
void Explosion7(Particle* par);
void ProcessParticles();
void DrawParticles();
//fParticle
void ProcessfParticles();
void DrawfParticles();
//rain
void ProcessRain();
void DrawRain();
//shoot
void ProcessShooting(bool);
void DrawShooting();