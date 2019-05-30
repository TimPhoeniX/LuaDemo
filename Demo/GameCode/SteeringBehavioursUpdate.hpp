#pragma once
#include <Logic/sge_logic.hpp>
#include <vector>
#include "DemoBot.hpp"

class SteeringBehavioursUpdate: public SGE::Logic
{
protected:
	std::vector<DemoBot>* objects = nullptr;
public:
	explicit SteeringBehavioursUpdate(std::vector<DemoBot>* objects);
	virtual void performLogic() override;
};
