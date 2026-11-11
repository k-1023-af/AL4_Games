#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace KamataEngine;

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> mapData;
};

class MapChip3D {

public:
	MapChip3D();
	~MapChip3D();

	//	左　右　表面　裏面
	struct Rect { float left;  float right; float front; float back; };

	struct IndexSet {
		uint32_t xIndex;
		//uint32_t yIndex;
		uint32_t zIndex;
	};

	//////////////////////////////////////////////////////////////
	//  Getters
	//////////////////////////////////////////////////////////////
	Rect GetRectByIndex(uint32_t xIndex, uint32_t zIndex);
	IndexSet GetMapChipIndexByPosition(const Vector3& position);			// return Index
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex);	// return Position
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex);	// return MapChipType

	uint32_t GetNumBlockXAxis() const { return kNumBlockXAxis_; }
	//uint32_t GetNumBlockYAxis() const { return kNumBlockYAxis_; }
	uint32_t GetNumBlockZAxis() const { return kNumBlockZAxis_; }

	///////////////////////////////////////////////////////////////
	//  関数　Functions
	///////////////////////////////////////////////////////////////
	void ResetMapChipData();
	void LoadMapChipData(const std::string& filePath);

private:

	MapChipData mapChipData_;

	//  ブロック
	static inline const float kBlockWidth_ = 1.0f; // Width of a single block
	static inline const float kBlockHeight_ = 1.0f; // Height of a single block
	static inline const float kBlockDepth_ = 1.0f;  // Depth of a single block
	//  ブロックの軸
	static inline const uint32_t kNumBlockXAxis_ = 20; // Number of blocks along the X-axis
	static inline const uint32_t kNumBlockZAxis_ = 20; // Number of blocks along the Z-axis


};

