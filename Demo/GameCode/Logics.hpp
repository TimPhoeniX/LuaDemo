#ifndef ZOMBIEGAME_LOGICS
#define ZOMBIEGAME_LOGICS

#include <Logic/sge_logic.hpp>
#include <IO/Key/sge_key.hpp>
#include <Object/Camera2d/sge_camera2d.hpp>
#include <IO/Mouse/sge_mouse.hpp>
#include <Utils/Timing/sge_fps_limiter.hpp>
#include <vector>
#include <random>

#include "DemoBot.hpp"
#include "Objects.hpp"
#include "World.hpp"

namespace SGE
{
	class WorldElement;
}
class DemoGameState;
class DemoBot;
class Player;
class World;

namespace SGE
{
	class Scene;
}

class MoveAwayFromObstacle : public SGE::Logic
{
protected:
	World* world;
	std::vector<SGE::Object*> obstacles;
	std::vector<DemoBot*> movers;
public:
	MoveAwayFromObstacle(World* const world, const std::vector<SGE::Object*>& obstacles);

	void performLogic() override;
};

class SeparateBots : public SGE::Logic
{
protected:
	World* world;
	std::vector<DemoBot>* movers;
	std::vector<DemoBot*> colliding;
public:

	SeparateBots(World* const world, std::vector<DemoBot>* const movers);

	void performLogic() override;
};

class MoveAwayFromWall : public SGE::Logic
{
protected:
	World* world;
	std::vector<DemoBot>& movers;
public:

	MoveAwayFromWall(World* const world, std::vector<DemoBot>& movers)
		: Logic(SGE::LogicPriority::Highest), world(world), movers(movers)
	{}

	void CollideWithWall(DemoBot& mo) const;
	void performLogic() override;
};

class SpectatorCamera: public SGE::Logic
{
	const float speed = 0;
	const SGE::Key up, down, left, right;
	SGE::Camera2d* cam = nullptr;

public:
	SpectatorCamera(const float specamed, const SGE::Key up, const SGE::Key down, const SGE::Key left, const SGE::Key right, SGE::Camera2d* cam);

	~SpectatorCamera() = default;

	void performLogic() override;
};

class Timer : public SGE::Logic
{
	float time = .0f;
	SGE::Action* action = nullptr;
public:
	Timer(float time, SGE::Action* action);
	void performLogic() override;
};

class OnKey : public SGE::Logic
{
	SGE::Key key;
	SGE::Scene* scene = nullptr;
public:
	OnKey(SGE::Key key, SGE::Scene* scene);
	void performLogic() override;
};

namespace SGE
{
	class Scene;
}

class RocketLogic: public SGE::Logic
{
protected:
	DemoGameState* gs;
	World* world;
public:
	RocketLogic(DemoGameState* gs, World* w);

	void performLogic() override;
};

class DemoGameState;

class BotLogic: public SGE::Logic
{
protected:
	World* world;
	DemoGameState* gs;

	void updateEnemies(DemoBot& bot);
	void updateItems(DemoBot& bot);
	void updateState(DemoBot& bot);
	void pickItems(DemoBot& bot);
	void ResetBot(DemoBot& bot);
	void updateBotState(DemoBot& bot);
	void FireRG(DemoBot& bot);
	void FireRL(DemoBot& bot);
	void UpdateEnemy(DemoBot& bot);
	void GetItem(DemoBot& bot, Item::IType type);
	void updateBot(DemoBot& bot);
	
	std::function<float(std::uniform_real_distribution<float>&)> randAngle;
public:
	using SpreadDistribution = std::uniform_real_distribution<float>;
	BotLogic(World* world, DemoGameState* gs)
		: Logic(SGE::LogicPriority::Highest), world(world), gs(gs)
	{
		std::default_random_engine dre{ std::random_device{}() };
		randAngle = [dre](std::uniform_real_distribution<float>& dist)->float
		{
			return 0.f;
		};
	}

	void performLogic() override;
};

class ItemLogic: public SGE::Logic
{
protected:
	World* world;
	DemoGameState* gs;
public:
	ItemLogic(World* world, DemoGameState* gs)
		: Logic(SGE::LogicPriority::High), world(world), gs(gs){}
	
	void performLogic() override;
};

#endif
