#pragma once
#include "IScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene : public IScene {

public:
	void Initialize();
	void Update();
	void Draw();

	Request GetRequest() const override { return request_; }
	void ClearRequest() override { request_ = Request::None; }

	void SetShowGameOver(bool v) { showGameOver_ = v; }

	TitleScene();
	~TitleScene();

private:
	Request request_ = Request::None;
	bool showGameOver_ = false;

	uint32_t voiceBGM_ = 0;
	uint32_t soundBGM_ = 0;
	uint32_t levelStartSoundHandle_ = 0;

	uint32_t titleSprite_ = 0;
	Sprite* titleBackground_ = nullptr;

};

