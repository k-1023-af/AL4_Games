#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Function.h"
#include "WorldUpdater.h"
#include <algorithm>


class PlayerHUD
{
public:
	void Initialize();
	void Update(float pMeter, bool canAirDash);
	void Draw();

	PlayerHUD();
	~PlayerHUD();
private:
	Sprite* pMeterBar_ = nullptr;
	Sprite* pMeterFill_ = nullptr;

	uint32_t barTextureHandle_ = 0;
	uint32_t fillTextureHandle_ = 0;

	const float kBarWidth	= 250.0f;
	const float kBarHeight	= 32.0f;
	const float kMaxPMeter	= 2.0f;

	float pulseTimer_ = 0.0f;
};

