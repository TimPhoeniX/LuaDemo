﻿#pragma once

#include <Object/sge_object.hpp>
#include "DemoBot.hpp"
#include "Utilities.hpp"

class Rocket: public SGE::Object
{
	constexpr static float width = 0.8f;
	constexpr static float height = 0.5f * width;
	constexpr static float radius = 3.f;
	static SGE::Shape* Shape()
	{
		static SGE::Shape* defShape = SGE::Shape::Rectangle(Width(), Height(), false);
		return defShape;
	}
	b2Vec2 heading = b2Vec2_zero;
	float exploding = 0.24f;
	bool primed = false;
public:
	Rocket(b2Vec2 pos, b2Vec2 dir): Object(pos, true, Shape()), heading(dir)
	{
		this->Rocket::setVisible(true);
		this->setOrientation(this->heading.Orientation());
	}

	static SGE::Shape* ExplosionShape()
	{
		static SGE::Shape* expShape = SGE::Shape::Circle(radius, false);
		return expShape;
	}

	static float Speed()
	{
		return DemoBot::RocketSpeed;
	}

	constexpr static float Radius()
	{
		return radius;
	}

	b2Vec2 Heading() const
	{
		return this->heading;
	}

	void Prime()
	{
		this->primed = true;
	}

	bool IsPrimed() const
	{
		return this->primed;
	}

	constexpr static float Width()
	{
		return  width;
	}
	
	constexpr static float Height()
	{
		return  height;
	}

	float RemainingTime() const
	{
		return this->exploding;
	}

	void Expire(float delta)
	{
		this->exploding -= delta;
	}
};

class Item: public SGE::Object
{
public:
	enum class IType: unsigned
	{
		Health, Armor, RLAmmo, RGAmmo
	};
protected:
	virtual void consumeItem(DemoBot&) = 0;
	IType type;
	float cd = itemCD;

	Item(b2Vec2 pos, IType type);
public:
	static float itemCD;
	void useItem(DemoBot&);


	IType Type() const
	{
		return this->type;
	}

	void Reload(float delta)
	{
		if(this->cd > 0.f) this->cd -= delta;
	}

	bool Respawnable() const
	{
		return !this->getVisible() && this->cd < 0.f;
	}

	void Respawn(b2Vec2 position)
	{
		this->setVisible(true);
		this->setPosition(position);
	}
};

class HealthPack: public Item
{
protected:
	void consumeItem(DemoBot& bot) override;
public:
	static float HealthValue;
	HealthPack();
	explicit HealthPack(b2Vec2 pos);
};

class ArmorPack: public Item
{
protected:
	void consumeItem(DemoBot& bot) override;
public:
	static float ArmorValue;
	ArmorPack();
	explicit ArmorPack(b2Vec2 pos);

};

class RailgunAmmo: public Item
{
protected:
	void consumeItem(DemoBot& bot) override;
public:
	static unsigned AmmoValue;
	RailgunAmmo();
	explicit RailgunAmmo(b2Vec2 pos);
};

class RocketAmmo: public Item
{
protected:
	void consumeItem(DemoBot& bot) override;
public:
	static unsigned AmmoValue;
	RocketAmmo();
	explicit RocketAmmo(b2Vec2 pos);
};