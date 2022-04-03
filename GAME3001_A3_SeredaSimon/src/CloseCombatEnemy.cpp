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
#include "Player.h"

CloseCombatEnemy::CloseCombatEnemy() : m_currentAnimationState(ENEMY_IDLE)
{
	TextureManager::Instance().loadSpriteSheet(
		"../Assets/sprites/slugSpriteData.txt",
		"../Assets/sprites/slug.png",
		"slugSpriteSheet");

	setSpriteSheet(TextureManager::Instance().getSpriteSheet("slugSpriteSheet"));
	setWidth(48);
	setHeight(48);
	
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

	// Fill in action state and patrol code
	setActionState(NO_ACTION);

	//set patrol
	m_patrol.push_back(glm::vec2(760, 40)); // top right
	m_patrol.push_back(glm::vec2(760, 560));// bot right
	m_patrol.push_back(glm::vec2(40, 560)); // bot left
	m_patrol.push_back(glm::vec2(40, 40)); // top left
	m_waypoint = 0;

	setTargetPosition(m_patrol[m_waypoint]);
	setType(AGENT);

	//create decision tree
	m_tree = new DecisionTree(this); // overloaded constructor
	m_buildTree();
	m_tree->display();

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
			TextureManager::Instance().playAnimation("slugSpriteSheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, this, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("slugSpriteSheet", getAnimation("idle"),
				x, y, 0.12f, 0, 255, this, true, SDL_FLIP_HORIZONTAL);
		}
		break;
	case ENEMY_RUN:
		if (!isFacingLeft)
		{
			TextureManager::Instance().playAnimation("slugSpriteSheet", getAnimation("run"),
				x, y, 0.10f, 0, 255, this, true);
		}
		else
		{
			TextureManager::Instance().playAnimation("slugSpriteSheet", getAnimation("run"),
				x, y, 0.12f, 0, 255, this, true, SDL_FLIP_HORIZONTAL);
		}
		break;
	default:
		break;
	}


	// draw LOS
	if (debug)
	{
	
		Util::DrawLine(getTransform()->position, getMiddleLOSEndPoint(), getLOSColour());
	}

	if (isWithinRadius)
	{
		Util::DrawCircle(getTransform()->position, 250, glm::vec4(0, 0, 1, 1));
	}

	drawHealthBar();
}

void CloseCombatEnemy::update()
{

	if (getRigidBody()->velocity.x == 0 && getRigidBody()->velocity.y == 0);
	{
		m_currentAnimationState = ENEMY_IDLE;
	}
	if (getRigidBody()->velocity.x > 0)
	{
		m_currentAnimationState = ENEMY_RUN;
		if (!isFacingLeft)
		{
			isFacingLeft = true;
		}
	}
	else if (getRigidBody()->velocity.x < 0)
	{
		m_currentAnimationState = ENEMY_RUN;
		if (isFacingLeft)
		{
			isFacingLeft = false;
		}
	}
	else if (getRigidBody()->velocity.y > 0 || getRigidBody()->velocity.y < 0)
	{
		m_currentAnimationState = ENEMY_RUN;
	}

	if (Util::distance(getTransform()->position, Player::s_pPlayerObj->getTransform()->position) < 250)
	{
		if (!isWithinRadius)
		{
			isWithinRadius = true;
		}
	}
	else
	{
		if (isWithinRadius)
		{
			isWithinRadius = false;
		}
	}

	// Determine which action to perform
	m_tree->makeDecision();
}

void CloseCombatEnemy::clean()
{

}

void CloseCombatEnemy::patrol()
{
	if (getActionState() != PATROL)
	{
		setTargetPosition(m_patrol[m_waypoint]);
		//initialize the action
		setActionState(PATROL);
	}
	m_move();
}

void CloseCombatEnemy::moveToPlayer()
{
	if (getActionState() != MOVE_TO_PLAYER)
	{
		//initialize the action
		setActionState(MOVE_TO_PLAYER);
	}
	setTargetPosition(Player::s_pPlayerObj->getTransform()->position);
	m_move();
}

void CloseCombatEnemy::m_buildAnimations()
{
	Animation idleAnimation = Animation();
	idleAnimation.name = "idle";
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-0"));
	idleAnimation.frames.push_back(getSpriteSheet()->getFrame("Idle-1"));
	setAnimation(idleAnimation);

	Animation runAnimation = Animation();
	runAnimation.name = "run";
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-0"));
	runAnimation.frames.push_back(getSpriteSheet()->getFrame("Run-1"));
	setAnimation(runAnimation);

	Animation AttackAnimation = Animation();
	AttackAnimation.name = "attack";
	AttackAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-0"));
	AttackAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-1"));
	AttackAnimation.frames.push_back(getSpriteSheet()->getFrame("Atk-2"));
	setAnimation(AttackAnimation);

	Animation TakeDamageAnimation = Animation();
	TakeDamageAnimation.name = "take_damage";
	TakeDamageAnimation.frames.push_back(getSpriteSheet()->getFrame("TakeDmg-0"));
	TakeDamageAnimation.frames.push_back(getSpriteSheet()->getFrame("TakeDmg-1"));
	TakeDamageAnimation.frames.push_back(getSpriteSheet()->getFrame("TakeDmg-2"));
	setAnimation(TakeDamageAnimation);

	Animation DieAnimation = Animation();
	DieAnimation.name = "die";
	DieAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-0"));
	DieAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-1"));
	DieAnimation.frames.push_back(getSpriteSheet()->getFrame("Die-2"));
	setAnimation(DieAnimation);
}
