﻿#include <set>
#include <functional>

#include <Logic/Logics/Camera/sge_logic_camera_zoom.hpp>
#include <Object/Shape/sge_shape.hpp>
#include <algorithm>
#include <random>
#include <thread>

#include "DemoScene.hpp"
#include "Image.hpp"
#include "Logics.hpp"
#include "Actions.hpp"
#include "DemoBot.hpp"
#include "Utilities.hpp"
#include "SteeringBehavioursUpdate.hpp"
#include "Game/InputHandler/sge_input_binder.hpp"
#include "Renderer/SpriteBatch/sge_sprite_batch.hpp"
#include "Renderer/sge_renderer.hpp"
#include "QuadBatch.hpp"
#include "QuadObject.hpp"
#include <queue>
#include "Graph.hpp"
#include "Actions.hpp"

#include "sol/sol.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

class RGTrace: public SGE::Object
{
public:
	RGTrace(): Object(b2Vec2_zero, true)
	{
		this->Object::setVisible(false);
	}
	~RGTrace() = default;
};

class Distance
{
public:
	float operator()(const GridVertex* cur, const GridVertex* end) const
	{
		return b2Distance(cur->Label().position, end->Label().position);
	}
};

class DiagonalDistance
{
	const static float sqrt2;
public:
	float operator()(const GridVertex* cur, const GridVertex* end) const
	{
		auto node = cur->Label().position, goal = end->Label().position;
		float dx = abs(node.x - goal.x), dy = abs(node.y - goal.y);
		return dx < dy ? dx * sqrt2 + dy - dx : dy * sqrt2 + dx - dy;
	}
};
const float DiagonalDistance::sqrt2 = sqrt(2.f);


void DemoGameState::InitRandomEngine()
{
	this->rand = std::bind(std::uniform_int_distribution<size_t>(0, graph.VertexCount()-1u), std::default_random_engine{});
}

GridCell* DemoGameState::GetCell(b2Vec2 pos)
{
	size_t x = size_t(std::floor(pos.x)), y = size_t(std::floor(pos.y));
	return &(cells[y][x]);
}

GridVertex* DemoGameState::GetVertex(b2Vec2 pos)
{
	GridVertex* res = this->GetCell(pos)->vertex;
	if(!res)
	{
		b2Vec2 dir = {-0.5f, 0.f};
		int reset = 8;
		while(!res)
		{
			res = this->GetCell(pos + dir)->vertex;
			dir = b2Mul(b2Rot(0.25f * b2_pi), dir);
			if(reset-- == 0)
			{
				reset = 8;
				dir *= 2.f;
			}
		}
	}
	return res;
}
GridVertex* DemoGameState::GetRandomVertex()
{
	GridVertex* res = graph[this->rand()];
	return res;
}

GridVertex* DemoGameState::GetRandomVertex(const b2Vec2& position, const float limit, bool inside = true)
{
	GridVertex* res = graph[this->rand()];
	if(inside)
	{
		while(b2DistanceSquared(position, res->Label().position) > limit * limit)
		{
			res = graph[this->rand()];
		}
	}
	else
	{
		while(b2DistanceSquared(position, res->Label().position) < limit * limit)
		{
			res = graph[this->rand()];
		}
	}
	return res;
}

Path DemoGameState::GetPath(GridVertex * begin, GridVertex * end)
{
	this->graph.AStar(begin, end, DiagonalDistance());
	return Path(begin, end);
}

void DemoGameState::UseItem(Item* item)
{
	for(auto& bot: this->bots)
	{
		bot.items.erase(item);
	}
	this->world->RemoveItem(item);
}

void DemoGameState::NewRocket(b2Vec2 pos, b2Vec2 direction)
{
	direction.Normalize();
	Rocket* rocket = new Rocket(pos + 0.5f * direction, direction);
	this->world->AddRocket(rocket);
	this->rocketBatch->addObject(rocket);
	this->rockets.push_back(rocket);
}

void DemoGameState::AddExplosion(Rocket* rocket)
{
	rocket->setShape(Rocket::ExplosionShape());
	rocket->setLayer(-0.6f);
	this->explosionBatch->addObject(rocket);
	this->explosions.push_back(rocket);
}


void DemoGameState::RemoveRocket(Rocket* rocket)
{
	this->world->RemoveRocket(rocket);
	this->rocketBatch->removeObject(rocket);
	this->rockets.erase(std::find(this->rockets.begin(), this->rockets.end(), rocket));
	this->AddExplosion(rocket);
}

void DemoGameState::RemoveExplosion(Rocket* rocket)
{
	this->explosionBatch->removeObject(rocket);
	this->explosions.erase(std::find(this->explosions.begin(), this->explosions.end(), rocket));
	delete rocket;
}

template <typename T>
void DemoGameState::GenerateItems(const size_t bots, SGE::RealSpriteBatch* batch)
{
	for(size_t i = 0u; i < bots; ++i)
	{
		Item* item = new T(this->GetRandomVertex()->Label().position);
		batch->addObject(item);
		this->items.push_back(item);
	}
}

bool DemoScene::init()
{
	return true;
}

constexpr size_t ObstaclesNum = 12u;

DemoScene::DemoScene(SGE::Game* game, const char* path) : Scene(), world(Width, Height), game(game),
path([game](const char* path)
{
	return game->getGamePath() + path;
}(path))
{
	static bool initialized = init();
}

struct GridCellBuild
{
	size_t x = 0u, y = 0u;
	enum State
	{
		Accepted,
		Queued,
		Untested,
		Invalid
	} state = State::Untested;
	SGE::Object* dummy = nullptr;
	GridVertex* vertex;
};

class GraphCellDummy: public SGE::Object
{
public:
	GraphCellDummy(size_t x, size_t y) : Object(0.5f + x, 0.5f + y, true, getCircle()){}
	GraphCellDummy(b2Vec2 pos) : Object(pos.x, pos.y, true, getCircle()) {}
};

class GraphCellDummy1: public GraphCellDummy
{
public:
	GraphCellDummy1(size_t x, size_t y): GraphCellDummy(x,y)
	{
		this->Object::setShape(SGE::Shape::Circle(0.3f, true));
	}
	GraphCellDummy1(b2Vec2 pos): GraphCellDummy(pos)
	{
		this->Object::setShape(SGE::Shape::Circle(0.3f, true));
	}
};

class GraphEdgeDummy: public SGE::Object
{
public:
	GraphEdgeDummy(size_t x, size_t y): Object(0.5f + x, 0.5f + y, true, getCircle())
	{}
	GraphEdgeDummy(b2Vec2 pos): Object(pos.x, pos.y, true, getCircle())
	{}
};

constexpr size_t Bots = 5u;

class InputLua : public SGE::Action
{
	sol::state lua;
	std::thread thread;
public:
	InputLua() : Action(true)
	{
		this->lua.open_libraries(sol::lib::base, sol::lib::math);
	}

	void runLua()
	{
		std::string script;
		std::getline(std::cin, script);
		if (script.find("load ") == 0)
		{
			auto result = this->lua.script_file(script.substr(5), sol::script_pass_on_error);
			if (!result.valid())
			{
				std::cerr << "Failed to execute: " << script << std::endl;
				sol::error err = result;
				std::cerr << err.what() << std::endl;;
			}
		}
		else
		{
			auto result = this->lua.script(script, sol::script_pass_on_error);
			if (!result.valid())
			{
				std::cerr << "Failed to execute: " << script << std::endl;
				sol::error err = result;
				std::cerr << err.what() << std::endl;;
			}
		}
		SGE::Game::getGame()->raiseWindow();
	}


	virtual void action_begin() override
	{
	}
	virtual void action_main() override
	{
#ifdef _WIN32
		//This works!
		HWND console = GetConsoleWindow();
		BringWindowToTop(console);
		SetActiveWindow(console);
#endif
		if (thread.joinable())
			thread.join();
		this->thread = std::thread(&InputLua::runLua, this);
	}
	virtual void action_ends() override
	{
	}
};

void DemoScene::loadScene()
{
	this->gs = new DemoGameState();
	this->gs->world = &this->world;

	//RenderBatches
	SGE::BatchRenderer* renderer = SGE::Game::getGame()->getRenderer();
	this->level = SGE::Level();
	GLuint basicProgram = renderer->getProgramID("BatchShader.vert", "BatchShader.frag");
	GLuint scaleUVProgram = renderer->getProgramID("BatchUVShader.vert", "BatchShader.frag");
	GLuint QuadProgram = renderer->getProgramID("QuadBatchShader.vert", "BatchShader.frag");

	std::string lightBrickTexPath = "Resources/Textures/light_bricks.png";
	std::string zombieTexPath = "Resources/Textures/zombie.png";
	std::string cellTexPath = "Resources/Textures/cell.png";
	std::string beamPath = "Resources/Textures/pointer.png";
	std::string rocketPath = "Resources/Textures/rocket.png";
	std::string explosionPath = "Resources/Textures/explosion.png";
	std::string healthPath = "Resources/Textures/health.png";
	std::string armorPath = "Resources/Textures/armor.png";
	std::string rlammoPath = "Resources/Textures/rlammo.png";
	std::string rgammoPath = "Resources/Textures/rgammo.png";

	SGE::RealSpriteBatch* wallBatch = renderer->getBatch(renderer->newBatch(scaleUVProgram, lightBrickTexPath, 4, false, true));
	SGE::RealSpriteBatch* obstacleBatch = renderer->getBatch(renderer->newBatch<QuadBatch>(QuadProgram, lightBrickTexPath, 12, false, true));
	SGE::RealSpriteBatch* botBatch = renderer->getBatch(renderer->newBatch(basicProgram, zombieTexPath, Bots));

	this->gs->railBatch = renderer->getBatch(renderer->newBatch(basicProgram, beamPath, Bots));
	this->gs->rocketBatch = renderer->getBatch(renderer->newBatch(basicProgram, rocketPath, Bots * 20u));
	this->gs->explosionBatch = renderer->getBatch(renderer->newBatch(basicProgram, explosionPath, Bots * 20u));

	SGE::RealSpriteBatch* healthBatch = renderer->getBatch(renderer->newBatch(basicProgram, healthPath, Bots));
	SGE::RealSpriteBatch* armorBatch = renderer->getBatch(renderer->newBatch(basicProgram, armorPath, Bots));
	SGE::RealSpriteBatch* rgammoBatch = renderer->getBatch(renderer->newBatch(basicProgram, rgammoPath, Bots));
	SGE::RealSpriteBatch* rlammoBatch = renderer->getBatch(renderer->newBatch(basicProgram, rlammoPath, Bots));

	SGE::RealSpriteBatch* graphTestBatch = renderer->getBatch(renderer->newBatch(basicProgram, cellTexPath, size_t(Width * Height), false, true));
	SGE::RealSpriteBatch* graphEdgeTestBatch = renderer->getBatch(renderer->newBatch(basicProgram, "Resources/Textures/path.png", size_t(Width * Height * 8u), false, true));

	QuadBatch* obBatch = dynamic_cast<QuadBatch*>(obstacleBatch);
	if (!obBatch)
		throw std::runtime_error("QuadBatch cast failed!");

	GLuint IBO = botBatch->initializeIBO();
	GLuint sampler = botBatch->initializeSampler();
	obstacleBatch->initializeIBO(IBO);
	obstacleBatch->initializeSampler(GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

	for (SGE::RealSpriteBatch* b : { wallBatch, this->gs->railBatch, this->gs->rocketBatch, healthBatch, armorBatch, rgammoBatch, rlammoBatch })
	{
		b->initializeIBO(IBO);
		b->initializeSampler(sampler);
	}
	//!RenderBatches

	auto& world = this->level.getWorld();
	world.reserve(4 + ObstaclesNum);

	//Boundaries
	SGE::Shape* horizontal = SGE::Shape::Rectangle(Width, 1.f, false);
	SGE::Shape* vertical = SGE::Shape::Rectangle(1.f, Height + 2.f, false);

	world.emplace_back(-0.5f, Height * 0.5f, lightBrickTexPath);
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Right);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width + .5f, Height * 0.5f, lightBrickTexPath);
	world.back().setShape(vertical);
	this->world.AddWall(&world.back(), Wall::Left);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width * 0.5f, Height + .5f, lightBrickTexPath);
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Bottom);
	wallBatch->addObject(&world.back());

	world.emplace_back(Width * 0.5f, -0.5f, lightBrickTexPath);
	world.back().setShape(horizontal);
	this->world.AddWall(&world.back(), Wall::Top);
	wallBatch->addObject(&world.back());
	//!Boundaries

	//Camera
	SGE::Camera2d* cam = game->getCamera();
	cam->setPosition({ 32.f * Width, 32.f * Height });
	cam->setCameraScale(0.197f);
	this->addLogic(new SpectatorCamera(10, SGE::Key::W, SGE::Key::S, SGE::Key::A, SGE::Key::D, cam));
	this->addLogic(new SGE::Logics::CameraZoom(cam, 0.5f, 1.f, 0.197f, SGE::Key::Q, SGE::Key::E));
	//!Camera

	//Obstacles
	using Quad = QuadBatch::Quad;
	std::uniform_real_distribution<float> angle_distribution(-b2_pi, b2_pi);
	std::mt19937 engine((std::random_device{})());
	auto angle = std::bind(angle_distribution, engine);
	constexpr float RB1 = 24.f; //Region Boundary
	constexpr float RB2 = 10.f; //Region Boundary
	Quad Diamond1 = {64.f * glm::vec2{-20.f, 0.f}, 64.f * glm::vec2{0, -8.f}, 64.f * glm::vec2{20.f, 0.f}, 64.f * glm::vec2{0.f, 8.f } };
	Quad Diamond2 = {64.f * glm::vec2{-8.f, 0.f}, 64.f * glm::vec2{0, -3.f}, 64.f * glm::vec2{8.f, 0.f}, 64.f * glm::vec2{0.f, 3.f}};
	SGE::Object* obstacle1  = new QuadObstacle(RB1,         RB1,          angle(), Diamond1);
	SGE::Object* obstacle2  = new QuadObstacle(Width - RB1, RB1,          angle(), Diamond1);
	SGE::Object* obstacle3  = new QuadObstacle(Width - RB1, Height - RB1, angle(), Diamond1);
	SGE::Object* obstacle4  = new QuadObstacle(RB1,         Height - RB1, angle(), Diamond1);
	SGE::Object* obstacle5  = new QuadObstacle(RB2,         RB2,          angle(), Diamond2);
	SGE::Object* obstacle6  = new QuadObstacle(Width - RB2, RB2,          angle(), Diamond2);
	SGE::Object* obstacle7  = new QuadObstacle(Width - RB2, Height - RB2, angle(), Diamond2);
	SGE::Object* obstacle8  = new QuadObstacle(RB2,         Height - RB2, angle(), Diamond2);
	SGE::Object* obstacle9  = new QuadObstacle(RB2,         .5f * Height, angle(), Diamond2);
	SGE::Object* obstacle10 = new QuadObstacle(.5f * Width, RB2,          angle(), Diamond2);
	SGE::Object* obstacle11 = new QuadObstacle(Width - RB2, .5f * Height, angle(), Diamond2);
	SGE::Object* obstacle12 = new QuadObstacle(.5f * Width, Height - RB2, angle(), Diamond2);

	for(auto ob : {obstacle1, obstacle2, obstacle3, obstacle4})
	{
		obBatch->addObject(ob, Diamond1);
		this->world.AddObstacle(ob);
		this->gs->obstacles.push_back(ob);
	}

	for(auto ob : {obstacle5, obstacle6, obstacle7, obstacle8, obstacle9, obstacle10, obstacle11, obstacle12})
	{
		obBatch->addObject(ob, Diamond2);
		this->world.AddObstacle(ob);
		this->gs->obstacles.push_back(ob);
	}

	//Grid
//#define GraphCellDebug
//#define GraphEdgeDebug
	{
		std::queue<GridCellBuild*> cells;
		int intersections = 0;
		GridCellBuild grid[Y][X];
		std::pair<int, int> directions[8] =
		{
			{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
		};
		b2Vec2 points[4] = {{-.5f, 0.f}, {0.f, -.5f}, {0.5f, 0.f}, {0.f, .5f}};
		b2Vec2 diags[4];
		for(size_t i = 0; i < 4u; ++i)
			diags[i] = b2Mul(b2Rot(0.5f * b2_pi), points[i]);

		for(size_t x = 0u; x < X; ++x)
			for(size_t y = 0u; y < Y; ++y)
			{
				grid[y][x].x = x;
				grid[y][x].y = y;
			}

		grid[0][0].state = GridCellBuild::Queued;
		cells.push(&grid[0][0]);

		while(!cells.empty())
		{
			GridCellBuild& currentCell = *cells.front();
			cells.pop();
			intersections = 0;
			b2Vec2 pos = b2Vec2{0.5f + currentCell.x, 0.5f + currentCell.y};
			std::vector<SGE::Object*> obstacles = std::move(this->world.getObstacles(pos, 1.5f));
			for(SGE::Object* o : obstacles)
			{
				QuadObstacle* qo = dynamic_cast<QuadObstacle*>(o);
				if(!qo) continue;
				for(Edge edge : qo->getEdges())
				{
					b2Vec2 radius = -edge.Normal();
					radius *= 0.5f;
					float dist = b2DistanceSquared(pos, edge.From());
					if(dist <= 0.25f)
					{
						currentCell.state = GridCellBuild::Invalid;
						break;
					}
					b2Vec2 intersection;
					if(LineIntersection(pos, pos + radius, edge.From(), edge.To(), dist, intersection))
					{
						currentCell.state = GridCellBuild::Invalid;
						break;
					}
					if(LineIntersection(pos, pos + b2Vec2{100.f, 0.f}, edge.From(), edge.To(), dist, intersection))
					{
						++intersections;
					}
				}
				if(currentCell.state == GridCellBuild::Invalid || 1 == intersections % 2)
					break;
			}
			if(currentCell.state != GridCellBuild::Invalid && 0 == intersections % 2)
			{
				currentCell.state = GridCellBuild::Accepted;
				currentCell.vertex = new GridVertex(CellLabel(pos));
				this->gs->graph.AddVertex(currentCell.vertex);
#ifdef GraphCellDebug
				currentCell.dummy = new GraphCellDummy(currentCell.x, currentCell.y);
				graphTestBatch->addObject(currentCell.dummy);
#endif
				for(size_t i = 0u; i < 8u; ++i)
				{
					auto dir = directions[i];
					size_t x = currentCell.x + dir.first;
					size_t y = currentCell.y + dir.second;
					b2Vec2 edgeVec = b2Vec2{float(dir.first), float(dir.second)};
					if(x < X && y < Y)
					{
						GridCellBuild& otherCell = grid[y][x];
						switch(otherCell.state)
						{
						case GridCellBuild::Accepted:
						{
							bool intersected = false;
							for(SGE::Object* o : obstacles)
							{
								QuadObstacle* qo = dynamic_cast<QuadObstacle*>(o);
								if(!qo) continue;
								for(Edge edge : qo->getEdges())
								{
									b2Vec2(&pts)[4] = i % 2 ? diags : points;
									for(b2Vec2 offset : pts)
									{
										b2Vec2 from = pos + offset;
										b2Vec2 to = from + edgeVec;
										float dist;
										b2Vec2 inters;
										intersected = LineIntersection(from, to, edge.From(), edge.To(), dist, inters);
										if(intersected)
										{
											break;
										}
									}
									if(intersected) break;
								}
								if(intersected) break;
							}
							if(!intersected)
							{
								this->gs->graph.AddEdge(currentCell.vertex, otherCell.vertex, edgeVec.Length());
#ifdef GraphEdgeDebug
								auto edgeOb = new GraphEdgeDummy(pos + 0.5f * edgeVec);
								edgeOb->setOrientation(edgeVec.Orientation());
								edgeOb->setLayer(.5f);
								edgeOb->setShape(SGE::Shape::Rectangle(edgeVec.Length(), 0.05f, true));
								graphEdgeTestBatch->addObject(edgeOb);
#endif
							}
							break;
						}
						case GridCellBuild::Queued: break;
						case GridCellBuild::Untested:
						{
							otherCell.state = GridCellBuild::Queued;
							cells.push(&otherCell);
							break;
						}
						case GridCellBuild::Invalid: break;
						default: break;
						}
					}
				}
			}
		}
		for(size_t x = 0u; x < X; ++x)
		{
			for(size_t y = 0u; y < Y; ++y)
			{
				if(grid[y][x].state == GridCellBuild::Accepted)
				{
					auto& cell = this->gs->cells[y][x];
					cell.state = GridCell::Valid;
					cell.vertex = grid[y][x].vertex;
				}
			}
		}
		this->gs->InitRandomEngine();
//#define ASTARDEBUG
#ifdef ASTARDEBUG
		//Test
		GridVertex* begin = this->gs->cells[0][0].vertex;
		GridVertex* end = this->gs->cells[Y-1u][X - 1u].vertex;
		this->gs->graph.AStar(begin, end, DiagonalDistance{});
		while(end != begin)
		{
			graphTestBatch->addObject(new GraphCellDummy(end->Label().position));
			end = end->Parent();
		}
		for(auto v : this->gs->graph)
		{
			if(v->State() != CTL::VertexState::White)
			{
				graphTestBatch->addObject(new GraphCellDummy1(v->Label().position));
			}
		}
#endif
	} //!Grid

	//Players
	{
		this->gs->bots.reserve(Bots);
		for(int i = 0; i < Bots; ++i)
		{
			this->gs->bots.emplace_back(this->gs->GetRandomVertex()->Label().position, getCircle(), &this->world);
			DemoBot* bot = &this->gs->bots.back();
			botBatch->addObject(bot);
			this->world.AddMover(bot);
			bot->RailgunTrace = new RGTrace();
			this->gs->railBatch->addObject(bot->RailgunTrace);
		}
	}

	//Items
	{
		this->gs->GenerateItems<HealthPack>(Bots, healthBatch);
		this->gs->GenerateItems<ArmorPack>(Bots, armorBatch);
		this->gs->GenerateItems<RocketAmmo>(Bots, rlammoBatch);
		this->gs->GenerateItems<RailgunAmmo>(Bots, rgammoBatch);
	}
	
	//Logics
	this->addLogic(new SteeringBehavioursUpdate(&this->gs->bots));
	this->addLogic(new SeparateBots(&this->world, &this->gs->bots));
	this->addLogic(new MoveAwayFromObstacle(&this->world, this->gs->obstacles));
	this->addLogic(new MoveAwayFromWall(&this->world, this->gs->bots));
	this->addLogic(new BotLogic(&this->world, this->gs));
	this->addLogic(new ItemLogic(&this->world, this->gs));
	this->addLogic(new RocketLogic(this->gs, &this->world));

	//
	game->mapAction(SGE::InputBinder(new InputLua(), SGE::Key::Return));
}

void DemoScene::unloadScene()
{
	this->finalize();
}

DemoScene::~DemoScene()
{
	std::cout << "~MainScene" << std::endl;
}

template<typename Vec>
void vec_clear(Vec& vec)
{
	for (auto h : vec)
	{
		delete h;
	}
	vec.clear();
}

void DemoScene::finalize()
{
	game->getRenderer()->deleteSceneBatch(this);
	delete this->gs;
	this->level.clear();
	this->world.clear();
	vec_clear(this->getLogics());
	vec_clear(this->getActions());
	this->getObjects().clear();
	game->unmapAll();
}

void DemoScene::onDraw()
{}
