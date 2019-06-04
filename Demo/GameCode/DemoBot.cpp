#include "DemoBot.hpp"
#include "SteeringBehaviours.hpp"

float DemoBot::RailgunReload = 10.f;
float DemoBot::RailgunDamage = 100.f;
float DemoBot::RailgunSpread = 0.05f;
float DemoBot::RailgunRate = 1.5f;
unsigned DemoBot::RailgunDefaultAmmo = 5u;

float DemoBot::LauncherReload = 5.f;
float DemoBot::LauncherDamage = 65.f;
float DemoBot::LauncherSpread = 0.05f;
float DemoBot::LauncherRate = 1.f;
unsigned DemoBot::LauncherDefaultAmmo = 15u;
float DemoBot::RocketSpeed = 5.f;

float DemoBot::DefaultHealth = 150.f;
float DemoBot::DefaultArmor = 250.f;

float DemoBot::maxSpeed = 3.f;
float DemoBot::maxForce = 15.f;
float DemoBot::SwapSpeed = 3.f;
