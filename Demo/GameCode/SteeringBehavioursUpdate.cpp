#include "SteeringBehavioursUpdate.hpp"
#include "Utils/Timing/sge_fps_limiter.hpp"
#include "Box2D/Common/b2Math.h"
#include "DemoBot.hpp"
#include "World.hpp"

SteeringBehavioursUpdate::SteeringBehavioursUpdate(std::vector<DemoBot>* objects): Logic(SGE::LogicPriority::Highest), objects(objects)
{}

void SteeringBehavioursUpdate::performLogic()
{
	for(DemoBot& o : *this->objects)
	{
		b2Vec2 heading = o.getVelocity();
		heading.Normalize();
		o.setHeading(heading);
		b2Vec2 sForce = o.getSteering()->CalculateForce();
		sForce.Truncate(o.getMaxForce());
		b2Vec2 acceleration = o.getMassInv() * sForce;
		b2Vec2 velocity = o.getVelocity() + SGE::delta_time * acceleration;
		velocity.Truncate(o.getMaxSpeed());
		o.setVelocity(velocity);
		auto oldPos = o.getPosition();
		o.setPosition(oldPos + SGE::delta_time * o.getVelocity());
		if(o.getVelocity().LengthSquared() > 0.01f)
		{
			velocity.Normalize();
			o.setHeading(velocity);
			o.setSide(velocity.Skew());
		}
		o.getWorld()->UpdateMover(&o, oldPos);
	}
}