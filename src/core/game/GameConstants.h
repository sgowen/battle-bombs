//
//  GameConstants.h
//  battlebombs
//
//  Created by Stephen Gowen on 5/13/14.
//  Copyright (c) 2014 Gowen Game Dev. All rights reserved.
//

#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#define SCREEN_WIDTH 24.0f
#define SCREEN_HEIGHT 13.6119403f
#define WORLD_HEIGHT 28.65671642105267f

#define GAME_X 1.43283582f
#define GAME_Y 1.43283582105267f

#define GRID_CELL_SIZE 1.43283582f

#define GRID_CELL_WIDTH GRID_CELL_SIZE
#define GRID_CELL_HEIGHT GRID_CELL_SIZE

#define GRID_CELL_NUM_ROWS 17
#define NUM_GRID_CELLS_PER_ROW 15

#define PLAYER_FRAME_WIDTH GRID_CELL_WIDTH * 2.0f
#define PLAYER_FRAME_HEIGHT GRID_CELL_HEIGHT * 8.0f / 5.0f

#define BOTTOM_HALF_TOP_GRID_Y 7
#define TOP_HALF_BOTTOM_GRID_Y 10

#define PLAYER_1_GRID_X 3
#define PLAYER_1_GRID_Y 0

#define PLAYER_2_GRID_X NUM_GRID_CELLS_PER_ROW - 1
#define PLAYER_2_GRID_Y BOTTOM_HALF_TOP_GRID_Y

#define PLAYER_3_GRID_X 0
#define PLAYER_3_GRID_Y TOP_HALF_BOTTOM_GRID_Y

#define PLAYER_4_GRID_X NUM_GRID_CELLS_PER_ROW - 1
#define PLAYER_4_GRID_Y GRID_CELL_NUM_ROWS - 1

#define PLAYER_5_GRID_X 11
#define PLAYER_5_GRID_Y 0

#define PLAYER_6_GRID_X 0
#define PLAYER_6_GRID_Y BOTTOM_HALF_TOP_GRID_Y

#define PLAYER_7_GRID_X NUM_GRID_CELLS_PER_ROW - 1
#define PLAYER_7_GRID_Y TOP_HALF_BOTTOM_GRID_Y

#define PLAYER_8_GRID_X 0
#define PLAYER_8_GRID_Y GRID_CELL_NUM_ROWS - 1

#define DIRECTION_RIGHT 0
#define DIRECTION_UP 1
#define DIRECTION_LEFT 2
#define DIRECTION_DOWN 3

#define FRICTION_FACTOR 6.4f

#endif /* GAME_CONSTANTS_H */
