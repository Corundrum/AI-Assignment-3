#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "PlayerAnimationState.h"
#include "Sprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player();
	static Player* s_pPlayerObj;

	// Life Cycle Methods
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	SDL_Rect getHitBox() { return hitBox; }

	// setters
	void setAnimationState(PlayerAnimationState new_state);

private:
	void m_buildAnimations();

	SDL_Rect hitBox;

	bool isFacingLeft = 0;
	PlayerAnimationState m_currentAnimationState;
};

#endif /* defined (__PLAYER__) */