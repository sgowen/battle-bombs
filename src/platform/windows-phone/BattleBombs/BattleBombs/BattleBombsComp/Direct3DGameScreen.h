//
//  Direct3DGameScreen.h
//  battlebombs
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2014 Techne Games. All rights reserved.
//

#pragma once

#include "GameScreen.h"
#include "Helpers\MediaEnginePlayer.h"

class Direct3DGameScreen : public GameScreen
{
public:
	Direct3DGameScreen(const char *username, bool isOffline = false);

	void load(float deviceScreenWidth, float deviceScreenHeight, int deviceScreenDpWidth, int deviceScreenDpHeight);

	void handleSound();

	void handleMusic();

	void unload();

	ID3D11Texture2D* getTexture();

	virtual void touchToWorld(TouchEvent &touchEvent);

	virtual void platformResume();

	virtual void platformPause();

	virtual bool handleOnBackPressed();

private:
	std::unique_ptr<MediaEnginePlayer> m_mediaPlayer;

	D3D_FEATURE_LEVEL m_featureLevel;

	float m_fGameScreenToDeviceScreenWidthRatio;
	float m_fGameScreenToDeviceScreenHeightRatio;
	float m_fDipToPixelRatio;
};