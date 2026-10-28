#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include "KamataEngine.h"
#include "Function.h"

using namespace KamataEngine;

enum class MapChipType {
	kBlank,
	kBlock,
	kSlope1L,	// downward 45 
	kSlope1R,	// upward 45
	kSlope2L,	// downward top 22.5
	kSlope2R,	// upward top 22.5
	kSlope3L,	// downward bottom 22.5
	kSlope3R,	// upward bottom 22.5
	kSlope4L,	// downward curve
	kSlope4R,	// upward curve
	
	kEnemy1,	// normal enemy
	kEnemy2,	// shielded enemy

	//kGoal,		// clear levels
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChip {

public:

	void ResetMapChipData();
	void LoadMapChipCSV(const std::string& filePath);

	uint32_t GetNumBlockVertical() const {return kNumBlockVertical_;}
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal_; }

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	float GetSlopeSurfaceY(uint32_t xIndex, uint32_t yIndex, float worldX);
	//float GetSurfaceY(uint32_t xIndex, uint32_t yIndex, float worldX);

	//bool IsSlopeWallLeft(MapChipType type) const { return type == MapChipType::kSlope1L; }
	//bool IsSlopeWallRight(MapChipType type) const { return type == MapChipType::kSlope1R; }
	bool isHalfSlopeWall(MapChipType type) const { 
		return type == MapChipType::kSlope2L || type == MapChipType::kSlope2R/* ||
		type == MapChipType::kSlope3L || type == MapChipType::kSlope3R*/;
	}
	bool IsInBottomHalf(uint32_t xIndex, uint32_t yIndex, float posY);


	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChip();
	~MapChip();
private:
	MapChipData mapChipData_;

	static inline const float kBlockWidth_ = 2.0f;
	static inline const float kBlockHeight_ = 2.0f;

	static inline const uint32_t kNumBlockVertical_ = 20;
	static inline const uint32_t kNumBlockHorizontal_ = 100;
};

