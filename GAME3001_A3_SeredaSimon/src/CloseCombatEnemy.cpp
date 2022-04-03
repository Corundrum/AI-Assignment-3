#include "CloseCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "EventManager.h"
#include "Util.h"

#include "AttackAction.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"

#include "Sprite.h"

CloseCombatEnemy::CloseCombatEnemy() : m_currentAnimationState(ENEMY_IDLE)
{
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/PlayerSpriteData.txt",
		"../Assets/sprites/player.png",
		"playerSpriteSheet");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("playerSpriteSheet"));
	setWidth(96);
	setHeight(96);
	
	getRigidBody()->bounds = glm::vec2(getWidth(), getHeight());
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	setMaxHealth(10);
	setHealth(getMaxHealth());

	// starting motion properties
	setCurrentHeading(0.0f);// current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_maxSpeed = 10.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 2.0f; // a maximum number of pixels to add to the velocity each frame
	
	setLOSDistance(400.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));

	m_buildAnimations();
}

CloseCombatEnemy::~CloseCombatEnemy() 
= default;

void CloseCombatEnemy::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;
	
	switch (m_currentAnimationState)
	{
	case ENEMY_IDLE:
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
	case ENEMY_RUN:
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

	drawHealthBar();

	// draw LOS
	if (debug)
	{
		Util::DrawLine(getTransform()->position, getMiddleLOSEndPoint(), getLOSColour());
	}
}

void CloseCombatEnemy::update()
{

	if (getCurrentDirection().x == 0 && getCurrentDirection().y == 0);
	{
		m_currentAnimationState = ENEMY_IDLE;
	}
	if (getCurrentDirection().x > 0)
	{
		m_currentAnimationState = ENEMY_RUN;
		if (isFacingLeft)
		{
			isFacingLeft == false;
		}
	}
	else if (getCurrentDirection().x < 0)
	{
		m_currentAnimationState = ENEMY_RUN;
		if (!isFacingLeft)
		{
			isFacingLeft == true;
		}
	}
	else if (getCurrentDirection().y > 0 || getCurrentDirection().y < 0)
	{
		m_currentAnimationState = ENEMY_RUN;
	}

	// Determine which action to perform
	m_tree->makeDecision();
}

void CloseCombatEnemy::clean()
{

}


void CloseCombatEnemy::m_buildAnimations()
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
