#pragma once
#ifndef __ENEMY_ANIMATION_STATE__
#define __ENEMY_ANIMATION_STATE__

enum EnemyAnimationState
{
	ENEMY_IDLE,
	ENEMY_RUN,
	ENEMY_ATTACK,
	ENEMY_TAKE_DAMAGE,
	ENEMY_DEATH,
	NUM_OF_ENEMY_ANIMATION_STATES
};

#endif /* defined (__ENEMY_ANIMATION_STATE__)*/