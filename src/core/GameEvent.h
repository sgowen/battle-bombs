//
//  GameEvent.h
//  bomber-party
//
//  Created by Stephen Gowen on 3/15/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#ifndef GAME_EVENT_H
#define GAME_EVENT_H

// Event Type
#define BEGIN_SPECTATE 1336
#define BEGIN_GAME 1337
#define CLIENT_UPDATE 1338

// Map Type
#define MAP_SPACE 0
#define MAP_GRASSLANDS 1
#define MAP_MOUNTAINS 2
#define MAP_BASE 3

#define PLAYER_EVENT_BASE 100

#define PLAYER_MOVE_RIGHT 1
#define PLAYER_MOVE_UP 2
#define PLAYER_MOVE_LEFT 3
#define PLAYER_MOVE_DOWN 4
#define PLAYER_MOVE_STOP 5
#define PLAYER_PLANT_BOMB 6
#define PLAYER_PUSH_BOMB 7
#define PLAYER_DEATH 9

#define PLAYER_0_MOVE_RIGHT PLAYER_MOVE_RIGHT
#define PLAYER_0_MOVE_UP PLAYER_MOVE_UP
#define PLAYER_0_MOVE_LEFT PLAYER_MOVE_LEFT
#define PLAYER_0_MOVE_DOWN PLAYER_MOVE_DOWN
#define PLAYER_0_MOVE_STOP PLAYER_MOVE_STOP
#define PLAYER_0_PLANT_BOMB PLAYER_PLANT_BOMB
#define PLAYER_0_DEATH PLAYER_DEATH
#define PLAYER_0_PUSH_BOMB PLAYER_PUSH_BOMB

#define PLAYER_1_MOVE_RIGHT PLAYER_EVENT_BASE * 1 + PLAYER_MOVE_RIGHT
#define PLAYER_1_MOVE_UP PLAYER_EVENT_BASE * 1 + PLAYER_MOVE_UP
#define PLAYER_1_MOVE_LEFT PLAYER_EVENT_BASE * 1 + PLAYER_MOVE_LEFT
#define PLAYER_1_MOVE_DOWN PLAYER_EVENT_BASE * 1 + PLAYER_MOVE_DOWN
#define PLAYER_1_MOVE_STOP PLAYER_EVENT_BASE * 1 + PLAYER_MOVE_STOP
#define PLAYER_1_PLANT_BOMB PLAYER_EVENT_BASE * 1 + PLAYER_PLANT_BOMB
#define PLAYER_1_DEATH PLAYER_EVENT_BASE * 1 + PLAYER_DEATH
#define PLAYER_1_PUSH_BOMB PLAYER_EVENT_BASE * 1 + PLAYER_PUSH_BOMB

#define PLAYER_2_MOVE_RIGHT PLAYER_EVENT_BASE * 2 + PLAYER_MOVE_RIGHT
#define PLAYER_2_MOVE_UP PLAYER_EVENT_BASE * 2 + PLAYER_MOVE_UP
#define PLAYER_2_MOVE_LEFT PLAYER_EVENT_BASE * 2 + PLAYER_MOVE_LEFT
#define PLAYER_2_MOVE_DOWN PLAYER_EVENT_BASE * 2 + PLAYER_MOVE_DOWN
#define PLAYER_2_MOVE_STOP PLAYER_EVENT_BASE * 2 + PLAYER_MOVE_STOP
#define PLAYER_2_PLANT_BOMB PLAYER_EVENT_BASE * 2 + PLAYER_PLANT_BOMB
#define PLAYER_2_DEATH PLAYER_EVENT_BASE * 2 + PLAYER_DEATH
#define PLAYER_2_PUSH_BOMB PLAYER_EVENT_BASE * 2 + PLAYER_PUSH_BOMB

#define PLAYER_3_MOVE_RIGHT PLAYER_EVENT_BASE * 3 + PLAYER_MOVE_RIGHT
#define PLAYER_3_MOVE_UP PLAYER_EVENT_BASE * 3 + PLAYER_MOVE_UP
#define PLAYER_3_MOVE_LEFT PLAYER_EVENT_BASE * 3 + PLAYER_MOVE_LEFT
#define PLAYER_3_MOVE_DOWN PLAYER_EVENT_BASE * 3 + PLAYER_MOVE_DOWN
#define PLAYER_3_MOVE_STOP PLAYER_EVENT_BASE * 3 + PLAYER_MOVE_STOP
#define PLAYER_3_PLANT_BOMB PLAYER_EVENT_BASE * 3 + PLAYER_PLANT_BOMB
#define PLAYER_3_DEATH PLAYER_EVENT_BASE * 3 + PLAYER_DEATH
#define PLAYER_3_PUSH_BOMB PLAYER_EVENT_BASE * 3 + PLAYER_PUSH_BOMB

#define PLAYER_4_MOVE_RIGHT PLAYER_EVENT_BASE * 4 + PLAYER_MOVE_RIGHT
#define PLAYER_4_MOVE_UP PLAYER_EVENT_BASE * 4 + PLAYER_MOVE_UP
#define PLAYER_4_MOVE_LEFT PLAYER_EVENT_BASE * 4 + PLAYER_MOVE_LEFT
#define PLAYER_4_MOVE_DOWN PLAYER_EVENT_BASE * 4 + PLAYER_MOVE_DOWN
#define PLAYER_4_MOVE_STOP PLAYER_EVENT_BASE * 4 + PLAYER_MOVE_STOP
#define PLAYER_4_PLANT_BOMB PLAYER_EVENT_BASE * 4 + PLAYER_PLANT_BOMB
#define PLAYER_4_DEATH PLAYER_EVENT_BASE * 4 + PLAYER_DEATH
#define PLAYER_4_PUSH_BOMB PLAYER_EVENT_BASE * 4 + PLAYER_PUSH_BOMB

#define PLAYER_5_MOVE_RIGHT PLAYER_EVENT_BASE * 5 + PLAYER_MOVE_RIGHT
#define PLAYER_5_MOVE_UP PLAYER_EVENT_BASE * 5 + PLAYER_MOVE_UP
#define PLAYER_5_MOVE_LEFT PLAYER_EVENT_BASE * 5 + PLAYER_MOVE_LEFT
#define PLAYER_5_MOVE_DOWN PLAYER_EVENT_BASE * 5 + PLAYER_MOVE_DOWN
#define PLAYER_5_MOVE_STOP PLAYER_EVENT_BASE * 5 + PLAYER_MOVE_STOP
#define PLAYER_5_PLANT_BOMB PLAYER_EVENT_BASE * 5 + PLAYER_PLANT_BOMB
#define PLAYER_5_DEATH PLAYER_EVENT_BASE * 5 + PLAYER_DEATH
#define PLAYER_5_PUSH_BOMB PLAYER_EVENT_BASE * 5 + PLAYER_PUSH_BOMB

#define PLAYER_6_MOVE_RIGHT PLAYER_EVENT_BASE * 6 + PLAYER_MOVE_RIGHT
#define PLAYER_6_MOVE_UP PLAYER_EVENT_BASE * 6 + PLAYER_MOVE_UP
#define PLAYER_6_MOVE_LEFT PLAYER_EVENT_BASE * 6 + PLAYER_MOVE_LEFT
#define PLAYER_6_MOVE_DOWN PLAYER_EVENT_BASE * 6 + PLAYER_MOVE_DOWN
#define PLAYER_6_MOVE_STOP PLAYER_EVENT_BASE * 6 + PLAYER_MOVE_STOP
#define PLAYER_6_PLANT_BOMB PLAYER_EVENT_BASE * 6 + PLAYER_PLANT_BOMB
#define PLAYER_6_DEATH PLAYER_EVENT_BASE * 6 + PLAYER_DEATH
#define PLAYER_6_PUSH_BOMB PLAYER_EVENT_BASE * 6 + PLAYER_PUSH_BOMB

#define PLAYER_7_MOVE_RIGHT PLAYER_EVENT_BASE * 7 + PLAYER_MOVE_RIGHT
#define PLAYER_7_MOVE_UP PLAYER_EVENT_BASE * 7 + PLAYER_MOVE_UP
#define PLAYER_7_MOVE_LEFT PLAYER_EVENT_BASE * 7 + PLAYER_MOVE_LEFT
#define PLAYER_7_MOVE_DOWN PLAYER_EVENT_BASE * 7 + PLAYER_MOVE_DOWN
#define PLAYER_7_MOVE_STOP PLAYER_EVENT_BASE * 7 + PLAYER_MOVE_STOP
#define PLAYER_7_PLANT_BOMB PLAYER_EVENT_BASE * 7 + PLAYER_PLANT_BOMB
#define PLAYER_7_DEATH PLAYER_EVENT_BASE * 7 + PLAYER_DEATH
#define PLAYER_7_PUSH_BOMB PLAYER_EVENT_BASE * 7 + PLAYER_PUSH_BOMB

#endif /* GAME_EVENT_H */
