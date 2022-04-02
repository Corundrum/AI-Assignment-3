#include "Player.h"
#include "TextureManager.h"
#include "EventManager.h"

Player::Player(): m_currentAnimationState(PLAYER_IDLE)
{
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/atlas.txt",
		"../Assets/sprites/atlas.png", 
		"spritesheet");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("spritesheet"));
	
	// set frame width
	setWidth(53);

	// set frame height
	setHeight(58);

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
			TextureManager::Instance().playAnimation("spritesheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("spritesheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		}
		break;
	case PLAYER_RUN:
		if (!isFacingLeft)
		{
			TextureManager::Instance().playAnimation("spritesheet", getAnimation("run"),
				x, y, 0.52f, 0, 255, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("spritesheet", getAnimation("run"),
				x, y, 0.52f, 0, 255, true, SDL_FLIP_HORIZONTAL);
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
		getTransform()->position.y -= 5;
	}
	else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
	{
		setAnimationState(PLAYER_RUN);
		getTransform()->position.y += 5;
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
	{
		if (!isFacingLeft)
		{
			isFacingLeft = true;
		}
		setAnimationState(PLAYER_RUN);
		getTransform()->position.x -= 5;
	}
	else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
	{
		if (isFacingLeft)
		{
			isFacingLeft = false;
		}
		setAnimationState(PLAYER_RUN);
		getTransform()->position.x += 5;
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
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-0"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-1"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-2"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-idle-3"));

	setAnimation(idleAnimation);

	Animation runAnimation = Animation();

	runAnimation.name = "run";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-1"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-2"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("megaman-run-3"));

	setAnimation(runAnimation);
}
