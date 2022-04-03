#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	for (auto const& i : m_tiles)
	{
		i.second->draw();
	}

	drawDisplayList();

	float health_percent = playerHealth / 100.0;

	Util::DrawRect(glm::vec2(15, 15), 202, 40, glm::vec4(1, 1, 1, 1));
	Util::DrawFilledRect(glm::vec2(16, 16), 200, 38, glm::vec4(0,0,0,1));
	Util::DrawFilledRect(glm::vec2(16, 16), 200 * health_percent, 38, glm::vec4(1, 0.2, 0.2, 1));


	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 25, 25, 25, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	
	for (auto const& i : m_tiles)
	{
		i.second->update();
	}

	//LOS Checks for Enemies to player
	for (auto enemy : BaseEnemy::s_EnemiesObj)
	{
		enemy->getTree()->getLOSNode()->setLOS(enemy->checkAgentLOSToTarget(enemy, Player::s_pPlayerObj, m_pObstacles));
	}

	//Node Checks from enemies
	switch (m_LOSMode)
	{
	case 0:
		m_checkAllNodesWithTarget(BaseEnemy::s_EnemiesObj.back());
		break;
	}
	
	//ALL COLLISION

	for (auto obstacle : m_pObstacles)
	{
		auto hitbox = Player::s_pPlayerObj->getHitBox();
		SDL_Rect box = { obstacle->getTransform()->position.x - 10, obstacle->getTransform()->position.y + 44, obstacle->getWidth() * 0.75, obstacle->getHeight() * 0.35 };
		
		glm::vec2 top = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y);
		glm::vec2 right = glm::vec2(hitbox.x + hitbox.w, hitbox.y + hitbox.h / 2);
		glm::vec2 bottom = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h);
		glm::vec2 left = glm::vec2(hitbox.x, hitbox.y + hitbox.h / 2);

		if (SDL_HasIntersection(&hitbox, &box))
		{
			if (right.x > box.x && right.x < box.x + box.w && right.y > box.y && right.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.x = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.x = 0;
				Player::s_pPlayerObj->getTransform()->position.x -= 2.5;
			}
			if (left.x > box.x && left.x < box.x + box.w && left.y > box.y && left.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.x = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.x = 0;
				Player::s_pPlayerObj->getTransform()->position.x += 2.5;
			}
			if (top.x > box.x && top.x < box.x + box.w && top.y > box.y && top.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.y = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.y = 0;
				Player::s_pPlayerObj->getTransform()->position.y += 2.5;
			}
			if (bottom.x > box.x && bottom.x < box.x + box.w && bottom.y > box.y && bottom.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.y = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.y = 0;
				Player::s_pPlayerObj->getTransform()->position.y -= 2.5;
			}
		}
		for (auto enemy : BaseEnemy::s_EnemiesObj)
		{
			hitbox = enemy->getHitBox();
			top = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y);
			right = glm::vec2(hitbox.x + hitbox.w, hitbox.y + hitbox.h / 2);
			bottom = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h);
			left = glm::vec2(hitbox.x, hitbox.y + hitbox.h / 2);

			if (SDL_HasIntersection(&hitbox, &box))
			{
				if (right.x > box.x && right.x < box.x + box.w && right.y > box.y && right.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.x = 0;
					enemy->getRigidBody()->acceleration.x = 0;
					enemy->getTransform()->position.x -= 2;
				}
				if (left.x > box.x && left.x < box.x + box.w && left.y > box.y && left.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.x = 0;
					enemy->getRigidBody()->acceleration.x = 0;
					enemy->getTransform()->position.x += 2;
				}
				if (top.x > box.x && top.x < box.x + box.w && top.y > box.y && top.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.y = 0;
					enemy->getRigidBody()->acceleration.y = 0;
					enemy->getTransform()->position.y += 2;
				}
				if (bottom.x > box.x && bottom.x < box.x + box.w && bottom.y > box.y && bottom.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.y = 0;
					enemy->getRigidBody()->acceleration.y = 0;
					enemy->getTransform()->position.y -= 2;
				}
			}
		}
	}

	//Collision with obstacle tiles
	TiledLevel* pLevel = static_cast<TiledLevel*>(GetGo("objects"));
	for (unsigned i = 0; i < pLevel->getObstacles().size(); i++)
	{
		auto hitbox = Player::s_pPlayerObj->getHitBox();
		auto box = *pLevel->getObstacles()[i]->getDst();

		glm::vec2 top = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y);
		glm::vec2 right = glm::vec2(hitbox.x + hitbox.w, hitbox.y + hitbox.h / 2);
		glm::vec2 bottom = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h);
		glm::vec2 left = glm::vec2(hitbox.x, hitbox.y + hitbox.h / 2);

		if (SDL_HasIntersection(&hitbox, &box))
		{
			if (right.x > box.x && right.x < box.x + box.w && right.y > box.y && right.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.x = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.x = 0;
				Player::s_pPlayerObj->getTransform()->position.x -= 2.5;
			}
			if (left.x > box.x && left.x < box.x + box.w && left.y > box.y && left.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.x = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.x = 0;
				Player::s_pPlayerObj->getTransform()->position.x += 2.5;
			}
			if (top.x > box.x && top.x < box.x + box.w && top.y > box.y && top.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.y = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.y = 0;
				Player::s_pPlayerObj->getTransform()->position.y += 2.5;
			}
			if (bottom.x > box.x && bottom.x < box.x + box.w && bottom.y > box.y && bottom.y < box.y + box.h)
			{
				Player::s_pPlayerObj->getRigidBody()->velocity.y = 0;
				Player::s_pPlayerObj->getRigidBody()->acceleration.y = 0;
				Player::s_pPlayerObj->getTransform()->position.y -= 2.5;
			}

		}
		for (auto enemy : BaseEnemy::s_EnemiesObj)
		{
			hitbox = enemy->getHitBox();
			top = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y);
			right = glm::vec2(hitbox.x + hitbox.w, hitbox.y + hitbox.h / 2);
			bottom = glm::vec2(hitbox.x + hitbox.w / 2, hitbox.y + hitbox.h);
			left = glm::vec2(hitbox.x, hitbox.y + hitbox.h / 2);

			if (SDL_HasIntersection(&hitbox, &box))
			{
				if (right.x > box.x && right.x < box.x + box.w && right.y > box.y && right.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.x = 0;
					enemy->getRigidBody()->acceleration.x = 0;
					enemy->getTransform()->position.x -= 2;
				}
				if (left.x > box.x && left.x < box.x + box.w && left.y > box.y && left.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.x = 0;
					enemy->getRigidBody()->acceleration.x = 0;
					enemy->getTransform()->position.x += 2;
				}
				if (top.x > box.x && top.x < box.x + box.w && top.y > box.y && top.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.y = 0;
					enemy->getRigidBody()->acceleration.y = 0;
					enemy->getTransform()->position.y += 2;
				}
				if (bottom.x > box.x && bottom.x < box.x + box.w && bottom.y > box.y && bottom.y < box.y + box.h)
				{
					enemy->getRigidBody()->velocity.y = 0;
					enemy->getRigidBody()->acceleration.y = 0;
					enemy->getTransform()->position.y -= 2;
				}
			}
		
		}
	}
}

TileObject* PlayScene::GetGo(const std::string& s)
{
	auto it = std::find_if(m_tiles.begin(), m_tiles.end(), 
		[&](const std::pair<std::string, TileObject*>& element)
		{
			return element.first == s;
		}
	);
	if (it != m_tiles.end())
	{
		return it->second;
	}
	else return nullptr;
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	if (EventManager::Instance().keyPressed(SDL_SCANCODE_H))
	{
		if (m_isGridEnabled)
		{
			m_isGridEnabled = false;
			m_toggleGrid(false);
			for (auto enemy : BaseEnemy::s_EnemiesObj)
			{
				enemy->debug = 0;
			}
		}
		else
		{
			m_isGridEnabled = true;
			m_toggleGrid(true);
			for (auto enemy : BaseEnemy::s_EnemiesObj)
			{
				enemy->debug = 1;
			}
		}
	}

	EventManager::Instance().update();
}

void PlayScene::start()
{
	m_guiTitle = "Play Scene";


	SoundManager::Instance().load("../Assets/audio/FallenLeaves.mp3", "fallen_leaves", SOUND_MUSIC);
	SoundManager::Instance().setMusicVolume(16);
	SoundManager::Instance().playMusic("fallen_leaves");

	TextureManager::Instance().load("../Assets/tiles/MysticWoodsTileMap.png", "woodsTiles");
	m_tiles.push_back(std::pair<std::string, TileObject*>("ground", new TiledLevel(25, 34, 24, 24, "../Assets/tiles/TileData.txt", "../Assets/tiles/ForestLayer1.txt", "woodsTiles")));
	m_tiles.push_back(std::pair<std::string, TileObject*>("objects", new TiledLevel(25, 34, 24, 24, "../Assets/tiles/TileData.txt", "../Assets/tiles/ForestLayer2.txt", "woodsTiles")));

	BaseEnemy::s_EnemiesObj.push_back(new CloseCombatEnemy());
	BaseEnemy::s_EnemiesObj.back()->getTransform()->position = glm::vec2(720, 40);
	addChild(BaseEnemy::s_EnemiesObj.back());

	Player::s_pPlayerObj = new Player();
	Player::s_pPlayerObj->getTransform()->position = glm::vec2(120, 300);
	addChild(Player::s_pPlayerObj, 1);

	m_pObstacles.push_back(new Obstacle());
	m_pObstacles.back()->getTransform()->position = glm::vec2(150, 440);
	addChild(m_pObstacles.back(), 2);

	m_pObstacles.push_back(new Obstacle());
	m_pObstacles.back()->getTransform()->position = glm::vec2(535, 290);
	addChild(m_pObstacles.back(), 2);

	m_pObstacles.push_back(new Obstacle());
	m_pObstacles.back()->getTransform()->position = glm::vec2(130, 210);
	addChild(m_pObstacles.back(), 2);

	m_pObstacles.push_back(new Obstacle());
	m_pObstacles.back()->getTransform()->position = glm::vec2(380, 220);
	addChild(m_pObstacles.back(), 2);

	m_LOSMode = 0;
	m_obstacleBuffer = -5;
	m_pathNodeLOSDistance = 250;
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE;
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_clearNodes(); // Because we rebuild/redraw the grid if we move an obstacle

	// add path_nodes to the Grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->getTransform()->position = glm::vec2((col * tileSize) + offset.x, (row * tileSize) + offset.y);
			bool keepNode = true;
			for (auto obstacle : m_pObstacles)
			{
				if (CollisionManager::AABBCheckWithBuffer(path_node, obstacle, m_obstacleBuffer))
				{
					keepNode = false; // We have collision between node and an obstacle
				}
			}
			
			TiledLevel* pLevel = static_cast<TiledLevel*>(GetGo("objects"));
			for (unsigned i = 0; i < pLevel->getObstacles().size(); i++)
			{
				if (CollisionManager::AABBCheckWithBuffer(path_node, pLevel->getObstacles()[i]->getDst(), m_obstacleBuffer))
				{
					keepNode = false;
				}
			}

			if (keepNode)
			{
				addChild(path_node, 0);
				m_pGrid.push_back(path_node);
			}
			else delete path_node;
		}
	}
	// if Grid is supposed to be hidden, make it so!
	m_toggleGrid(m_isGridEnabled);
}

void PlayScene::m_toggleGrid(bool state)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setVisible(state);
	}
}

bool PlayScene::m_checkAgentLOS(Agent* agent, DisplayObject* target_object)
{
	bool hasLOS = false;
	agent->setHasLOS(hasLOS);
	// if ship to target distance is less than or equal to LOS Distance
	auto AgentToTargetDistance = Util::getClosestEdge(agent->getTransform()->position, target_object);
	if (AgentToTargetDistance <= agent->getLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : getDisplayList())
		{
			if (object->getType() == NONE) continue; // Added Lab 7.
			auto AgentToObjectDistance = Util::getClosestEdge(agent->getTransform()->position, object);
			if (AgentToObjectDistance > AgentToTargetDistance) continue;
			if ((object->getType() != AGENT) && (object->getType() != PATH_NODE) && (object->getType() != TARGET))
			{
				contactList.push_back(object);
			}
		}
		const glm::vec2 agentEndPoint = agent->getTransform()->position + agent->getCurrentDirection() * agent->getLOSDistance();
		hasLOS = CollisionManager::LOSCheck(agent,
			agentEndPoint, contactList, target_object);
		agent->setHasLOS(hasLOS);
	}
	return hasLOS;
}

bool PlayScene::m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object)
{
	// check angle to target so we can still use LOS distance for path_nodes
	auto targetDirection = target_object->getTransform()->position - path_node->getTransform()->position;
	auto normalizedDirection = Util::normalize(targetDirection);
	path_node->setCurrentDirection(normalizedDirection);

	return m_checkAgentLOS(path_node, target_object);
}

void PlayScene::m_checkAllNodesWithTarget(DisplayObject* target_object)
{
	for (auto path_node : m_pGrid)
	{
		m_checkPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (auto object : getDisplayList())
	{
		if (object->getType() == PATH_NODE)
			removeChild(object);
	}
}

void PlayScene::m_setPathNodeLOSDistance(int distance)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setLOSDistance((float)distance);
	}
}



void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Lab 7 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}
	ImGui::Separator();

	if (ImGui::Button("Node LOS to Target", { 300,20 }))
	{
		m_LOSMode = 0;
	}

	if (m_LOSMode == 0)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::SliderInt("Path Node LOS Distance", &m_pathNodeLOSDistance, 0, 500))
	{
		m_setPathNodeLOSDistance(m_pathNodeLOSDistance);
	}
	ImGui::End();
}

Player* Player::s_pPlayerObj;
std::vector<BaseEnemy*> BaseEnemy::s_EnemiesObj;