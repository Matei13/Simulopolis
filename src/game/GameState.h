#pragma once

#include "util/NonCopyable.h"
#include "util/NonMovable.h"
#include "message/Mailbox.h"

class MessageBus;
class RenderEngine;
class InputEngine;
class TextureManager;
class StylesheetManager;
class GuiManager;

class GameState : public NonCopyable, public NonMovable
{
public:
    GameState();
    virtual ~GameState();

    static void setMessageBus(MessageBus* messageBus);
    static void setGameId(Id gameId);
    static void setRenderEngine(RenderEngine* renderEngine);
    static void setInputEngine(InputEngine* inputEngine);
    static void setTextureManager(TextureManager* textureManager);
    static void setStylesheetManager(StylesheetManager* stylesheetManager);
    static void setGuiManager(GuiManager* guiManager);

    virtual void draw(const float dt) = 0;
    virtual void update(const float dt) = 0;
    virtual void handleMessages() = 0;

protected:
    static MessageBus* sMessageBus;
    static Id sGameId;
    static RenderEngine* sRenderEngine;
    static InputEngine* sInputEngine;
    static TextureManager* sTextureManager;
    static StylesheetManager* sStylesheetManager;
    static GuiManager* sGuiManager;

    Mailbox mMailbox;
};
