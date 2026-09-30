#pragma once
#include "KamataEngine.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <numbers>
#include <cstdint>
#include "MapChip.h"
#include "Player.h"
//#include "WorldUpdate.h"
//#include "Sky.h"
#include "Enemy.h"
#include <vector>
#include <memory>


using namespace KamataEngine;


const int kWindowWidth = 1280;
const int kWindowHeight = 720;

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; } // true → game over
	void ClearFinished() { finished_ = false; }

	GameScene();
	~GameScene();

private:


	bool finished_ = false;

};

