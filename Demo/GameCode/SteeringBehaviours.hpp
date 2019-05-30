#pragma once
#include "Box2D/Common/b2Math.h"
#include <array>
#include <vector>
#include "Path.hpp"
#include "Wall.hpp"
#include <vector>

namespace SGE
{
	class Object;
}
class DemoBot;

enum class Deceleration:char
{
	slow = 3,
	normal = 2,
	fast = 1
};

class SteeringBehaviours
{
protected:
	std::array<b2Vec2, 3> feelers = {b2Vec2_zero, b2Vec2_zero, b2Vec2_zero};
	DemoBot* owner = nullptr;
	const DemoBot* enemy = nullptr;
	const SGE::Object* obstacle = nullptr;
	std::vector<DemoBot*> neighbours;
	b2Vec2 wTarget = b2Vec2_zero;
	Path path;
	float wRadius = 2.5f;
	float wDistance = 4.f;
	float wJitter = 15.f;
	float boxLength = 8.f;
	float32 WSDsq = 0.8f;
	float total_space_time = 0.0f;
	float alone_time = 0.0f;
	void CreateFeelers();
	b2Vec2 WallAvoidanceImp(std::vector<std::pair<SGE::Object*, Edge>>& walls);
	static b2Vec2 GetHidingSpot(const b2Vec2& obPos, float obRadius, b2Vec2 targetPos);

public:
	SteeringBehaviours(DemoBot* owner);
	virtual ~SteeringBehaviours();
	virtual b2Vec2 CalculateForce();
	b2Vec2 Seek(b2Vec2 target) const;
	b2Vec2 Flee(b2Vec2 target) const;
	b2Vec2 Arrive(b2Vec2 target, Deceleration dec) const;
	b2Vec2 Pursuit(const DemoBot* evader) const;
	b2Vec2 Evade(const DemoBot* pursuer) const;
	b2Vec2 Wander();
	b2Vec2 ObstacleAvoidance();
	b2Vec2 WallAvoidance();
	b2Vec2 Interpose(const DemoBot*const aA, const DemoBot*const aB) const;
	b2Vec2 Hide(const DemoBot*const target, bool runaway = false,const SGE::Object** = nullptr) const;
	b2Vec2 FollowPath();
	b2Vec2 OffsetPursuit(const DemoBot*const leader, b2Vec2 offset) const;
	b2Vec2 Separation(const std::vector<DemoBot*>& neighbours) const;
	b2Vec2 Alignment(const std::vector<DemoBot*>& neighbours) const;
	b2Vec2 Cohesion(const std::vector<DemoBot*>& neighbours) const;

	void NewPath(Path&& path)
	{
		this->path = std::move(path);
	}

	void NewPath(const Path& path)
	{
		this->path = path;
	}

	void ClearPath()
	{
		this->path.Clear();
	}

	void setEnemy(const DemoBot* const enemy)
	{
		this->enemy = enemy;
	}

	const DemoBot* getEnemy() const
	{
		return this->enemy;
	}

	std::vector<DemoBot*>& getNeighbours()
	{
		return this->neighbours;
	}

	const Path& getPath() const
	{
		return this->path;
	}
};

class DemoSteering: public SteeringBehaviours
{
public:
	using SteeringBehaviours::SteeringBehaviours;
	virtual ~DemoSteering() override = default;
	virtual b2Vec2 CalculateForce() override;

};

