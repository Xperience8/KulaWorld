#include "stdafx.h"
#include "KulaWorldLogic.h"

#include "GameView\KulaWorldView.h"
#include "GameView\KulaWorldEndLevelMenuView.h"

#include "EightEngine\Actor\ActorFactory.h"
#include "EightEngine\Actor\Components\AudioComponent.h"
#include "EightEngine\Actor\Components\CollectItemTriggerComponent.h"

#include "EightEngine\GameLogic\GamePhysics.h"
#include "EightEngine\Process\ProcessManager.h"

#include "EightEngine\Audio\AudioManager.h"
#include "EightEngine\Audio\Audio.h"

#include "EightEngine\GameLogic\LevelDatabase.h"

#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\DestroyActorEventData.h"
#include "EightEngine\EventManager\Events\LooseEventData.h"
#include "EightEngine\EventManager\Events\PhysCollisionEventData.h"
#include "EightEngine\EventManager\Events\WinEventData.h"
#include "EightEngine\EventManager\Events\ChangeGravityEventData.h"
#include "EightEngine\EventManager\Events\StartLevelEventData.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"
#include "EightEngine\EventManager\Events\GoToMainMenuEventData.h"
#include "EightEngine\EventManager\Events\StartGameEventData.h"

#include "EightEngine\GameLogic\GameTime.h"

void KulaWorldLogic::StartUp()
{
	m_pActorFactory = std::make_unique<ActorFactory>();

	g_LevelDatabase.Init();

	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::CollectItemEvent),
		CollectItemEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::ChangeGravityEvent),
		ChangeGravityEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::CollisionEvent),
		PhysCollisionEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::LooseLevelEvent),
		LooseEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::WinLevelEvent),
		WinEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::StartLevelEvent),
		StartLevelEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::GoToMainMenuEvent),
		GoToMainMenuEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldLogic::StartNewGameEvent),
		StartNewGameEventData::s_TYPE);


	ChangeGameState(GameState::GS_INITIALIZING);
}
void KulaWorldLogic::ShutDown()
{
	m_GameLogicState = GameState::GS_INVALID;

	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), StartNewGameEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), GoToMainMenuEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), StartLevelEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), WinEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), LooseEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), PhysCollisionEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), ChangeGravityEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), CollectItemEventData::s_TYPE);

	m_CurrentLevel.Destroy();
	m_pActorFactory.reset();
}
void KulaWorldLogic::Update()
{
	switch (m_GameLogicState)
	{
	case GameState::GS_INVALID:
		break;
	case GameState::GS_INITIALIZING:
		ChangeGameState(GameState::GS_MAIN_MENU);
		break;
	case GameState::GS_MAIN_MENU:
		m_CurrentLevel.Init("MainMenuConfig.xml");

		ChangeGameState(GameState::GS_LOADING_LEVEL);
		break;
	case GameState::GS_LOADING_LEVEL:
		m_CurrentLevel.Load(this);
		m_CurrentLevel.Start();

		ChangeGameState(GameState::GS_LOADING_PLAYERS);
		break;
	case GameState::GS_LOADING_PLAYERS:
		m_Timer.Reset();

		ChangeGameState(GameState::GS_GAME_RUNNING);
		break;
	case GameState::GS_GAME_RUNNING:
		m_Timer.Tick();

		{
			static const float UPDATE_TIME = 1.f / 60.f;
			static float elapsedTime = 0.f;
			elapsedTime += m_Timer.GetDeltaTime();

			while (elapsedTime >= UPDATE_TIME)
			{
				elapsedTime -= UPDATE_TIME;

				g_Physics.Update(UPDATE_TIME);
				g_ProcessManager.Update(UPDATE_TIME);

				m_CurrentLevel.Update(UPDATE_TIME);
			}
		}
		//

		//g_Physics.Update(m_Timer.GetDeltaTime());
		//g_ProcessManager.Update(m_Timer.GetDeltaTime());

		//m_CurrentLevel.Update(m_Timer.GetDeltaTime());
		m_CurrentLevel.Render();

		break;
	case GameState::GS_PAUSED:
		m_Timer.Tick();

		m_CurrentLevel.Render();

		break;
	}
}
std::shared_ptr<IGameView> KulaWorldLogic::CreateGameView(
	UINT viewType, const std::shared_ptr<Actor> &pActor)
{
	std::shared_ptr<IGameView> pGameView;

	switch (viewType)
	{
	case GameViewType::PLAYER_VIEW:
		pGameView = std::make_shared<KulaWorldView>(std::weak_ptr<Actor>(pActor));
		break;
	case GameViewType::MAIN_MENU_VIEW:
		pGameView = std::make_shared<MainMenuView>(std::weak_ptr<Actor>(pActor));
		break;
	case GameViewType::MENU_VIEW:
		pGameView = std::make_shared<KulaWorldEndLevelMenuView>(std::weak_ptr<Actor>(pActor));
		break;
	default:
		return std::shared_ptr<IGameView>();
	}

	pGameView->VOnRestore();
	pGameView->VOnResize(g_Renderer.GetScreen());
	pGameView->VRegisterEvents();

	return pGameView;
}
void KulaWorldLogic::StartNewGameEvent(const EventPtr<IEventData> &pEvent)
{
	g_LevelDatabase.StartNewGame();

	m_CurrentLevel.Restart(this);
}
void KulaWorldLogic::GoToMainMenuEvent(const EventPtr<IEventData> &eventData)
{
	m_CurrentLevel.Destroy();
	g_AudioManager.StopPlaying();

	m_GameLogicState = GameState::GS_MAIN_MENU;
}
void KulaWorldLogic::StartLevelEvent(const EventPtr<IEventData> &eventData)
{
	auto pEventData = StaticPointerCast<StartLevelEventData>(eventData);

	m_CurrentLevel.Destroy();
	g_AudioManager.StopPlaying();

	m_CurrentLevel.Init(pEventData->GetLevelName());

	m_GameLogicState = GameState::GS_LOADING_LEVEL;
}
void KulaWorldLogic::WinLevelEvent(const EventPtr<IEventData> &eventData)
{
	auto pWinLevelEventData = StaticPointerCast<WinEventData>(eventData);

	g_LevelDatabase.LevelCompleted(m_CurrentLevel.GetLevelName(), pWinLevelEventData->GetStatus());
	g_LevelDatabase.Save();

	m_CurrentLevel.End(this);

	std::static_pointer_cast<KulaWorldEndLevelMenuView>(m_CurrentLevel.GetActorView(
		m_pActorFactory->GetLastActorId()))->ShowReward(pWinLevelEventData->GetStatus());

	g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(false));
	m_GameLogicState = GameState::GS_PAUSED;
}
void KulaWorldLogic::LooseLevelEvent(const EventPtr<IEventData> &eventData)
{
	m_CurrentLevel.Restart(this);

	m_GameLogicState = GameState::GS_LOADING_PLAYERS;
}
void KulaWorldLogic::CollectItemEvent(const EventPtr<IEventData> &eventData)
{
	auto collectItemEvent = StaticPointerCast<CollectItemEventData>(eventData);

	m_CurrentLevel.RemoveActor(collectItemEvent->GetTriggerId());
}
void KulaWorldLogic::ChangeGravityEvent(const EventPtr<IEventData> &eventData)
{
	auto pChangeGravityEventData = StaticPointerCast<ChangeGravityEventData>(eventData);

	auto pDynamicView = std::static_pointer_cast<KulaWorldView>(
		m_CurrentLevel.GetActorView(pChangeGravityEventData->GetActorId()));
	if (pDynamicView)
	{
		pDynamicView->ChangeGravity(pChangeGravityEventData->GetStatus());
	}
}
void KulaWorldLogic::CollisionEvent(const EventPtr<IEventData> &eventData)
{
	auto pEventData = StaticPointerCast<PhysCollisionEventData>(eventData);

	Actor *pDynamicActor = pEventData->GetFirstActor();
	if (pDynamicActor->GetActorType() != ActorType::DYNAMIC_ACTOR)
	{
		pDynamicActor = pEventData->GetSecondActor();
	}

	auto pAudioComponent = pDynamicActor->GetActorComponent<AudioComponent>();
	if (pAudioComponent)
	{
		pAudioComponent->GetAudio("Collision")->Play();
	}
}
