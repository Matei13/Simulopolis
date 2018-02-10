#include "Game.h"
#include "GameState.h"
#include "GameStateStart.h"
#include "GameStateEditor.h"

Game::Game()
{
    // Register mailbox
    mMessageBus.addMailbox(mMailbox);

    // Load resources
    mResourceManager.setUp();
    Map::loadTiles(mResourceManager.getTextureManager());

    // Push dependencies
    GameState::setMessageBus(&mMessageBus);
    GameState::setGameId(mMailbox.getId());
    GameState::setRenderEngine(&mRenderEngine);
    GameState::setTextureManager(&mResourceManager.getTextureManager());
    GameState::setStylesheetManager(&mResourceManager.getStylesheetManager());

    // Add the start state
    pushState(new GameStateStart());
}

Game::~Game()
{
    while (!mStates.empty())
        popState();

    // Free resources
    mResourceManager.tearDown();
}

void Game::pushState(GameState* state)
{
    mStates.push(state);
}

void Game::popState()
{
    delete mStates.top();
    mStates.pop();
}

void Game::changeState(GameState* state)
{
    if (!mStates.empty())
        popState();
    pushState(state);
}

GameState* Game::peekState()
{
    if (mStates.empty())
        return nullptr;
    return mStates.top();
}

void Game::run()
{
    sf::Clock clock;

    while (mRenderEngine.isWindowOpen())
    {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        GameState* curState = peekState();
        if (curState != nullptr)
        {
            curState->handleInput();
            curState->update(dt);
            curState->draw(dt);
            mRenderEngine.display();
            handleMessages();
        }
    }
}

void Game::handleMessages()
{
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::PUSH_GAME_STATE)
        {
            if (message.getInfo<GameStateName>() == GameStateName::EDITOR)
                pushState(new GameStateEditor());
        }
    }
}
