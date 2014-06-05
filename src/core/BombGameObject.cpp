//
//  BombGameObject.cpp
//  bomberparty
//
//  Created by Stephen Gowen on 4/28/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#include "pch.h"
#include "BombGameObject.h"
#include "Vector2D.h"
#include "Rectangle.h"
#include "PlayerDynamicGameObject.h"
#include "Explosion.h"
#include "InsideBlock.h"
#include "BreakableBlock.h"
#include "OverlapTester.h"

BombGameObject::BombGameObject(PlayerDynamicGameObject *bombOwner, short power, int gridX, int gridY, float width, float height) : DynamicGridGameObject(gridX, gridY, width, height, 0)
{
    m_bombOwner = bombOwner;
    m_fStateTime = 0;
    m_fSizeScalar = 0.2f;
    m_sPower = power;
    m_isExploding = false;
	m_isKicked = false;

    m_bombOwner->onBombDropped();
}

void BombGameObject::update(float deltaTime, std::vector<std::unique_ptr<Explosion >> &explosions, std::vector<std::unique_ptr<InsideBlock >> &insideBlocks, std::vector<std::unique_ptr<BreakableBlock >> &breakableBlocks)
{
    m_fStateTime += deltaTime;

    m_fWidth += deltaTime * m_fSizeScalar;
    m_fHeight = m_fWidth;
    if (m_fWidth > 0.9f || m_fWidth < 0.7f)
    {
        m_fSizeScalar = -m_fSizeScalar;
    }

	if(m_isKicked)
	{
		switch(m_iKickedDirection)
		{
			case DIRECTION_UP :	
				if(!willHitBreakableBlock(breakableBlocks) && !willHitInsideBlock(insideBlocks))
				{
					m_position->add(0, 0.2f);
				}
				else
				{
					m_position->setY(m_position->getY() - (GRID_CELL_HEIGHT - 0.2f));
					m_isKicked = false;
					//Gridlock the bomb again here
				}
				break;
			case DIRECTION_DOWN :
				if(!willHitBreakableBlock(breakableBlocks) && !willHitInsideBlock(insideBlocks))
				{
					m_position->sub(0, 0.2f);
				}
				else
				{
					m_position->setY(m_position->getY() + (GRID_CELL_HEIGHT + 0.2f));
					m_isKicked = false;
					//Gridlock the bomb again here
				}
				break;
			case DIRECTION_RIGHT :
				if(!willHitBreakableBlock(breakableBlocks) && !willHitInsideBlock(insideBlocks))
				{
					m_position->add(0.2f, 0);
				}
				else
				{
					m_position->setX(m_position->getX() - (GRID_CELL_WIDTH - 0.2f));
					m_isKicked = false;
					//Gridlock the bomb again here
				}
				break;
			case DIRECTION_LEFT :
				if(!willHitBreakableBlock(breakableBlocks) && !willHitInsideBlock(insideBlocks))
				{
					m_position->sub(0.2f, 0);
				}
				else
				{
					m_position->setX(m_position->getX() + (GRID_CELL_WIDTH + 0.2f));
					m_isKicked = false;
					//Gridlock the bomb again here
				}
				break;
		}
	}

    if (m_fStateTime > 3)
    {
        m_isExploding = true;
        m_bombOwner->onBombExploded();

        explosions.push_back(std::unique_ptr<Explosion>(new Explosion(Explosion_Type::CORE, m_position->getX(), m_position->getY(), 0)));

        Vector2D rightPosition = Vector2D(m_position->getX(), m_position->getY());
        Vector2D topPosition = Vector2D(m_position->getX(), m_position->getY());
        Vector2D leftPosition = Vector2D(m_position->getX(), m_position->getY());
        Vector2D bottomPosition = Vector2D(m_position->getX(), m_position->getY());

        bool canExpandRight = true;
        bool canExpandTop = true;
        bool canExpandLeft = true;
        bool canExpandBottom = true;

        for (short i = m_sPower; i > 0; i--)
        {
            rightPosition.add(1, 0);
            topPosition.add(0, 1);
            leftPosition.sub(1, 0);
            bottomPosition.sub(0, 1);

            Explosion_Type explosionType = i == 1 ? Explosion_Type::TAIL : Explosion_Type::BODY;

            if (canExpandRight && !canExplodeAtPosition(rightPosition, insideBlocks))
            {
                canExpandRight = false;
            }
            if (canExpandTop && !canExplodeAtPosition(topPosition, insideBlocks))
            {
                canExpandTop = false;
            }
            if (canExpandLeft && !canExplodeAtPosition(leftPosition, insideBlocks))
            {
                canExpandLeft = false;
            }
            if (canExpandBottom && !canExplodeAtPosition(bottomPosition, insideBlocks))
            {
                canExpandBottom = false;
            }

            if (canExpandRight && willDestroyBlockAtPosition(rightPosition, breakableBlocks))
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, rightPosition.getX(), rightPosition.getY(), 0)));
                canExpandRight = false;
            }
            if (canExpandTop && willDestroyBlockAtPosition(topPosition, breakableBlocks))
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, topPosition.getX(), topPosition.getY(), 90)));
                canExpandTop = false;
            }
            if (canExpandLeft && willDestroyBlockAtPosition(leftPosition, breakableBlocks))
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, leftPosition.getX(), leftPosition.getY(), 180)));
                canExpandLeft = false;
            }
            if (canExpandBottom && willDestroyBlockAtPosition(bottomPosition, breakableBlocks))
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, bottomPosition.getX(), bottomPosition.getY(), 270)));
                canExpandBottom = false;
            }

            if (canExpandRight)
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, rightPosition.getX(), rightPosition.getY(), 0)));
            }
            if (canExpandTop)
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, topPosition.getX(), topPosition.getY(), 90)));
            }
            if (canExpandLeft)
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, leftPosition.getX(), leftPosition.getY(), 180)));
            }
            if (canExpandBottom)
            {
                explosions.push_back(std::unique_ptr<Explosion>(new Explosion(explosionType, bottomPosition.getX(), bottomPosition.getY(), 270)));
            }
        }
    }
}

float BombGameObject::getStateTime()
{
    return m_fStateTime;
}

float BombGameObject::isExploding()
{
    return m_isExploding;
}

void BombGameObject::kicked(int direction)
{
	m_isKicked = true;
	m_iKickedDirection = direction;
}

#pragma mark <Private>

bool BombGameObject::canExplodeAtPosition(Vector2D &position, std::vector<std::unique_ptr<InsideBlock >> &insideBlocks)
{
    for (std::vector < std::unique_ptr < InsideBlock >> ::iterator itr = insideBlocks.begin(); itr != insideBlocks.end(); itr++)
    {
        if (OverlapTester::isPointInRectangle(position, (*itr)->getBounds()))
        {
            return false;
        }
    }

    return true;
}

bool BombGameObject::willDestroyBlockAtPosition(Vector2D &position, std::vector<std::unique_ptr<BreakableBlock >> &breakableBlocks)
{
    for (std::vector < std::unique_ptr < BreakableBlock >> ::iterator itr = breakableBlocks.begin(); itr != breakableBlocks.end(); itr++)
    {
        if (OverlapTester::isPointInRectangle(position, (*itr)->getBounds()))
        {
            // Destroy the block
            (*itr)->onDestroy();
            return true;
        }
    }

    return false;
}

bool BombGameObject::willHitBreakableBlock(std::vector<std::unique_ptr<BreakableBlock >> &breakableBlocks)
{
	for(std::vector < std::unique_ptr < BreakableBlock >> ::iterator itr = breakableBlocks.begin(); itr != breakableBlocks.end(); itr++)
	{
		if (OverlapTester::isPointInRectangle(*m_position, (*itr)->getBounds()))
		{
			//m_isKicked = false;
			return true;
		}
	}

	return false;
}

bool BombGameObject::willHitInsideBlock(std::vector<std::unique_ptr<InsideBlock >> &insideBlocks)
{
	for(std::vector<std::unique_ptr<InsideBlock>>::iterator itr = insideBlocks.begin(); itr != insideBlocks.end(); itr++)
	{
		if (OverlapTester::isPointInRectangle(*m_position, (*itr)->getBounds()))
		{
			//m_isKicked = false;
			return true;
		}
	}

	return false;
}