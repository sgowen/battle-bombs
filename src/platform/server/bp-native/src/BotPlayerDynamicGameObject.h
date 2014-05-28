//
//  BotPlayerDynamicGameObject.h
//  bomberparty
//
//  Created by Stephen Gowen on 5/28/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#ifndef __bomberparty__BotPlayerDynamicGameObject__
#define __bomberparty__BotPlayerDynamicGameObject__

#include "PlayerDynamicGameObject.h"

class BotPlayerDynamicGameObject : public PlayerDynamicGameObject
{
public:
    BotPlayerDynamicGameObject(float x, float y, SoundListener *soundListener, int direction = DIRECTION_RIGHT, float width = PLAYER_WIDTH, float height = PLAYER_HEIGHT);
    
    virtual void update(float deltaTime, std::vector<std::unique_ptr<InsideBlock >> &insideBlocks, std::vector<std::unique_ptr<BreakableBlock >> &breakableBlocks, std::vector<std::unique_ptr<PowerUp >> &powerUps);
};

#endif /* defined(__bomberparty__BotPlayerDynamicGameObject__) */