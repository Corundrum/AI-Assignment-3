#include "Player.h"
#include "TextureManager.h"
#include "EventManager.h"

Player::Player(): m_currentAnimationState(PLAYER_IDLE)
{
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/PlayerSpriteData.txt",
		"../Assets/sprites/player.png", 
		"playerSpriteSheet");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerSpriteSheet"));
	
	// set size
	setWidth(96);
	setHeight(96);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(PLAYER);

	m_buildAnimations();
}

Player::~Player()
= default;

void Player::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the player according to animation state
	switch(m_currentAnimationState)
	{
	case PLAYER_IDLE:
		if (!isFacingLeft)
		{
			TextureManager::Instance().playAnimation("playerSpriteSheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, this, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("playerSpriteSheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, this, true, SDL_FLIP_HORIZONTAL);
		}
		break;
	case PLAYER_RUN:
		if (!isFacingLeft)
		{
			TextureManager::Instance().playAnimation("playerSpriteSheet", getAnimation("run"),
				x, y, 0.52f, 0, 255, this, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("playerSpriteSheet", getAnimation("run"),
				x, y, 0.52f, 0, 255, this, true, SDL_FLIP_HORIZONTAL);
		}
		break;
	default:
		break;
	}
	
}

void Player::update()
{
	setAnimationState(PLAYER_IDLE);
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W))
	{
		setAnimationState(PLAYER_RUN);
		getTransform()->position.y -= 4;
	}
	else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
	{
		setAnimationState(PLAYER_RUN);
		getTransform()->position.y += 4;
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
	{
		if (!isFacingLeft)
		{
			isFacingLeft = true;
		}
		setAnimationState(PLAYER_RUN);
		getTransform()->position.x -= 4;
	}
	else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
	{
		if (isFacingLeft)
		{
			isFacingLeft = false;
		}
		setAnimationState(PLAYER_RUN);
		getTransform()->position.x += 4;
	}

}

void Player::clean()
{
}

void Player::setAnimationState(const PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Player::m_buildAnimations()
{
	Animation idleAnimation = Animation();
	idleAnimation.name = "idle";
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-0"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-1"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-2"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-3"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-4"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-5"));
	setAnimation(idleAnimation);

	Animation runAnimation = Animation();
	runAnimation.name = "run";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-1"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-2"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-3"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-4"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-5"));
	setAnimation(runAnimation);

	Animation AttackAnimation = Animation();
	runAnimation.name = "attack";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-1"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-2"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-3"));
	setAnimation(AttackAnimation);

	Animation DieAnimation = Animation();
	runAnimation.name = "attack";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-1"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-2"));
	setAnimation(DieAnimation);
}
