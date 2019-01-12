#include "Particle.h"

static pParticle Particles;
static UINT nOfFires = 0;
static UINT Tick1 = 0, Tick2 = 0;  //(GetTickCount();)
static float DTick = 0, offY = 1.5;  //(float(Tick2 - Tick1); )
static GLfloat grav = 0.00003f; // (0.00003f;)
static GLuint    textureID;
#define MAX_PARTICLES 1000 
#define MAX_FIRES 3  
#define MAX_RAIN 200
#define MAX_SHOOT 1000
fountainParticle::fountainParticle() {
	float xn = 0.5 - float(rand() % 100) / 100.0,
		zn = 0.5 - float(rand() % 100) / 100.0;
	pos = Point3d(xn, 7.0f, zn);
	speed = Point3d(xn, 1.0f, zn);
	normal = Point3d(float(rand() % 100), float(rand() % 100), float(rand() % 100)).normalize();
	color = Point3d(float(rand() % 100) / 60.0f, float(rand() % 100) / 60.0f, float(rand() % 100) / 60.0f);
	life = 0.3;
	size = float(rand() % 100 / 300.f);
	fade = 0.005f + float(rand() % 21) / 10000.0f;
}

rainParticle::rainParticle() {
	float xn = 100.0 - float(rand() % 200) ,
		zn = 100.0 - float(rand() % 200) ;
	pos1 = Point3d(xn, 100.0f, zn);
	speed = -float(rand() % 30 + 10.0) / 30.0f;
	size = rand() % 15 + 5;
}

shootParticle::shootParticle() {
	float yn = 41.0 - float(rand() % 50) / 25.0f,
		xn = 1.0f - float(rand() % 50 ) /25.0f;
	shooting = reflect =false;
	life = 1.0f;
	fade = 0.01;
	pos = pos2 = Point3d(xn, yn, 70.0);
	color = Point3d(255, 255, 0);
	speed = Point3d(0, 0, -1.0);
}

static fParticle Particles2[MAX_PARTICLES];
static rain Rain[MAX_RAIN];
static shoot shooting[MAX_SHOOT];

void AddParticle(Particle ex)
{
	pParticle p;
	p = new Particle;//new particle   
	p->pNext = NULL;        p->pPrev = NULL;
	p->b = ex.b;  p->g = ex.g;  p->r = ex.r;
	p->fade = ex.fade;
	p->life = ex.life;
	p->size = ex.size;
	p->xpos = ex.xpos;
	p->ypos = ex.ypos;
	p->zpos = ex.zpos;
	p->xspeed = ex.xspeed;
	p->yspeed = ex.yspeed;
	p->zspeed = ex.zspeed;
	p->AddCount = ex.AddCount;
	p->AddSpeed = ex.AddSpeed;
	p->bAddParts = ex.bAddParts;
	p->bFire = ex.bFire;
	p->nExpl = ex.nExpl;
	if (!Particles)//當目前的Particle列表為空時
	{
		Particles = p;
		return;
	}
	else
	{
		Particles->pPrev = p;//插入Particle  
		p->pNext = Particles;
		Particles = p;
	}
}

void DeleteParticle(pParticle* p)
{
	if (!(*p))//假如Particle列表為空
		return;
	if (!(*p)->pNext && !(*p)->pPrev)//假如只有一個Particle，直接删除 
	{
		delete (*p);
		*p = NULL;
		return;
	}
	pParticle tmp;
	if (!(*p)->pPrev)//假如是首節點 
	{
		tmp = (*p);
		*p = (*p)->pNext;
		Particles = *p;
		(*p)->pPrev = NULL;
		delete tmp;
		return;
	}
	if (!(*p)->pNext)//末節點 
	{
		(*p)->pPrev->pNext = NULL;
		delete (*p);
		*p = NULL;
		return;
	}
	else
	{
		//一般情况 
		tmp = (*p);
		(*p)->pPrev->pNext = (*p)->pNext;
		(*p)->pNext->pPrev = (*p)->pPrev;
		*p = (*p)->pNext;
		delete tmp;
	}
}

void DeleteAll(pParticle* Part)
{
	while ((*Part))
		DeleteParticle(Part);
}

void InitParticle(Particle& ep)
{
	ep.b = float(rand() % 100) / 60.0f;//顏色隨機
	ep.g = float(rand() % 100) / 60.0f;
	ep.r = float(rand() % 100) / 60.0f;
	ep.life = 1.0f;//初始壽命
	ep.fade = 0.005f + float(rand() % 21) / 10000.0f;//衰减速度
	ep.size = 1;//大小 
	ep.xpos = 30.0f - float(rand() % 601) / 10.0f;//位置 
	ep.ypos = 15.0f;
	ep.zpos = 20.0f - float(rand() % 401) / 10.0f;
	if (!int(ep.xpos))//x方向速度(z方向相同)
	{
		ep.xspeed = 0.0f;
	}
	else
	{
		if (ep.xpos<0)
		{
			ep.xspeed = (rand() % int(-ep.xpos)) / 1500.0f;
		}
		else
		{
			ep.xspeed = -(rand() % int(-ep.xpos)) / 1500.0f;
		}
	}
	ep.yspeed = 0.04f + float(rand() % 11) / 1000.0f;//y方向速度(向上)
	ep.bFire = 1;
	ep.nExpl = 1 + rand() % 6;//粒子效果 
	ep.bAddParts = 1;//設定有尾巴 
	ep.AddCount = 0.0f;
	ep.AddSpeed = 0.2f;
	nOfFires++;//粒子數+1 
	AddParticle(ep);//加入粒子列表  
}

void Explosion1(Particle* par) {
	Particle ep;
	for (int i = 0; i<100; i++)
	{
		ep.b = float(rand() % 100) / 60.0f;
		ep.g = float(rand() % 100) / 60.0f;
		ep.r = float(rand() % 100) / 60.0f;
		ep.life = 1.0f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.yspeed = 0.01f - float(rand() % 41) / 1000.0f;
		ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.bFire = 0;
		ep.nExpl = 0;
		ep.bAddParts = 0;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.0f;
		AddParticle(ep);
	}

}

void Explosion2(Particle* par) {
	Particle ep;
	for (int i = 0; i<1000; i++)
	{
		ep.b = par->b;
		ep.g = par->g;
		ep.r = par->r;
		ep.life = 1.0f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.yspeed = 0.01f - float(rand() % 41) / 1000.0f;
		ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.bFire = 0;
		ep.nExpl = 0;
		ep.bAddParts = 0;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.0f;
		AddParticle(ep);
	}
}

void Explosion3(Particle* par) {
	Particle ep;
	float PIAsp = 3.1415926 / 180;
	for (int i = 0; i < 30; i++) {
		float angle = float(rand() % 360)*PIAsp;
		ep.b = par->b;
		ep.g = par->g;
		ep.r = par->r;
		ep.life = 1.5f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = (float)sin(angle)*0.01f;
		ep.yspeed = 0.01f + float(rand() % 11) / 1000.0f;
		ep.zspeed = (float)cos(angle)*0.01f;
		ep.bFire = 0;
		ep.nExpl = 0;
		ep.bAddParts = 1;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.2f;
		AddParticle(ep);
	}
}

void Explosion4(Particle* par) {
	Particle ep;
	float PIAsp = 3.1415926 / 180;
	for (int i = 0; i < 30; i++) {
		float angle = float(rand() % 360)*PIAsp;
		ep.b = float(rand() % 100) / 60.0f;
		ep.g = float(rand() % 100) / 60.0f;
		ep.r = float(rand() % 100) / 60.0f;
		ep.life = 1.5f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = (float)sin(angle)*0.01f;
		ep.yspeed = 0.01f + float(rand() % 11) / 1000.0f;
		ep.zspeed = (float)cos(angle)*0.01f;
		ep.bFire = 0;
		ep.nExpl = 0;
		ep.bAddParts = 1;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.2f;
		AddParticle(ep);
	}
}

void Explosion5(Particle* par) {
	Particle ep;
	for (int i = 0; i < 30; i++) {
		ep.b = par->b;
		ep.g = par->g;
		ep.r = par->r;
		ep.life = 0.8f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.yspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.zspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.bFire = 0;
		ep.nExpl = 7;
		ep.bAddParts = 0;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.0f;
		AddParticle(ep);
	}
}

void Explosion6(Particle* par) {
	Particle ep;
	for (int i = 0; i < 100; i++) {
		ep.b = float(rand() % 100) / 60.0f;
		ep.g = float(rand() % 100) / 60.0f;
		ep.r = float(rand() % 100) / 60.0f;
		ep.life = 0.8f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.8f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.yspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.zspeed = 0.01f - float(rand() % 21) / 1000.0f;
		ep.bFire = 0;
		ep.nExpl = 7;
		ep.bAddParts = 0;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.0f;
		AddParticle(ep);
	}
}

void Explosion7(Particle* par) {
	Particle ep;
	for (int i = 0; i < 10; i++) {
		ep.b = par->b;
		ep.g = par->g;
		ep.r = par->r;
		ep.life = 0.5f;
		ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
		ep.size = 0.6f;
		ep.xpos = par->xpos;
		ep.ypos = par->ypos;
		ep.zpos = par->zpos;
		ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.yspeed = 0.01f - float(rand() % 41) / 1000.0f;
		ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
		ep.bFire = 0;
		ep.nExpl = 0;
		ep.bAddParts = 0;
		ep.AddCount = 0.0f;
		ep.AddSpeed = 0.0f;
		AddParticle(ep);
	}
}

void ProcessParticles()
{
	Tick1 = Tick2;
	Tick2 = GetTickCount();
	DTick = float(Tick2 - Tick1);
	DTick *= 0.5f;
	Particle ep;
	if (nOfFires<MAX_FIRES)
	{
		InitParticle(ep);
	}
	pParticle par;
	par = Particles;
	while (par) {
		par->life -= par->fade*(float(DTick)*0.1f);//Particle壽命衰減 
		if (par->life <= 0.05f) {//當壽命小於一定值     
			if (par->nExpl) {//爆炸效果
				switch (par->nExpl) {
				case 1:
					Explosion1(par);
					break;
				case 2:
					Explosion2(par);
					break;
				case 3:
					Explosion3(par);
					break;
				case 4:
					Explosion4(par);
					break;
				case 5:
					Explosion5(par);
					break;
				case 6:
					Explosion6(par);
					break;
				case 7:
					Explosion7(par);
					break;
				default:
					break;
				}
			}
			if (par->bFire)
				nOfFires--;
			DeleteParticle(&par);
		}
		else {
			par->xpos += par->xspeed*DTick;  par->ypos += par->yspeed*DTick;
			par->zpos += par->zspeed*DTick;   par->yspeed -= grav * DTick;
			if (par->bAddParts) {//假如有尾巴 
				par->AddCount += 0.01f*DTick;//AddCount變化愈慢，尾巴粒子愈小 
				if (par->AddCount>par->AddSpeed) {//AddSpeed愈大，尾巴粒子愈小 
					par->AddCount = 0;
					ep.b = par->b; ep.g = par->g; ep.r = par->r;
					ep.life = par->life*0.9f;//壽命變短 
					ep.fade = par->fade*7.0f;//衰减快一些 
					ep.size = 0.6f;//粒子尺寸小一些 
					ep.xpos = par->xpos; ep.ypos = par->ypos; ep.zpos = par->zpos;
					ep.xspeed = 0.0f;  ep.yspeed = 0.0f; ep.zspeed = 0.0f;
					ep.bFire = 0;
					ep.nExpl = 0;
					ep.bAddParts = 0;//尾巴粒子没有尾巴 
					ep.AddCount = 0.0f;
					ep.AddSpeed = 0.0f;
					AddParticle(ep);
				}
			}  par = par->pNext;//更新下一粒子   
		}
	}


}

void DrawParticles() {
	glBindTexture(GL_TEXTURE_2D, textureID);
	pParticle par;
	par = Particles;
	while (par)
	{
		glColor4f(par->r, par->g, par->b, par->life);

		glPointSize(7);
		glBegin(GL_POINTS);
		glVertex3f(par->xpos, par->ypos, par->zpos);
		glEnd();


		/*glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(1, 1);
		glVertex3f(par->xpos + par->size, par->ypos + par->size, par->zpos);
		glTexCoord2d(0, 1);
		glVertex3f(par->xpos - par->size, par->ypos + par->size, par->zpos);
		glTexCoord2d(1, 0);
		glVertex3f(par->xpos + par->size, par->ypos - par->size, par->zpos);
		glTexCoord2d(0, 0);
		glVertex3f(par->xpos - par->size, par->ypos - par->size, par->zpos);
		glEnd();*/
		par = par->pNext;
	}
}

void ProcessfParticles() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		fParticle& p = Particles2[i];
		p.life -= p.fade;
		if (p.life > 0.0f) {
			p.pos = p.pos + p.speed;
			p.speed.y -= 0.05f;
		}
		else {
			p = fParticle();
		}
	}
}

void DrawfParticles() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		fParticle& p = Particles2[i];
		Point3d pos(-80.0, 10.0, 80.0);
		if (p.life > 0.0f) {
			if(p.speed.y < 0)
				glColor3f(255, 255, 255);
			else
				glColor3f(0, 187, 255);

			glPointSize(3);
			glBegin(GL_POINTS);
			glVertex3f(pos.x + p.pos.x, pos.y + p.pos.y, pos.z + p.pos.z);
			glEnd();
			/*glBegin(GL_QUADS);
			glVertex3f(pos.x + p.pos.x + p.size + p.normal.x, pos.y + p.pos.y + p.size + p.normal.y, pos.z + p.pos.z + p.size + p.normal.z);
			glVertex3f(pos.x + p.pos.x + p.size + p.normal.x, pos.y + p.pos.y + p.size + p.normal.y, pos.z + p.pos.z - p.size + p.normal.z);
			glVertex3f(pos.x + p.pos.x - p.size + p.normal.x, pos.y + p.pos.y + p.size + p.normal.y, pos.z + p.pos.z - p.size + p.normal.z);
			glVertex3f(pos.x + p.pos.x - p.size + p.normal.x, pos.y + p.pos.y - p.size + p.normal.y,  pos.z + p.pos.z + p.size - p.normal.z);
			glEnd();*/
		}
	}
}

void ProcessRain() {
	for (int i = 0; i < MAX_RAIN; i++) {
		if (!i || Rain[i].pos1.distance(Rain[i - 1].pos1) > 8.0f) {
			rain& r = Rain[i];
			if (!(r.pos1.y < 0.0f)) {
				r.pos1.y += r.speed;
			}
			else
				r = rain();
		}
		else break;
	}
}

void DrawRain() {
	
	for (int i = 0; i < MAX_RAIN; i++) {
		rain& p = Rain[i];
		//Point3d pos(-70.0, 0, -70.0);
		glColor3f(255, 255, 255);
		glBegin(GL_POINTS);
		glVertex3f(p.pos1.x,p.pos1.y, p.pos1.z);
		//glVertex3f(pos.x + p.pos2.x, pos.y + p.pos2.y, pos.z + p.pos2.z);
		glEnd();
	}

}

void ProcessShooting() {
	Point3d destination(0, 39.5, -63);
	for (int i = 0; i < MAX_SHOOT; i++) {
		shoot& r = shooting[i];
		if (!i || r.shooting || shooting[i].pos.distance(shooting[i - 1].pos) > 2.0f) {
			r.shooting = 1;
			if (r.life > 0.0f) {
				if (!r.reflect) {
					if (r.pos.distance(destination) > 8.0f)
						r.pos += r.speed;
					else {
						r.color = Point3d(255, 0, 0);
						r.reflect = 1;
						Point3d N = (r.pos - destination).normalize();
						N.z = - N.z * 0.1;
						r.speed = N.normalize() * 0.2;
						r.speed.normalize();
					}
				}
				else {
					r.life -= r.fade;
					r.pos += r.speed;
				}
			}
			else
				r = shoot();
		}
		else break;
	}
}

void DrawShooting() {
	for (int i = 0; i < MAX_SHOOT; i++) {
		shoot& p = shooting[i];
		if (p.shooting) {
			glColor3f(255, 255, 0);
			if(p.reflect) glColor3f(255, 255, 255);
			glPointSize(5);
			glBegin(GL_POINTS);
			glVertex3f(p.pos.x, p.pos.y, p.pos.z);
			glEnd();
		}
	}
}
