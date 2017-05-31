#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "Vector.h"
#include "WaveFrontOBJ.h"

using namespace std;

class particle
{
public:
	double	mass;
	Vector force;
	Vector	position;
	Vector	velocity;
	Vector	acceleration;
	double	air_friction;

	bool fixed = false;

public:
	particle(void)
	{
	}
	particle(Vector init_pos)
	{
		position = init_pos;
		mass = 1.0;
		air_friction = 0.005;
	}
	particle(Vector init_pos, Vector init_vel)
	{
		position = init_pos;
		velocity = init_vel;
		mass = 1.0;
		air_friction = 0.005;
	}
	~particle(void)
	{
	}
public:
	double	getPosX(void) { return position.getX(); }
	double	getPosY(void) { return position.getY(); }
	double  getPosZ(void) { return position.getZ(); }
	void	integrate(double dt, Vector gravity)
	{
		if (!fixed)
		{
			//air friction
			force = force + ((velocity * (air_friction)) * (-1.0));

			force = force + (gravity * mass);
			acceleration = force / mass;
			velocity = velocity + (acceleration*dt);
			position = position + (velocity*dt);
		}
		force.x = 0.0;
		force.y = 0.0;
		force.z = 0.0;
	}

	void add_force(Vector ext_force)
	{
		force = force + ext_force;
		
	}

};


class mass_spring {
public:
	double		spring_coef;
	double		damping_coef;
	particle	*p1;
	particle	*p2;
	double		initial_length;

public:
	mass_spring() {
		spring_coef = 100.0;
		damping_coef = 1.0;
		p1 = p2 = NULL;
		init();
	}
	mass_spring(particle *p1, particle *p2)
	{
		spring_coef = 100.0;
		damping_coef = 12.0;
		this->p1 = p1;
		this->p2 = p2;
		init();
	}

public:
	void init() {

		Vector dp = (p2->position - p1->position);

		initial_length = dp.Magnitude();
	}
	void integrate(double dt, Vector gravity)
	{
		Vector dp = p2->position - p1->position;
		double dx = dp.Magnitude() - initial_length;
		Vector dv = p2->velocity - p1->velocity;

		Vector hook_force = dp * (dx * spring_coef);
		Vector damping_force = dv * damping_coef;

		p2->add_force(damping_force * -1);
		p2->add_force(hook_force * -1);

		p1->add_force(damping_force);
		p1->add_force(hook_force);

		p2->integrate(dt, gravity);
		p1->integrate(dt, gravity);
	}
	void internal_force(double dt)
	{
		Vector dp = p2->position - p1->position;
		double dx = dp.Magnitude() - initial_length;
		Vector dv = p2->velocity - p1->velocity;

		Vector hook_force = dp * (dx * spring_coef);
		Vector damping_force = dv * damping_coef;

		p2->add_force(damping_force * -1);
		p2->add_force(hook_force * -1);

		p1->add_force(damping_force);
		p1->add_force(hook_force);
	}
	void add_force(Vector ext_force)
	{
		p1->force += ext_force;
		p2->force += ext_force;
	}

};

class edge {
public:
	int p1, p2;
	edge(int _p1, int _p2)
	{
		p1 = _p1;
		p2 = _p2;
		if (p1 > p2)
		{
			int t = p1;
			p1 = p2;
			p2 = t;
		}
	}

	bool operator <(edge &e)
	{
		if (p1 - e.p1) return p1 < e.p1;
		if (p2 - e.p2) return p2 < e.p2;
		return false;
	}
	bool operator ==(edge &e)
	{
		return p1 == e.p1 && p2 == e.p2;
	}
};

class mass_cloth {
public:
	std::vector<particle *> p;
	std::vector<mass_spring *> e;

	int grid_n, grid_m, grid_l;
	double dx, dy, dz;
	double shear_coef;
	double dist_coef;
	double bending_coef;
	int iteration_n;


	double		reflect_coef;
	double		badak = -0.2;
	mass_cloth()
	{
		grid_n = 20;
		grid_m = 20;
		grid_l = 20;
		dx = 0.1;
		dy = 0.1;
		dz = 0.1;

		shear_coef = 100.0;
		dist_coef = 100.0;
		bending_coef = 1.0;

		iteration_n = 10;

		reflect_coef = 0.8;
	}
	~mass_cloth()
	{
		for (auto &ep : p)delete ep;
		for (auto &ep : e)delete ep;
		p.clear();
		e.clear();
	}
	void init(WaveFrontOBJ* ctn)
	{
		int i, j, k;
		// init particle
		for (i = 0; i < ctn->verts.size(); i++) {
			particle *tmp = new particle(ctn->verts[i].pos);
			if (i < 20) {
				tmp->fixed = true;
			}
			p.push_back(tmp);
			
		}
		// init_spring
		vector<edge> ed;
		for (i = 0; i < ctn->faces.size(); i++) {
			j = ctn->faces[i].vIndexStart;
			ed.push_back(edge(ctn->vIndex[j], ctn->vIndex[j + 1]));
			ed.push_back(edge(ctn->vIndex[j + 1], ctn->vIndex[j + 2]));
			ed.push_back(edge(ctn->vIndex[j + 2], ctn->vIndex[j]));
		}
		sort(ed.begin(), ed.end());
		ed.erase(unique(ed.begin(), ed.end()), ed.end());

		for (int i = 0; i < ed.size(); i++)
		{
			mass_spring *sp = new mass_spring(p[ed[i].p1], p[ed[i].p2]);
			sp->spring_coef = dist_coef;
			e.push_back(sp);
			
		}

	}
	void sync(WaveFrontOBJ* ctn) {
		int i;
		// init particle
		for (i = 0; i < ctn->verts.size(); i++) {
			ctn->verts[i].pos = p[i]->position;
		}
		//printf("%f, %f, %f", ctn->verts[31].pos.x, ctn->verts[31].pos.y, ctn->verts[31].pos.z);
		//printf("%f, %f, %f\n", p[31]->position.x, p[31]->position.y, p[31]->position.z);
	}
	void add_force(Vector ext_force)
	{
		for (int i = 0; i < p.size(); i++)
		{
			p[i]->add_force(ext_force);
		}
	}

	void integrate(double dt, Vector gravity)
	{

		for (int i = 0; i < p.size(); i++)
		{
			p[i]->add_force(gravity);
		}
		for (int iter = 0; iter < iteration_n; iter++)
		{
			for (int i = 0; i < e.size(); i++)
			{
				e[i]->internal_force(dt);
				
			}
			for (int i = 0; i < p.size(); i++)
			{
				p[i]->integrate(dt, Vector(0, 0, 0));
			}
		}
		collisionCheck();
	}

	void collisionCheck()
	{
		for (int i = 0; i < p.size(); i++)
		{
			if (p[i]->position.y < badak)
			{
				p[i]->position.y = badak;
				p[i]->velocity.y *= -reflect_coef;
			}
		}
	}

};
