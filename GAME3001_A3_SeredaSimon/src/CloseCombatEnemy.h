#pragma once
#ifndef __CLOSE_COMBAT_ENEMY__
#define __CLOSE_COMBAT_ENEMY__

#include "BaseEnemy.h"
#include "DecisionTree.h"

class CloseCombatEnemy final : public BaseEnemy
{
public:
	CloseCombatEnemy();
	~CloseCombatEnemy();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	//new action functions
	void moveToPlayer() override;
	
private:
	// private movement variables

};


#endif /* defined (__CLOSE_COMBAT_ENEMY__) */