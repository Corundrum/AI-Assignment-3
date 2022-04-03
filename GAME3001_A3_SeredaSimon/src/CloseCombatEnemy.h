#pragma once
#ifndef __CLOSE_COMBAT_ENEMY__
#define __CLOSE_COMBAT_ENEMY__

#include "BaseEnemy.h"
#include "DecisionTree.h"
#include "Sprite.h"
#include "EnemyAnimationState.h"

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
	
private:
	// private movement variables
	void m_buildAnimations();
	bool isFacingLeft = 0;
	EnemyAnimationState m_currentAnimationState;
};


#endif /* defined (__CLOSE_COMBAT_ENEMY__) */