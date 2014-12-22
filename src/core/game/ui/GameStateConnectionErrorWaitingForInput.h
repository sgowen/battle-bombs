//
//  GameStateConnectionErrorWaitingForInput.h
//  battlebombs
//
//  Created by Stephen Gowen on 12/22/14.
//  Copyright (c) 2014 Gowen Game Dev. All rights reserved.
//

#ifndef __battlebombs__GameStateConnectionErrorWaitingForInput__
#define __battlebombs__GameStateConnectionErrorWaitingForInput__

#include "GameState.h"

class GameStateConnectionErrorWaitingForInput : public GameState
{
public:
    virtual void update(GameScreen *gameScreen, float deltaTime);
    
    virtual void processServerMessage(GameScreen *gameScreen, rapidjson::Document &d, int eventType);
    
    virtual void updateInput(GameScreen *gameScreen, std::vector<TouchEvent> &touchEvents);
    
    virtual void present(GameScreen *gameScreen);
};

#endif /* defined(__battlebombs__GameStateConnectionErrorWaitingForInput__) */