#include "game/GameStateSettings.h"
#include "message/MessageBus.h"
#include "render/RenderEngine.h"
#include "input/InputEvent.h"
#include "audio/AudioEngine.h"
#include "resource/GuiManager.h"
#include "gui/Gui.h"
#include "gui/GuiButton.h"
#include "gui/GuiText.h"
#include "gui/GuiEvent.h"
#include "util/format.h"

GameStateSettings::GameStateSettings() : mGui(sGuiManager->getGui("settings")),
    mFullscreen(false), mMute(false)
{
    // Gui
    createGui();
}

GameStateSettings::~GameStateSettings()
{
    mGui->get("fullscreenButton")->unsubscribe(mMailbox.getId());
    mGui->get("musicButton")->unsubscribe(mMailbox.getId());
}

void GameStateSettings::enter()
{
    mGui->handleMessages();
    // Subscribe to inputs
    mGui->subscribe(mMailbox.getId());
}

void GameStateSettings::handleMessages()
{
    mGui->handleMessages();
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::INPUT)
        {
            const InputEvent& event = message.getInfo<InputEvent>();
            switch (event.type)
            {
                case sf::Event::Closed:
                    sRenderEngine->closeWindow();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                        sMessageBus->send(Message::create(sGameId, MessageType::GAME, Event(Event::Type::OPEN_MENU)));
                    break;
                default:
                    break;
            }
        }
        if (message.type == MessageType::GUI)
        {
            const GuiEvent& event = message.getInfo<GuiEvent>();
            switch (event.type)
            {
                case GuiEvent::Type::BUTTON_RELEASED:
                {
                    const std::string& name = event.widget->getName();
                    if (name == "fullscreenButton")
                    {
                        mFullscreen = !mFullscreen;
                        if (mFullscreen)
                            mGui->get<GuiText>("fullscreenText")->setString("Fullscreen: on");
                        else
                            mGui->get<GuiText>("fullscreenText")->setString("Fullscreen: off");
                        sRenderEngine->setFullscreen(mFullscreen);
                    }
                    else if (name == "musicButton")
                    {
                        mMute = !mMute;
                        if (mMute)
                            mGui->get<GuiText>("musicText")->setString("Music: off");
                        else
                            mGui->get<GuiText>("musicText")->setString("Music: on");
                        sAudioEngine->setVolume(!mMute * 100.0f);
                    }
                }
                default:
                    break;
            }
        }
    }
}

void GameStateSettings::update(float dt)
{
    sAudioEngine->update();
    mGui->update();
}

void GameStateSettings::draw()
{
    sRenderEngine->draw(*mGui);
}

void GameStateSettings::exit()
{
    // Unsubscribe to inputs
    mGui->unsubscribe(mMailbox.getId());
}

void GameStateSettings::createGui()
{
    mGui->setViewportSize(sRenderEngine->getViewportSize());

    // Register to events
    mGui->get("fullscreenButton")->subscribe(mMailbox.getId());
    mGui->get("musicButton")->subscribe(mMailbox.getId());
}