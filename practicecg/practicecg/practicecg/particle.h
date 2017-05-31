#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "Vector.h"

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
			force += ((velocity * (air_friction)) * (-1));

			force += gravity * mass;
			acceleration = force / mass;
			velocity += acceleration*dt;
			position += velocity*dt;
		}
		force.x = force.y = force.z = 0.0;
	}

	void add_force(Vector ext_force)
	{
		force += ext_force;
	}

	void draw();
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


	void draw();

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

	int grid_n, grid_m;
	double dx, dy;
	double shear_coef;
	double dist_coef;
	double bending_coef;
	int iteration_n;


	double		reflect_coef;
	double		badak = -2.0;
	mass_cloth()
	{
		grid_n = 20;
		grid_m = 20;
		dx = 0.1;
		dy = 0.1;

		shear_coef = 100.0;
		dist_coef = 100.0;
		bending_coef = 1.0;

		iteration_n = 10;

		reflect_coef = 0.8;
	}
	~mass_cloth()
	{
		//asdfasdf
		for (auto &ep : p)delete ep;
		for (auto &ep : e)delete ep;
		p.clear();
		e.clear();
	}
	void init()
	{
		int i, j, k;
		// init particle
		for (i = 0; i <= grid_n; i++)
			for (j = 0; j <= grid_m; j++)
			{
				particle *xp = new particle(Vector(i*dx - dx*grid_n / 2, j*dy - dy*grid_m / 2));
				if (j == grid_m)
					xp->fixed = true;
				p.push_back(xp);
			}

		// init_spring
		for (i = 0; i <= grid_n; i++)
		{
			for (j = 0; j <= grid_m; j++)
			{
				int pid, qid, rid, sid;
				pid = (i + 0)*(grid_m + 1) + (j + 0);
				qid = (i + 0)*(grid_m + 1) + (j + 1);
				rid = (i + 1)*(grid_m + 1) + (j + 0);
				sid = (i + 1)*(grid_m + 1) + (j + 1);
				if (j + 1 <= grid_m)
				{
					mass_spring *sp = new mass_spring(p[pid], p[qid]);
					sp->spring_coef = dist_coef;
					e.push_back(sp);
				}
				if (i + 1 <= grid_n)
				{
					mass_spring *sp = new mass_spring(p[pid], p[rid]);
					sp->spring_coef = dist_coef;
					e.push_back(sp);
				}
				if (i + 1 <= grid_n && j + 1 <= grid_m)
				{
					mass_spring *sp = new mass_spring(p[pid], p[sid]);
					sp->spring_coef = shear_coef;
					e.push_back(sp);
					sp = new mass_spring(p[qid], p[rid]);
					sp->spring_coef = shear_coef;
					e.push_back(sp);
				}
			}
		}
	}

	void init_by_file(char *filename)
	{
		FILE *fp = fopen(filename, "rt");
		if (fp == NULL)
		{
			printf("hell world!\n");
			return;
		}

		using namespace std;
		vector<Vector> v;
		vector<edge> ed;
		char line[257];
		char command[10];
		while (fgets(line, 256, fp))
		{
			command[0] = 0;
			sscanf(line, "%s", command);
			if (strcmp(command, "v") == 0)
			{
				double x, y, z;
				sscanf(line, "v %lf %lf %lf", &x, &y, &z);
				v.push_back(Vector(x, y));
			}
			else if (strcmp(command, "f") == 0)
			{
				int i, j, k;
				sscanf(line, "f %d %d %d", &i, &j, &k);
				i--; j--; k--;
				ed.push_back(edge(i, j));
				ed.push_back(edge(k, j));
				ed.push_back(edge(i, k));
			}
		}

		sort(ed.begin(), ed.end());
		ed.erase(unique(ed.begin(), ed.end()), ed.end());

		// inil_particle
		for (int i = 0; i < v.size(); i++)
		{
			particle *xp = new particle(v[i]);
			if (i < 20) xp->fixed = true;
			p.push_back(xp);
		}
		for (int i = 0; i < ed.size(); i++)
		{
			mass_spring *sp = new mass_spring(p[ed[i].p1], p[ed[i].p2]);
			sp->spring_coef = dist_coef;
			e.push_back(sp);
		}

		fclose(fp);
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
				auto ep = e[i];
				ep->internal_force(dt);
			}
			for (int i = 0; i < p.size(); i++)
			{
				auto ep = p[i];
				ep->integrate(dt, Vector(0, 0));
			}
		}
		collisionCheck();
	}

	void collisionCheck()
	{
		for (auto &ep : p)
		{
			if (ep->position.y < badak)
			{
				ep->position.y = badak;
				ep->velocity.y *= -reflect_coef;
			}
		}
	}

	void draw();
};