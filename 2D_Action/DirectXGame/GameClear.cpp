#include "GameClear.h"

GameClear::GameClear() {}

GameClear::~GameClear() {
	for (int i = 0; i < kTimeDigitCount; ++i) {
		delete timeSprites_[i];
		timeSprites_[i] = nullptr;
	}
	//delete leaderboard_;
	//leaderboard_ = nullptr;
}

void GameClear::Initialize() {

	//textBoxHandle_ = TextureManager::Load("./Resources/Leaderboard.png");
	//leaderboard_ = Sprite::Create(textBoxHandle_, { 0.0f, 0.0f });

	numberHandles_[0] = TextureManager::Load("./Resources/numbers/0.png");
	numberHandles_[1] = TextureManager::Load("./Resources/numbers/1.png");
	numberHandles_[2] = TextureManager::Load("./Resources/numbers/2.png");
	numberHandles_[3] = TextureManager::Load("./Resources/numbers/3.png");
	numberHandles_[4] = TextureManager::Load("./Resources/numbers/4.png");
	numberHandles_[5] = TextureManager::Load("./Resources/numbers/5.png");
	numberHandles_[6] = TextureManager::Load("./Resources/numbers/6.png");
	numberHandles_[7] = TextureManager::Load("./Resources/numbers/7.png");
	numberHandles_[8] = TextureManager::Load("./Resources/numbers/8.png");
	numberHandles_[9] = TextureManager::Load("./Resources/numbers/9.png");
	numberHandles_[10] = TextureManager::Load("./Resources/numbers/#.png"); //index10
	numberHandles_[11] = TextureManager::Load("./Resources/numbers/Colon.png");  // index11
	numberHandles_[12] = TextureManager::Load("./Resources/numbers/Period.png"); // index12

	for (int i = 0; i < kTimeDigitCount; ++i) {
		timeSprites_[i] = Sprite::Create(numberHandles_[0], { 0.0f, 0.0f });
	}
}

void GameClear::Update() {
	globalTimer_++;

}

void GameClear::Draw() {

	Sprite::PreDraw();
	//if (leaderboard_) {
	//	leaderboard_->Draw();
	//}

	DrawTime(globalTimer_, 100.0f, 30.0f, 28.0f);
	//DrawLeaderboard(150.0f, 150.0f, 47.0f, 28.0f);

	Sprite::PostDraw();
}

//void GameClear::DrawLeaderboard(float startX, float startY, float lineHeight, float digitWidth) {
//	std::sort(scores.begin(), scores.end(),
//		[](const PlayerScore& a, const PlayerScore& b) {
//			return a.score > b.score; // Sort in descending order
//		});
//
//	const size_t count = (std::min)(scores.size(), static_cast<size_t>(kMaxDisplayEntries));
//
//	for (size_t i = 0; i < count; ++i) {
//		const PlayerScore& entry = scores[i];
//		float y = startY + static_cast<int>(i) * lineHeight;
//
//		// Rank (1-based)
//		DrawNumber(static_cast<int>(i + 1), startX, y, digitWidth, 2);
//
//		// Score
//		DrawNumber(entry.score, startX + 240.0f, y, digitWidth);
//
//		// Distance
//		//DrawNumber(entry.distance, startX + 520.0f, y, digitWidth);
//
//		// Time
//		DrawTime(entry.time, startX + 740.0f, y, digitWidth);
//	}
//}

void GameClear::DrawNumber(int value, float x, float y, float digitWidth, int digits) {

	char buf[16];
	if (digits > 0)
		std::snprintf(buf, sizeof(buf), "%0*d", digits, value);
	else
		std::snprintf(buf, sizeof(buf), "%d", value);

	for (char* p = buf; *p; ++p) {
		if (*p >= '0' && *p <= '9') {
			uint32_t handle = numberHandles_[*p - '0'];
			Sprite* s = Sprite::Create(handle, { x, y });
			s->Draw();
			delete s;
			x += digitWidth;
		}
	}
}

void GameClear::DrawTime(int globalTimer, float startX, float startY, float digitWidth) {
	// 1. format globalTimer into minutes, seconds, milliseconds
	long long totalMs = (static_cast<long long>(globalTimer) * 1000) / 60;
	int minutes = static_cast<int>(totalMs / 60000);
	int seconds = static_cast<int>((totalMs % 60000) / 1000);
	int milliseconds = static_cast<int>(totalMs % 1000);

	char buffer[16];
	std::snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, milliseconds);

	// 2. Draw numbers as Sprites
	float x = startX;
	for (int i = 0; i < kTimeDigitCount; ++i) {

		char c = buffer[i];
		uint32_t handle = 0;

		if (c >= '0' && c <= '9')
			handle = numberHandles_[c - '0'];
		else if (c == ':')
			handle = numberHandles_[11];
		else if (c == '.')
			handle = numberHandles_[12];
		if (handle != 0 && timeSprites_[i]) {
			timeSprites_[i]->SetTextureHandle(handle);
			timeSprites_[i]->SetPosition({ x, startY });
			timeSprites_[i]->Draw();
		}
		x += digitWidth;
	}
}

//void GameClear::addScore(int score, int time) {
//	scores.push_back({ score, time });
//}

//void GameClear::SaveLeaderboard(const std::string& filename) {
//
//	if (scores.empty()) {
//		std::cerr << "SaveLeaderboard: scores is empty – refusing to overwrite file\n";
//		return;
//	}
//
//	std::vector<PlayerScore> sorted = scores;
//	std::sort(sorted.begin(), sorted.end(), [](const PlayerScore& a, const PlayerScore& b) { return a.score > b.score; });
//
//	std::ofstream file(filename);
//	if (!file.is_open()) {
//		std::cerr << "Failed to open file for writing: " << filename << std::endl;
//		return;
//	}
//
//	// Optional header row
//	file << "Rank,Score,Time\n";
//
//	for (size_t i = 0; i < sorted.size(); ++i) {
//		const auto& p = sorted[i];
//		// Rank starts at 1
//		file << (i + 1) << "," << p.score << "," << p.time << "\n";
//	}
//
//	file.close();
//	std::cout << "Leaderboard saved (" << sorted.size() << "entries)\n";
//}

//void GameClear::LoadLeaderboard(const std::string& filename) {
//	std::ifstream file(filename);
//	if (!file.is_open()) {
//		std::cerr << "Failed to open file for reading: " << filename << std::endl;
//		return;
//	}
//
//	std::vector<PlayerScore> loaded;
//
//	std::string line;
//
//	// Skip header if present
//	if (!std::getline(file, line)) {
//		file.close();
//		return;
//	}
//
//	if (!line.empty() && !std::isdigit(static_cast<unsigned char>(line[0]))) {
//	}
//	else {
//		file.seekg(0);
//	}
//
//	while (std::getline(file, line)) {
//		if (line.empty())
//			continue;
//
//		std::stringstream ss(line);
//		std::string rankStr, scoreStr, distStr, timeStr;
//
//		// Read the 4 cells separated by commas
//		if (!std::getline(ss, rankStr, ','))
//			continue;
//		if (!std::getline(ss, scoreStr, ','))
//			continue;
//		/*if (!std::getline(ss, distStr, ','))
//			continue;*/
//		if (!std::getline(ss, timeStr, ','))
//			continue;
//
//		try {
//			PlayerScore entry;
//			// entry.name = name;
//			entry.score = std::stoi(scoreStr);
//			//entry.distance = std::stoi(distStr);
//			entry.time = std::stoi(timeStr);
//
//			loaded.push_back(entry);
//		}
//		catch (const std::exception&) {
//			std::cerr << "Skipping invalid line: " << line << std::endl;
//		}
//	}
//
//	file.close();
//
//	if (!loaded.empty()) {
//		scores = std::move(loaded);
//		std::cout << "Leaderboard loaded (" << scores.size() << " entries)\n";
//	}
//	else {
//		std::cerr << "LoadLeaderboard: no valid entries found – keeping existing data\n";
//	}
//}



