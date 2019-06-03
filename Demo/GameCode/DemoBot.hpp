#pragma once
#include <Object/sge_object.hpp>
#include <set>
#include "SteeringBehaviours.hpp"

class World;
class DemoBot;
class Item;

enum class BotState: char
{
	Wandering,
	Attacking,
	Running,
	GettingAmmo,
	GettingHealth,
	GettingArmor
};

enum class CurrentWeapon : char
{
	Railgun,
	Launcher
};

class DemoBot: public SGE::Object
{
public:
	static float RailgunReload;
	static float LauncherReload;
	static float RailgunDamage;
	static float LauncherDamage;
	static float RailgunSpread;
	static float LauncherSpread;
	static float RailgunRate;
	static float LauncherRate;
	static float RocketSpeed;
	static unsigned RailgunDefaultAmmo;
	static unsigned LauncherDefaultAmmo;
	static float DefaultHealth;
	static float DefaultArmor;
	static float maxSpeed;
	static float maxForce;
	static float SwapSpeed;

protected:
	b2Vec2 velocity = b2Vec2_zero;
	b2Vec2 heading = b2Vec2_zero;
	b2Vec2 side = b2Vec2_zero;
	float mass = 1.f;
	float massInv = 1.f;
	float maxTurnRate = 90.f;
	float health = DefaultHealth;
	float armor = DefaultArmor;
	float rgCD = RailgunReload;
	float rlCD = LauncherReload;
	float rgRate = -1.f;
	float rlRate = -1.f;
	float swapping = -1.f;
	unsigned rgLoadedAmmo = RailgunDefaultAmmo;
	unsigned rlLoadedAmmo = LauncherDefaultAmmo;
	unsigned rgSpareAmmo = 0u;
	unsigned rlSpareAmmo = 0u;
	bool hit = false;
	World* world = nullptr;
	SteeringBehaviours* steering = new DemoSteering(this);
	BotState state = BotState::Wandering;
	CurrentWeapon cw = CurrentWeapon::Railgun;
public:
	std::set<DemoBot*> enemies;
	std::set<Item*> items;
	SGE::Object* RailgunTrace = nullptr;

	DemoBot(const b2Vec2& position, SGE::Shape* shape, World* world, const b2Vec2& heading = b2Vec2{1.f,0.f})
		: Object(position, true, shape), heading(heading), side(heading.Skew()), world(world)
	{
		this->orientation = heading.Orientation();
	}

	b2Vec2 getVelocity() const
	{
		return velocity;
	}

	void setVelocity(b2Vec2 velocity)
	{
		this->velocity = velocity;
	}

	b2Vec2 getHeading() const
	{
		return heading;
	}

	void setHeading(b2Vec2 heading)
	{
		this->heading = heading;
		this->orientation = heading.Orientation();
	}

	b2Vec2 getSide() const
	{
		return side;
	}

	void setSide(b2Vec2 side)
	{
		this->side = side;
	}

	float getMass() const
	{
		return mass;
	}

	void setMass(float mass)
	{
		this->mass = mass;
		this->massInv = 1.f / mass;
	}

	float getMassInv() const
	{
		return massInv;
	}

	float getMaxSpeed() const
	{
		return maxSpeed;
	}

	void setMaxSpeed(float maxSpeed)
	{
		this->maxSpeed = maxSpeed;
	}

	float getMaxForce() const
	{
		return maxForce;
	}

	void setMaxForce(float maxForce)
	{
		this->maxForce = maxForce;
	}

	float getMaxTurnRate() const
	{
		return maxTurnRate;
	}

	void setMaxTurnRate(float maxTurnRate)
	{
		this->maxTurnRate = maxTurnRate;
	}

	World* getWorld() const
	{
		return world;
	}

	SteeringBehaviours* getSteering() const
	{
		return steering;
	}

	void setSteering(SteeringBehaviours* steering)
	{
		this->steering = steering;
	}

	float getSpeed() const
	{
		return this->velocity.Length();
	}

	bool IsAttacking() const
	{
		return this->state == BotState::Attacking;
	}

	bool IsRunning() const
	{
		return this->state == BotState::Running;
	}

	bool IsWandering() const
	{
		return this->state == BotState::Wandering;
	}

	void setState(BotState state)
	{
		this->steering->ClearPath();
		this->state = state;
	}
	
	float Health() const
	{
		return this->health;
	}

	float Armor() const
	{
		return this->armor;
	}

	void AddHealth(float h)
	{
		this->health += h;
	}

	void AddArmor(float r)
	{
		if(armor < 0.f)
			armor = 0.f;
		this->armor += r;
	}

	void Damage(float d)
	{
		this->hit = true;
		if(armor < 0.f)
		{
			this->health -= d;
		}
		else
		{
			this->armor -= d;
		}
	}

	bool Hit() const
	{
		return this->hit;
	}

	void ClearHit()
	{
		this->hit = false;
	}

	unsigned RGAmmo() const
	{
		return this->rgLoadedAmmo;
	}

	unsigned RLAmmo() const
	{
		return this->rlLoadedAmmo;
	}

	unsigned RGSpareAmmo() const
	{
		return this->rgSpareAmmo;
	}

	unsigned RLSpareAmmo() const
	{
		return this->rlSpareAmmo;
	}

	float RLCD() const
	{
		return this->rlCD;
	}

	float RGCD() const
	{
		return this->rgCD;
	}

	void AddRailgunAmmo(unsigned i)
	{
		this->rgSpareAmmo += i;
		if (this->rgLoadedAmmo == 0u)
		{
			this->ReloadRG();
		}
	}

	void AddLauncherAmmo(unsigned i)
	{
		this->rlSpareAmmo += i;
		if (this->rlLoadedAmmo == 0u)
		{
			this->ReloadRL();
		}
	}

	BotState getState() const
	{
		return this->state;
	}

	bool IsRGReady() const
	{
		return this->cw == CurrentWeapon::Railgun && this->rgCD < 0.f && this->rgRate < 0.f;
	}

	bool IsRLReady() const
	{
		return this->cw == CurrentWeapon::Launcher && this->rlCD < 0.f && this->rgRate < 0.f;
	}

	bool IsFollowingPath() const
	{
		return !this->getSteering()->getPath().Finished();
	}
	
	void Reloading(float delta)
	{
		if(this->rgCD > 0.f) this->rgCD -= delta;
		if(this->rlCD > 0.f) this->rlCD -= delta;
		if(this->rgRate > 0.f) this->rgRate -= delta;
		if(this->rlRate > 0.f) this->rlRate -= delta;
		if(this->rgCD < (RailgunReload - 0.5f))
			this->RailgunTrace->setVisible(false);
	}

	void ReloadRG()
	{
		this->rgCD = RailgunReload;
		unsigned ammo = std::min(RailgunDefaultAmmo, this->rgSpareAmmo);
		this->rgLoadedAmmo = ammo;
		this->rgSpareAmmo -= ammo;
	}

	bool FireRG()
	{
		if(this->rgRate < 0.f && this->rgCD < 0.f && this->rgLoadedAmmo > 0u)
		{
			if(this->rgLoadedAmmo == 0u)
				throw std::runtime_error("Bot has no Railgun Ammo!");
			this->rgLoadedAmmo -= 1u;
			if (this->rgLoadedAmmo == 0u)
			{
				this->ReloadRG();
			}
			this->rlRate = LauncherRate;
			return true;
		}
		return false;
	}

	void ReloadRL()
	{
		this->rlCD = LauncherReload;
		unsigned ammo = std::min(LauncherDefaultAmmo, this->rlSpareAmmo);
		this->rlLoadedAmmo = ammo;
		this->rlSpareAmmo -= ammo;
	}

	bool FireRL()
	{
		if(this->rlRate < 0.f && this->rlCD < 0.f && this->rlLoadedAmmo > 0u)
		{
			if(this->rlLoadedAmmo == 0u)
				throw std::runtime_error("Bot has no RocketLauncher Ammo!");
			this->rlLoadedAmmo -= 1u;
			if (this->rlLoadedAmmo == 0u)
			{
				this->ReloadRL();
			}
			this->rlRate = LauncherRate;
			return true;
		}
		return false;
	}

	void SwapWeapon()
	{
		if (this->cw == CurrentWeapon::Railgun)
		{
			this->cw = CurrentWeapon::Launcher;
			this->swapping == SwapSpeed;
		}
		else
		{
			this->cw = CurrentWeapon::Railgun;
			this->swapping == SwapSpeed;
		}
	}

	bool IsDead() const
	{
		return this->health < 0.f;
	}

	void Respawn(b2Vec2 position)
	{
		this->setPosition(position);
		this->setState(BotState::Wandering);
		this->health = DefaultHealth;
		this->armor = DefaultArmor;
		this->rgLoadedAmmo = RailgunDefaultAmmo;
		this->rlLoadedAmmo = LauncherDefaultAmmo;
		this->rlSpareAmmo = RailgunDefaultAmmo;
		this->rgSpareAmmo = LauncherDefaultAmmo;
		this->rgCD = -1.f;
		this->rlCD = -1.f;
		this->rlRate = -1.f;
		this->rgRate = -1.f;
		this->swapping = -1.f;
		this->steering->ClearPath();
		this->steering->setEnemy(nullptr);
		this->enemies.clear();
		this->items.clear();
	}

	bool IsReloading() const
	{
		return this->swapping < 0.f || (this->rgCD > 0.f && this->rlCD > 0.f);
	}
};
