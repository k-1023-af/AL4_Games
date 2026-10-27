#pragma once
#include "KamataEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

using namespace KamataEngine;

struct PlayerScore {
	//std::string name;
	//int score = 0;
	//int distance = 0;
	//int time = 0;
};

class GameClear { // "Leaderboard.csv"に保存する
private:

	//std::vector<PlayerScore> scores;


	//uint32_t textBoxHandle_ = 0;
	//Sprite* leaderboard_ = nullptr;

	uint32_t numberHandles_[13] = {};
	static const int kTimeDigitCount = 8;
	Sprite* timeSprites_[kTimeDigitCount] = {};

	uint32_t globalTimer_ = 0;
	int playerTimer_ = 0;

	//static const int kMaxDisplayEntries = 8;

public:
	GameClear();
	~GameClear();

	void Initialize();
	void Update();
	void Draw();

	//void DrawLeaderboard(float startX, float startY, float lineHeight = 40.0f, float digitWidth = 28.0f);
	void DrawNumber(int value, float x, float y, float digitWidth, int digits = 0);
	void DrawTime(int globalTimer, float startX, float startY, float digitWidth = 32.0f);

	void addScore(int score, int time);

	//void display() const {
	//	for (const auto& player : scores) {
	//		std::cout <<
	//			" score:" << player.score <<
	//			//" dist:" << player.distance << 
	//			" time:" << player.time << std::endl;
	//	}
	//}

	//void SaveLeaderboard(const std::string& filename);

	//void LoadLeaderboard(const std::string& filename);

	//// Sort the leaderboard in descending order based on score
	//void SortLeaderboard() {
	//	std::sort(scores.begin(), scores.end(), [](const PlayerScore& a, const PlayerScore& b) {
	//		return a.score > b.score; // Sort in descending order
	//		});
	//}

	// Get the top N scores from the leaderboard
	//std::vector<PlayerScore> GetTopScores(size_t topN) const {
	//	std::vector<PlayerScore> topScores;
	//	if (topN > scores.size()) {
	//		topN = scores.size();
	//	}
	//	topScores.insert(topScores.end(), scores.begin(), scores.begin() + topN);
	//	return topScores;
	//}
	//
	//// place this code where the player game overs and you want to save the score
	//void OnGameOver(int playerScore, int playerTime, const std::string& filename) {
	//	addScore(playerScore, playerTime);
	//	SortLeaderboard();
	//	SaveLeaderboard(filename);
	//}

	uint32_t GetGlobalTimer() const { return globalTimer_; }

};