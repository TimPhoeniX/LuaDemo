#pragma once
#include <Logic/sge_logic.hpp>
#include "IO/Key/sge_key.hpp"

class DemoBot;

class PlayerMove: public SGE::Logic
{
	float speed = 0;
	const SGE::Key up;
	const SGE::Key down;
	const SGE::Key left;
	const SGE::Key right;
	DemoBot* object = nullptr;
public:
	PlayerMove(DemoBot* object, const float speed, const SGE::Key up, const SGE::Key down, const SGE::Key left, const SGE::Key right);
	void performLogic() override;
};
