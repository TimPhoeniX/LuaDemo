#include "Objects.hpp"

Item::Item(b2Vec2 pos, Item::IType type): Object(pos, true, getCircle()), type(type)
{
	this->setLayer(0.5f);
	this->Item::setVisible(false);
}

float Item::itemCD = 15.f;

void Item::useItem(DemoBot& bot)
{
	this->visible = false;
	this->cd = itemCD;
	this->consumeItem(bot);
}

HealthPack::HealthPack(): Item(b2Vec2_zero, IType::Health)
{}

HealthPack::HealthPack(b2Vec2 pos): Item(pos, IType::Health)
{}

float HealthPack::HealthValue = 50.f;

void HealthPack::consumeItem(DemoBot& bot)
{
	bot.AddHealth(HealthValue);
}

ArmorPack::ArmorPack(): Item(b2Vec2_zero, IType::Armor)
{}
	
ArmorPack::ArmorPack(b2Vec2 pos): Item(pos, IType::Armor)
{}

float ArmorPack::ArmorValue = 50.f;

void ArmorPack::consumeItem(DemoBot& bot)
{
	bot.AddArmor(ArmorValue);
}

RailgunAmmo::RailgunAmmo(): Item(b2Vec2_zero, IType::RGAmmo)
{}

RailgunAmmo::RailgunAmmo(b2Vec2 pos): Item(pos, IType::RGAmmo)
{}

unsigned RailgunAmmo::AmmoValue = DemoBot::RailgunDefaultAmmo;

void RailgunAmmo::consumeItem(DemoBot& bot)
{
	bot.AddRailgunAmmo(RailgunAmmo::AmmoValue);
}

RocketAmmo::RocketAmmo(): Item(b2Vec2_zero, IType::RLAmmo)
{}

RocketAmmo::RocketAmmo(b2Vec2 pos): Item(pos, IType::RLAmmo)
{}

unsigned RocketAmmo::AmmoValue = DemoBot::LauncherDefaultAmmo;

void RocketAmmo::consumeItem(DemoBot& bot)
{
	bot.AddLauncherAmmo(RocketAmmo::AmmoValue);
}
