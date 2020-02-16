#include "Controller.h"
#include <cmath>
#include <iostream>
namespace mrsd
{
	void Controller::control(const mrsd::Game& g, float t)
	{
        std::vector<Player*> ps = g.getPlayers();
        //for (std::vector<Player*>::iterator it = ps.begin(); it != ps.end(); it++)
        if (p!=0)
        {
            std::vector<bool> safeSpots(g.getWidth()+1, true);
            determineSafeSpots(g, safeSpots);
            int safeSpot = pickSafeSpot(g, safeSpots, p);
            if (std::abs(safeSpot - p->x) < g.playerSpeed) p -> x = safeSpot;
            else p -> x += (safeSpot > (p->x)) ? g.playerSpeed : (-g.playerSpeed);
        }
	}

	void Controller::createPlayer(Game& g)
	{
		if(p == 0)
		{
			p = new Player();
			p->dead = true;
		}
		if(p->dead)
		{
			p->dead = false;
			p->x = g.getWidth()/2;
			g.newPlayer(p);
		}
	}

	Prediction Controller::trackProjectile(const Projectile& p, const Game& g)
	{
		Prediction pred; // (t, x), where t is global time
        pred.t = (-p.vy - sqrt(pow(p.vy, 2) - 2*g.getGravity()*p.y)) / g.getGravity();
        pred.x = p.x + p.vx * pred.t;
		return pred;
	}

	void Controller::determineSafeSpots(const Game& g, std::vector<bool>& safeSpots)
	{
        std::list<Projectile> pr = g.getProjectiles();
        std::list<Projectile>::iterator it1;
        for (it1 = pr.begin(); it1 != pr.end(); it1++)
        {
            Prediction pred = trackProjectile(*it1, g);
            if (pred.t > 50 * g.getTimeStep()) continue;
            for (int i = std::floor(pred.x - g.explosionSize);
                     i <= std::ceil(pred.x + g.explosionSize);
                     ++i)
            {
                if (i >= 0 && i <= g.getWidth())
                    safeSpots[i] = false;
            }
        }
        
        std::list<Explosion> e = g.getExplosions();
        std::list<Explosion>::iterator it;
        for (it = e.begin(); it != e.end(); it++)
        {
            for (int i = std::floor(it->x - g.explosionSize);
                     i <= std::ceil(it->x + g.explosionSize);
                     ++i)
            {
                if (i >= 0 && i <= g.getWidth())
                    safeSpots[i] = false;
            }
        }
	}

	int Controller::pickSafeSpot(const Game& g, std::vector<bool>& safeSpots, Player* p)
	{
        int left = std::floor(p -> x), right = std::ceil(p -> x);
        while(left >= 0 || right <= g.getWidth())
        {
            if (left >= 0) 
            {
                if (safeSpots[left]) 
                    return left;
            }
            if (right <= g.getWidth())
            {
                if (safeSpots[right]) 
                    return right;
            }
            left--;
            right++;
        }
        return int(std::round(p -> x));
    }
}
