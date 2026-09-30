#include "TitleScene.h"

void TitleScene::Initialize() {
	request_ = Request::None;
	showGameOver_ = false;


}


void TitleScene::Update() {
	if (showGameOver_) {
		//option retry
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_R)) {
			showGameOver_ = false;
			request_ = Request::GoToGame;
		}
		//option return to title
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			showGameOver_ = false;
		}
		return;
	}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_R)){
		request_ = Request::GoToGame;
	}
}

void TitleScene::Draw() {
	Sprite::PreDraw();
	Sprite::PostDraw();
}

TitleScene::~TitleScene() {

}
