#include "GameScene.h"
#include "IScene.h"
#include "TitleScene.h"
#include "KamataEngine.h"
#include <Windows.h>

#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

enum class SceneId {
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
};
SceneId scene = SceneId::kUnknown;

void ChangeScene();

void UpdateScene();

void DrawScene();

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LE2C_25_ファルコン_エブラハム_GameTitle");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//TitleScene* titleScene = new TitleScene();
	//GameScene* gameScene = nullptr;

	titleScene = new TitleScene();
	titleScene->Initialize();
	scene = SceneId::kTitle;

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		ImGuiManager* imguiManager = ImGuiManager::GetInstance();
		imguiManager->Begin();

		ChangeScene();
		UpdateScene();

		imguiManager->End();

		///描画開始
		dxCommon->PreDraw();

		/// ゲームシーンの描画

		DrawScene();
		AxisIndicator::GetInstance()->Draw();

		/// 描画終了
		dxCommon->PostDraw();

		//
		/// 解放処理
		//
	}

	delete gameScene;
	delete titleScene;

	KamataEngine::Finalize();

	return 0;
}

void ChangeScene() {

	switch (scene) {
	case SceneId::kTitle:
		//titleScene->Update();

		if (titleScene->GetRequest() == IScene::Request::GoToGame) {
			titleScene->ClearRequest();
			titleScene->StopBGM();
			

			// Create a fresh GameScene
			delete gameScene;
			gameScene = new GameScene();
			gameScene->Initialize();
			scene = SceneId::kGame;
			
		}
		break;

	case SceneId::kGame:
		//gameScene->Update();

		// Manual retry with R
		if (Input::GetInstance()->TriggerKey(DIK_P)) {
			
			//titleScene->StopBGM();
			gameScene->IsFinished();

			//delete gameScene;
			//gameScene = nullptr;
			//
			//titleScene = new TitleScene();
			//titleScene->Initialize();
			//scene = SceneId::kTitle;
			
			
		}

		// Finished (clear or death)
		if (gameScene->IsFinished()) {
			bool cleared = gameScene->IsCleared();

			delete gameScene;
			gameScene = nullptr;

			if (cleared) {
				titleScene->SetShowGameOver(false);	// Stage Clear
			}
			else {
				titleScene->SetShowGameOver(true);	// Game Over
			}

			// Return to title and play BGM
			titleScene->PlayBGM();
			scene = SceneId::kTitle;
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case SceneId::kTitle:
		titleScene->Update();
		break;

	case SceneId::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case SceneId::kTitle:
		titleScene->Draw();
		break;

	case SceneId::kGame:
		gameScene->Draw();
		break;
	}
}