//
//  MapSearchNode.h
//  bomberparty
//
//  Created by Stephen Gowen on 7/2/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#ifndef __bomberparty__MapSearchNode__
#define __bomberparty__MapSearchNode__

#include "stlastar.h" // See header for copyright and usage information

class MapSearchNode
{
public:
	unsigned int x;	 // the (x,y) positions of the node
	unsigned int y;
	
	MapSearchNode() { x = y = 0; }
	MapSearchNode( unsigned int px, unsigned int py ) { x=px; y=py; }
    
	float GoalDistanceEstimate( MapSearchNode &nodeGoal );
	bool IsGoal( MapSearchNode &nodeGoal );
	bool GetSuccessors( AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node );
	float GetCost( MapSearchNode &successor );
	bool IsSameState( MapSearchNode &rhs );
    
	void PrintNodeInfo();
};

#endif /* defined(__bomberparty__MapSearchNode__) */