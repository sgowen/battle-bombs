#include "pch.h"
#include "Direct3DInterop.h"
#include "Direct3DContentProvider.h"
#include "Direct3DGameScreen.h"
#include "Vector2D.h"
#include "TouchEvent.h"
#include "Vector2D.h"
#include "Rectangle.h"
#include "Assets.h"
#include "OverlapTester.h"
#include "GameEvent.h"
#include "BombGameObject.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "PlayerDynamicGameObject.h"
#include "BotPlayerDynamicGameObject.h"
#include "MapSearchNode.h"
#include "GameListener.h"
#include "Renderer.h"
#include "Fire.h"
#include "Triangle.h"
#include "MapBorder.h"
#include "InsideBlock.h"
#include "BreakableBlock.h"
#include "PathFinder.h"
#include "WaitingForServerInterface.h"
#include "WaitingForLocalSettingsInterface.h"
#include "InterfaceOverlay.h"
#include "BombButton.h"
#include "PowerUpBarItem.h"
#include "PlayerAvatar.h"
#include "Font.h"
#include "SpectatorControls.h"
#include "CountDownNumberGameObject.h"
#include "DisplayXMovingGameObject.h"
#include "DisplayGameOverGameObject.h"
#include "PlayerRow.h"
#include "PlayerRowPlatformAvatar.h"
#include "Direct3DRenderer.h"
#include "Global.h"
#include "SpriteBatcher.h"
#include "RectangleBatcher.h"
#include "GameSound.h"
#include "PlayerRowAvatar.h"
#include "Crater.h"
#include "FireBall.h"
#include "IceBall.h"
#include "IcePatch.h"
#include "FallingObjectShadow.h"
#include "SpaceTile.h"
#include "ScreenState.h"
#include <string.h>
#include <sstream>

// For Randomness
#include <stdlib.h>
#include <time.h>

using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Microsoft::WRL;
using namespace Windows::Phone::Graphics::Interop;
using namespace Windows::Phone::Input::Interop;

namespace BattleBombsComp
{
	Direct3DInterop::Direct3DInterop() : m_timer(ref new BasicTimer())
	{
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
		m_touchEventsPool.push_back(TouchEvent(0, 0, Touch_Type::DOWN));
	}

	IDrawingSurfaceContentProvider^ Direct3DInterop::CreateContentProvider(Platform::String^ username, bool isOffline)
	{
		std::wstring fooW(username->Begin());
		std::string fooA(fooW.begin(), fooW.end());
		const char *usernameCharArray = fooA.c_str();

		int usernameLength = (int)strlen(usernameCharArray);

		m_username = new char[usernameLength];

		std::strncpy(m_username, usernameCharArray, usernameLength);
		m_username[usernameLength] = '\0';

		m_isOffline = isOffline;
		m_gameScreen = std::unique_ptr<Direct3DGameScreen>(new Direct3DGameScreen(usernameCharArray, isOffline));

		ComPtr<Direct3DContentProvider> provider = Make<Direct3DContentProvider>(this);
		return reinterpret_cast<IDrawingSurfaceContentProvider^>(provider.Get());
	}

	// IDrawingSurfaceManipulationHandler
	void Direct3DInterop::SetManipulationHost(DrawingSurfaceManipulationHost^ manipulationHost)
	{
		manipulationHost->PointerPressed += ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DInterop::OnPointerPressed);
		manipulationHost->PointerMoved += ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DInterop::OnPointerMoved);
		manipulationHost->PointerReleased += ref new TypedEventHandler<DrawingSurfaceManipulationHost^, PointerEventArgs^>(this, &Direct3DInterop::OnPointerReleased);
	}

	bool Direct3DInterop::onBackPressed()
	{
		return m_gameScreen->handleOnBackPressed();
	}

	void Direct3DInterop::onChatReceived(Platform::String^ message)
	{
		std::wstring fooW(message->Begin());
		std::string fooA(fooW.begin(), fooW.end());
		const char *messageCharArray = fooA.c_str();

		m_gameScreen->handleServerUpdate(messageCharArray);
	}

	void Direct3DInterop::setWinRtCallback(WinRtCallback^ callback)
	{
		m_winRtCallback = callback;
	}

	void Direct3DInterop::RenderResolution::set(Windows::Foundation::Size renderResolution)
	{
		if (renderResolution.Width != m_renderResolution.Width || renderResolution.Height != m_renderResolution.Height)
		{
			m_renderResolution = renderResolution;

			if (m_gameScreen)
			{
				m_gameScreen->updateForRenderResolutionChange(m_renderResolution.Width, m_renderResolution.Height);
				RecreateSynchronizedTexture();
			}
		}
	}

	// Event Handlers
	void Direct3DInterop::OnPointerPressed(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
	{
		addTouchEventForType(DOWN, args->CurrentPoint->RawPosition.X, args->CurrentPoint->RawPosition.Y);
	}

	void Direct3DInterop::OnPointerMoved(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
	{
		if (m_touchEventsBuffer.size() < 3)
		{
			addTouchEventForType(DRAGGED, args->CurrentPoint->RawPosition.X, args->CurrentPoint->RawPosition.Y);
		}
	}

	void Direct3DInterop::OnPointerReleased(DrawingSurfaceManipulationHost^ sender, PointerEventArgs^ args)
	{
		addTouchEventForType(UP, args->CurrentPoint->RawPosition.X, args->CurrentPoint->RawPosition.Y);
	}

	// Interface With Direct3DContentProvider
	HRESULT Direct3DInterop::Connect(_In_ IDrawingSurfaceRuntimeHostNative* host)
	{
		m_gameScreen->load(RenderResolution.Width, RenderResolution.Height, (int)WindowBounds.Width, (int)WindowBounds.Height);
		m_gameScreen->onResume();

		if (m_isOffline)
		{
			handleScreenStateOfflineModeNextMap();
		}

		// Restart timer after renderer has finished initializing.
		m_timer->Reset();

		return S_OK;
	}

	void Direct3DInterop::Disconnect()
	{
		m_gameScreen->unload();
	}

	HRESULT Direct3DInterop::PrepareResources(_In_ const LARGE_INTEGER* presentTargetTime, _Out_ BOOL* contentDirty)
	{
		*contentDirty = true;

		return S_OK;
	}

	HRESULT Direct3DInterop::GetTexture(_In_ const DrawingSurfaceSizeF* size, _Out_ IDrawingSurfaceSynchronizedTextureNative** synchronizedTexture, _Out_ DrawingSurfaceRectF* textureSubRectangle)
	{
		static std::wstring toastMessage = L"You are now in Spectator Mode"; // I know this is stupid, but it works
		
		int screenState = m_gameScreen->getState();
		switch (screenState)
		{
		case SCREEN_STATE_ENTERED_SPECTATOR_MODE:
			m_winRtCallback->Invoke("DISPLAY_TOAST", ref new Platform::String(toastMessage.c_str()));

			m_gameScreen->clearState();
		case SCREEN_STATE_NORMAL:
			for (std::vector<TouchEvent>::iterator itr = m_touchEvents.begin(); itr != m_touchEvents.end(); itr++)
			{
				if (m_touchEventsPool.size() < 50)
				{
					m_touchEventsPool.push_back(*itr);
				}
			}

			m_touchEvents.clear();
			m_touchEvents.swap(m_touchEventsBuffer);
			m_touchEventsBuffer.clear();

			m_timer->Update();
			m_gameScreen->update(m_timer->Delta, m_touchEvents);

			pushEvents();

			if (m_isOffline && _isGameOver)
			{
				_timeSinceOneOrLessPlayersRemaining += m_timer->Delta;
				if (_timeSinceOneOrLessPlayersRemaining > 0.5f)
				{
					_isGameOver = false;
					_timeSinceOneOrLessPlayersRemaining = 0;
					handleGameOver();
				}
			}
			break;
		case SCREEN_STATE_CONNECTION_ERROR:
			m_winRtCallback->Invoke("CONNECTION_ERROR", "NULL");
			break;
		case SCREEN_STATE_EXIT:
			m_winRtCallback->Invoke("EXIT", "NULL");
			break;
		case SCREEN_STATE_OFFLINE_MODE_NEXT_MAP:
			handleScreenStateOfflineModeNextMap();
			break;
		default:
			break;
		}

		m_gameScreen->present();
		m_gameScreen->handleSound();
		m_gameScreen->handleMusic();

		RequestAdditionalFrame();

		return S_OK;
	}

	ID3D11Texture2D* Direct3DInterop::GetTexture()
	{
		return m_gameScreen->getTexture();
	}

	void Direct3DInterop::pushEvents()
	{
		if (m_isOffline)
		{
			int event;
			while ((event = m_gameScreen->popOldestEventId()) > 0)
			{
				int playerIndex = 0;
				while (event >= PLAYER_EVENT_BASE)
				{
					event -= PLAYER_EVENT_BASE;
					playerIndex++;
				}

				while (event >= PLAYER_EVENT_DIRECTION_BASE)
				{
					event -= PLAYER_EVENT_DIRECTION_BASE;
				}

				while (event >= PLAYER_EVENT_GRID_X_BASE)
				{
					event -= PLAYER_EVENT_GRID_X_BASE;
				}

				while (event >= PLAYER_EVENT_GRID_Y_BASE)
				{
					event -= PLAYER_EVENT_GRID_Y_BASE;
				}

				while (event >= PLAYER_EVENT_MOD_BASE)
				{
					event -= PLAYER_EVENT_MOD_BASE;
				}

				switch (event)
				{
				case PLAYER_DEATH:
				case PLAYER_ABOUT_TO_FALL:
				case PLAYER_FREEZE:
					handleDeathForPlayerIndex(playerIndex);
					continue;
				default:
					continue;
				}
			}

			if (m_gameScreen->getNumSecondsLeft() <= 0)
			{
				std::stringstream ss;
				ss << "{\"eventType\": ";
				ss << GAME_OVER;
				ss << ", \"hasWinner\": false, \"winningPlayerIndex\": -1}";
				std::string gameOverMessageString = ss.str();
				const char *gameOverMessage = gameOverMessageString.c_str();

				m_gameScreen->handleServerUpdate(gameOverMessage);
			}
			else if (!_isSuddenDeath && m_gameScreen->getNumSecondsLeft() <= 60)
			{
				std::stringstream ss;
				ss << "{\"eventType\": ";
				ss << SUDDEN_DEATH;
				ss << "}";
				std::string suddenDeathMessageString = ss.str();
				const char *suddenDeathMessage = suddenDeathMessageString.c_str();

				m_gameScreen->handleServerUpdate(suddenDeathMessage);

				_isSuddenDeath = true;
			}
		}
		else
		{
			int eventId = m_gameScreen->popOldestEventId();
			if (eventId > 0)
			{
				Platform::String^ eventsMessage = eventId + ",";
				while ((eventId = m_gameScreen->popOldestEventId()) > 0)
				{
					eventsMessage += eventId + ",";
				}

				eventsMessage += "0"; // Terminate with 0

				static Platform::String^ EVENT_TYPE = "eventType";
				static Platform::String^ EVENTS = "events";

				static Platform::String^ PLAYER_INDEX = "playerIndex";
				static Platform::String^ X = "X";
				static Platform::String^ Y = "Y";
				static Platform::String^ DIRECTION = "Direction";

				int playerIndex = m_gameScreen->getPlayerIndex();
				Platform::String^ clientUpdate = "{";
				clientUpdate += "\"" + EVENT_TYPE + "\":" + CLIENT_UPDATE;
				clientUpdate += ",\"" + EVENTS + "\":\"" + eventsMessage + "\"";
				clientUpdate += ",\"" + PLAYER_INDEX + playerIndex + X + "\":" + m_gameScreen->getPlayerX();
				clientUpdate += ",\"" + PLAYER_INDEX + playerIndex + Y + "\":" + m_gameScreen->getPlayerY();
				clientUpdate += ",\"" + PLAYER_INDEX + playerIndex + DIRECTION + "\":" + m_gameScreen->getPlayerDirection();
				clientUpdate += "}";

				m_gameScreen->resetTimeSinceLastClientEvent();

				m_winRtCallback->Invoke("SEND_CHAT", clientUpdate);
			}
			else if (m_gameScreen->isTimeToSendKeepAlive())
			{
				m_gameScreen->resetTimeSinceLastClientEvent();

				m_winRtCallback->Invoke("SEND_CHAT", "KEEP_ALIVE,3");
			}
		}
	}

	void Direct3DInterop::handleScreenStateOfflineModeNextMap()
	{
		m_playersAlive[0] = true;
		m_playersAlive[1] = true;
		m_playersAlive[2] = true;
		m_playersAlive[3] = true;
		m_playersAlive[4] = true;
		m_playersAlive[5] = true;
		m_playersAlive[6] = true;
		m_playersAlive[7] = true;

		_isSuddenDeath = false;

		_timeSinceOneOrLessPlayersRemaining = 0;
		_isGameOver = false;

		m_gameScreen->clearState();

		srand((int)time(NULL));

		int _beginGameMessagesIndex = rand() % 6;

		if (_beginGameMessagesIndex == 0)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"3,0,0,0,5,0,0,0,0,4,1,0,2,0,5,0,0,6,5,5,1,3,4,6,0,0,0,0,0,3,3,3,0,0,0,4,2,1,0,0,0,4,0,1,3,0,2,0,1,0,6,0,1,0,1,6,2,4,1,0,6,1,0,3,0,4,0,0,5,4,3,0,0,3,0,0,0,2,2,0,0,0,0,0,2,4,1,1,0,0,0,5,1,0,0,0,0,5,0,1,0,0,0,3,0,6,5,0,4,0,0\", \"breakableBlockXValues\": \"8,5,6,8,10,3,9,11,1,2,5,6,7,8,9,10,11,13,2,4,6,8,14,0,1,2,3,4,6,7,8,9,10,11,12,13,14,2,4,6,8,12,2,3,4,6,7,8,9,12,2,6,10,12,2,4,6,8,10,12,3,5,6,7,8,9,10,2,4,6,8,10,0,1,2,3,4,5,6,8,9,11,12,13,14,2,8,10,0,1,2,3,6,11,14,2,4,6,8,10,12,2,3,4,5,6,7,8,9,10,12\", \"breakableBlockYValues\": \"0,1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,8,8,9,9,9,9,9,9,10,10,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,14,14,14,14,14,14,14,15,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 0, \"numBreakableBlocks\": 111, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 22.208955764770508, \"playerIndex0Y\": 12.537313461303711, \"playerIndex1\": \"Roy Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 2.1492538452148438, \"playerIndex1Y\": 16.8358211517334, \"playerIndex2\": \"Heather Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 22.208955764770508, \"playerIndex2Y\": 16.8358211517334, \"playerIndex3\": \"Tyler Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 6.447761058807373, \"playerIndex3Y\": 2.507462739944458, \"playerIndex4\": \"Andre Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 22.208955764770508, \"playerIndex4Y\": 25.432836532592773, \"playerIndex5\": \"Dave Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 2.1492538452148438, \"playerIndex5Y\": 12.537313461303711, \"playerIndex6\": \"Debra Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 17.91044807434082, \"playerIndex6Y\": 2.507462739944458, \"playerIndex7\": \"Stephen Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 2.1492538452148438, \"playerIndex7Y\": 25.432836532592773}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
		else if (_beginGameMessagesIndex == 1)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"2,2,1,1,0,0,3,0,1,5,0,0,2,0,3,0,0,0,2,3,0,4,0,3,2,0,2,2,0,0,1,0,3,1,2,1,0,4,0,2,0,0,0,6,3,3,3,2,2,2,2,0,3,5,3,2,0,6,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,2,4,0,3,2,0,1,0,2,0,4,3,0,2,0,3,4,0\", \"breakableBlockXValues\": \"8,5,6,7,8,10,5,9,11,1,2,3,4,6,9,12,13,14,0,2,4,6,8,10,12,14,1,2,3,4,5,6,7,9,10,11,12,2,4,8,3,4,7,10,2,4,6,12,6,10,5,6,7,8,9,10,11,12,4,6,8,1,2,3,4,7,8,10,11,13,14,4,6,8,10,0,1,2,3,7,8,9,10,11,14,4,6,8,10,12,3,6,7,10,11,12\", \"breakableBlockYValues\": \"0,1,1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,6,6,6,7,7,7,7,8,8,8,8,9,9,10,10,10,10,10,10,10,10,11,11,11,12,12,12,12,12,12,12,12,12,12,13,13,13,13,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 1, \"numBreakableBlocks\": 96, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 22.208955764770508, \"playerIndex0Y\": 12.537313461303711, \"playerIndex1\": \"Stephen Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 2.1492538452148438, \"playerIndex1Y\": 25.432836532592773, \"playerIndex2\": \"Barney Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 6.447761058807373, \"playerIndex2Y\": 2.507462739944458, \"playerIndex3\": \"Kyle Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 2.1492538452148438, \"playerIndex3Y\": 12.537313461303711, \"playerIndex4\": \"Andre Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 17.91044807434082, \"playerIndex4Y\": 2.507462739944458, \"playerIndex5\": \"Pier Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 2.1492538452148438, \"playerIndex5Y\": 16.8358211517334, \"playerIndex6\": \"Glen Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 22.208955764770508, \"playerIndex6Y\": 25.432836532592773, \"playerIndex7\": \"Randy Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 22.208955764770508, \"playerIndex7Y\": 16.8358211517334}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
		else if (_beginGameMessagesIndex == 2)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"0,0,0,2,2,0,2,0,4,0,2,0,0,0,0,2,3,5,5,0,0,2,0,0,1,0,2,1,0,1,3,2,5,0,6,0,0,0,0,2,3,0,1,0,4,6,1,0,0,0,1,0,1,0,0,1,3,6,3,0,4,0,0,0,4,0,0,2,0,0,0,5,5,6,0,2,0,5,0,0,1,1,2,0,0,2,0,2,2,0,0,3,0,0,1,3,5,3,0\", \"breakableBlockXValues\": \"6,8,4,7,8,9,10,3,5,7,9,11,0,1,3,6,7,11,12,14,0,2,4,6,12,14,0,1,2,4,6,8,10,11,13,2,8,10,12,3,4,5,7,10,11,6,8,12,2,8,10,5,6,7,10,11,12,4,10,0,1,2,4,5,6,7,8,9,10,11,12,13,2,4,8,10,12,14,0,1,2,3,7,8,9,10,13,14,2,4,5,7,8,12,2,3,5,9,10\", \"breakableBlockYValues\": \"0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,6,6,6,6,7,7,7,7,7,7,8,8,8,9,9,9,10,10,10,10,10,10,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 2, \"numBreakableBlocks\": 99, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 22.208955764770508, \"playerIndex0Y\": 16.8358211517334, \"playerIndex1\": \"Brandi Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 2.1492538452148438, \"playerIndex1Y\": 12.537313461303711, \"playerIndex2\": \"Bradlee Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 2.1492538452148438, \"playerIndex2Y\": 16.8358211517334, \"playerIndex3\": \"Heather Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 22.208955764770508, \"playerIndex3Y\": 12.537313461303711, \"playerIndex4\": \"Matt Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 2.1492538452148438, \"playerIndex4Y\": 25.432836532592773, \"playerIndex5\": \"Glen Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 6.447761058807373, \"playerIndex5Y\": 2.507462739944458, \"playerIndex6\": \"Barney Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 17.91044807434082, \"playerIndex6Y\": 2.507462739944458, \"playerIndex7\": \"Dean Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 22.208955764770508, \"playerIndex7Y\": 25.432836532592773}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
		else if (_beginGameMessagesIndex == 3)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"0,1,0,0,1,1,3,2,3,5,2,1,4,0,3,0,2,2,4,0,0,3,0,0,0,0,5,0,1,0,0,3,0,1,3,0,3,0,2,2,3,3,2,0,1,0,0,1,3,0,0,3,0,0,0,0,0,0,2,3,2,4,0,0,0,0,6,2,0,4,0,1,5,6,5,1,0,3,0,2,0,5,3,5,2,0,0,1,4,0,2,0,0,0,0,1\", \"breakableBlockXValues\": \"6,8,4,5,6,7,8,9,10,3,5,7,11,0,1,2,3,4,5,6,7,9,11,13,12,2,4,5,6,8,9,10,12,6,10,2,3,4,6,7,8,10,11,2,6,8,12,2,4,10,12,2,3,4,7,9,10,2,8,10,12,0,10,11,12,13,14,0,2,4,8,10,12,14,1,2,3,5,6,7,9,10,11,12,13,4,6,8,12,2,3,5,7,8,10,12\", \"breakableBlockYValues\": \"0,0,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,4,5,5,5,5,5,5,5,5,6,6,7,7,7,7,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,10,11,11,11,11,12,12,12,12,12,12,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,16,16,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 0, \"numBreakableBlocks\": 96, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 6.447761058807373, \"playerIndex0Y\": 2.507462739944458, \"playerIndex1\": \"Kenny Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 2.1492538452148438, \"playerIndex1Y\": 25.432836532592773, \"playerIndex2\": \"Wesley Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 22.208955764770508, \"playerIndex2Y\": 12.537313461303711, \"playerIndex3\": \"Tyler Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 17.91044807434082, \"playerIndex3Y\": 2.507462739944458, \"playerIndex4\": \"Morgan Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 22.208955764770508, \"playerIndex4Y\": 16.8358211517334, \"playerIndex5\": \"Paul Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 2.1492538452148438, \"playerIndex5Y\": 12.537313461303711, \"playerIndex6\": \"Lily Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 22.208955764770508, \"playerIndex6Y\": 25.432836532592773, \"playerIndex7\": \"Bruce Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 2.1492538452148438, \"playerIndex7Y\": 16.8358211517334}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
		else if (_beginGameMessagesIndex == 4)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"4,0,5,0,0,6,0,2,0,3,3,0,2,3,3,2,0,5,0,3,0,0,5,5,3,0,0,4,2,2,0,0,0,0,2,3,0,0,0,3,0,3,0,0,0,0,0,2,1,0,1,2,0,0,2,4,0,0,0,0,5,0,0,3,0,2,5,0,4,3,5,0,0,0,0,0,0,6,2,0,3,1,0,3,3,0,0,0,0,0,6,0,0,0,5,1,2\", \"breakableBlockXValues\": \"6,4,6,9,7,9,11,0,1,2,3,7,11,14,0,4,8,12,14,0,1,2,3,5,6,8,10,11,14,2,4,6,8,12,2,5,6,8,9,10,11,12,2,4,10,2,4,6,10,2,6,9,10,11,2,4,6,8,12,0,1,2,3,5,6,7,8,10,12,13,2,4,8,10,12,1,2,3,5,6,7,8,11,12,13,14,4,6,8,10,12,3,4,5,7,8,10\", \"breakableBlockYValues\": \"0,1,1,1,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,8,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 1, \"numBreakableBlocks\": 97, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 2.1492538452148438, \"playerIndex0Y\": 16.8358211517334, \"playerIndex1\": \"Tyler Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 22.208955764770508, \"playerIndex1Y\": 12.537313461303711, \"playerIndex2\": \"Joe Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 17.91044807434082, \"playerIndex2Y\": 2.507462739944458, \"playerIndex3\": \"Lily Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 22.208955764770508, \"playerIndex3Y\": 25.432836532592773, \"playerIndex4\": \"Brandi Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 2.1492538452148438, \"playerIndex4Y\": 25.432836532592773, \"playerIndex5\": \"Kathy Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 6.447761058807373, \"playerIndex5Y\": 2.507462739944458, \"playerIndex6\": \"Dexter Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 22.208955764770508, \"playerIndex6Y\": 16.8358211517334, \"playerIndex7\": \"Paul Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 2.1492538452148438, \"playerIndex7Y\": 12.537313461303711}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
		else if (_beginGameMessagesIndex == 5)
		{
            std::stringstream ss;
            ss << "{\"breakableBlockPowerUpFlags\": \"0,0,0,2,2,4,0,3,1,5,4,0,0,0,0,6,6,0,3,0,0,2,1,4,0,0,3,1,3,0,1,1,3,5,0,0,0,1,2,0,1,6,3,0,5,0,0,0,0,0,1,0,0,3,3,3,1,6,0,0,0,0,4,4,2,0,0,0,0,0,0,0,0,0,0,1,0,6,3,3,0,3,3,0,1,6,2,0,0,3,5,0,0,0,2,1,2,1,1,3,0,5,1,0,5,0,3,0,0,0\", \"breakableBlockXValues\": \"6,8,4,5,6,7,9,5,7,0,3,4,5,7,9,10,11,12,13,14,0,4,6,8,10,12,14,1,4,5,6,9,11,12,13,14,2,4,6,8,10,3,4,6,7,8,9,11,4,6,10,12,2,4,6,8,10,2,3,4,6,7,8,9,10,11,12,4,6,8,10,12,0,3,4,5,6,7,9,10,11,12,13,14,0,2,4,6,10,12,14,1,2,3,7,8,10,12,13,14,2,5,6,12,3,4,5,10,11,12\", \"breakableBlockYValues\": \"0,0,1,1,1,1,1,2,2,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,7,7,8,8,8,8,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,15,15,15,15,16,16,16,16,16,16\", \"eventType\": 1337, \"mapType\": 2, \"numBreakableBlocks\": 110, \"numClientBots\": 7,\"numPlayers\": 1, \"numSecondsLeftForRound\": 180, \"playerIndex0\": \"";
            ss << m_username;
            ss << "\", \"playerIndex0Alive\": true, \"playerIndex0Direction\": 0, \"playerIndex0IsBot\": false, \"playerIndex0X\": 6.447761058807373, \"playerIndex0Y\": 2.507462739944458, \"playerIndex1\": \"Eric Bot\", \"playerIndex1Alive\": true, \"playerIndex1Direction\": 2, \"playerIndex1IsBot\": true, \"playerIndex1X\": 22.208955764770508, \"playerIndex1Y\": 25.432836532592773, \"playerIndex2\": \"Frank Bot\", \"playerIndex2Alive\": true, \"playerIndex2Direction\": 0, \"playerIndex2IsBot\": true, \"playerIndex2X\": 17.91044807434082, \"playerIndex2Y\": 2.507462739944458, \"playerIndex3\": \"James Bot\", \"playerIndex3Alive\": true, \"playerIndex3Direction\": 2, \"playerIndex3IsBot\": true, \"playerIndex3X\": 2.1492538452148438, \"playerIndex3Y\": 12.537313461303711, \"playerIndex4\": \"Ryan Bot\", \"playerIndex4Alive\": true, \"playerIndex4Direction\": 1, \"playerIndex4IsBot\": true, \"playerIndex4X\": 2.1492538452148438, \"playerIndex4Y\": 25.432836532592773, \"playerIndex5\": \"Roy Bot\", \"playerIndex5Alive\": true, \"playerIndex5Direction\": 3, \"playerIndex5IsBot\": true, \"playerIndex5X\": 22.208955764770508, \"playerIndex5Y\": 12.537313461303711, \"playerIndex6\": \"Alfred Bot\", \"playerIndex6Alive\": true, \"playerIndex6Direction\": 1, \"playerIndex6IsBot\": true, \"playerIndex6X\": 2.1492538452148438, \"playerIndex6Y\": 16.8358211517334, \"playerIndex7\": \"Ted Bot\", \"playerIndex7Alive\": true, \"playerIndex7Direction\": 3, \"playerIndex7IsBot\": true, \"playerIndex7X\": 22.208955764770508, \"playerIndex7Y\": 16.8358211517334}";
			std::string beginGameMessageString = ss.str();
			const char *beginGameMessage = beginGameMessageString.c_str();

			m_gameScreen->handleServerUpdate(beginGameMessage);
		}
	}

	void Direct3DInterop::handleDeathForPlayerIndex(int playerIndex)
	{
		m_playersAlive[playerIndex] = false;

		int numAlive = 0;
		for (int i = 0; i < 8; i++)
		{
			if (m_playersAlive[i])
			{
				numAlive++;
			}
		}

		if (numAlive <= 1)
		{
			_isGameOver = true;
		}
	}

	void Direct3DInterop::handleGameOver()
	{
		int numAlive = 0;
		int winningPlayerIndex = -1;
		for (int i = 0; i < 8; i++)
		{
			if (m_playersAlive[i])
			{
				winningPlayerIndex = i;
				numAlive++;
			}
		}

		if (numAlive <= 1)
		{
			bool hasWinner = numAlive == 1;

			std::stringstream ss;
			ss << "{\"eventType\": ";
			ss << GAME_OVER;
			ss << ", \"hasWinner\": ";
			ss << (hasWinner ? "true" : "false");
			ss << ", \"winningPlayerIndex\": ";
			ss << winningPlayerIndex;
			ss << "}";
			std::string gameOverMessageString = ss.str();
			const char *gameOverMessage = gameOverMessageString.c_str();

			m_gameScreen->handleServerUpdate(gameOverMessage);
		}
	}

	void Direct3DInterop::addTouchEventForType(Touch_Type touchType, float x, float y)
	{
		TouchEvent touchEvent = newTouchEvent();
		touchEvent.setTouchType(touchType);
		touchEvent.setX(x);
		touchEvent.setY(y);

		m_touchEventsBuffer.push_back(touchEvent);
	}

	TouchEvent Direct3DInterop::newTouchEvent()
	{
		if (m_touchEventsPool.size() == 0)
		{
			return TouchEvent(0, 0, Touch_Type::DOWN);
		}
		else
		{
			TouchEvent touchEvent = m_touchEventsPool.back();
			m_touchEventsPool.pop_back();
			return touchEvent;
		}
	}
}