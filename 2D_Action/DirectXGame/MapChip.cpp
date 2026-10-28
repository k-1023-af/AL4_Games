#include "MapChip.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>

namespace {
    std::map<std::string, MapChipType> mapChipTable = {
        {"-", MapChipType::kBlank},
        {"0", MapChipType::kBlock},
        {"1L", MapChipType::kSlope1L},
        {"1R", MapChipType::kSlope1R},
        {"2L", MapChipType::kSlope2L},
        {"2R", MapChipType::kSlope2R},
        {"3L", MapChipType::kSlope3L},
        {"3R", MapChipType::kSlope3R},
        {"4L", MapChipType::kSlope4L},
        {"4R", MapChipType::kSlope4R},

        // Enemies
        {"E1", MapChipType::kEnemy1},	// Normal
        {"E2", MapChipType::kEnemy2},	// Shielded

        //{"G",  MapChipType::kGoal},
    };
}

void MapChip::ResetMapChipData() {
    mapChipData_.data.clear();
    mapChipData_.data.resize(kNumBlockVertical_);
    for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
        mapChipDataLine.resize(kNumBlockHorizontal_);
    }
}

void MapChip::LoadMapChipCSV(const std::string& filePath) {
    ResetMapChipData();

    std::ifstream file;
    file.open(filePath);
    assert(file.is_open());

    std::stringstream mapChipCsv;
    mapChipCsv << file.rdbuf();
    file.close();

    for (uint32_t i = 0; i < kNumBlockVertical_; ++i) {
        std::string line;
        getline(mapChipCsv, line);

        std::istringstream lineStream(line);

        for (uint32_t j = 0; j < kNumBlockHorizontal_; ++j) {
            std::string word;
            std::getline(lineStream, word, ',');

            if (mapChipTable.contains(word)) {
                mapChipData_.data[i][j] = mapChipTable[word];
            }
        }
    }
}

MapChipType MapChip::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
    if (xIndex < 0 || kNumBlockHorizontal_ - 1 < xIndex) {
        return MapChipType::kBlank;
    }
    if (yIndex < 0 || kNumBlockVertical_ - 1 < yIndex) {
        return MapChipType::kBlank;
    }

    return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChip::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
    return Vector3(
        kBlockWidth_ * xIndex,
        kBlockHeight_ * (kNumBlockVertical_ - 1 - yIndex),
        0.0f);
}

//float MapChip::GetSurfaceY(uint32_t xIndex, uint32_t yIndex, float worldX) {
//    MapChipType type = GetMapChipTypeByIndex(xIndex, yIndex);
//    Rect rect = GetRectByIndex(xIndex, yIndex);
//
//    switch (type) {
//    case MapChipType::kBlock:
//        return rect.top - 0.05f;                          // flat top
//
//    case MapChipType::kSlope1L:{
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.top + 0.4f, rect.bottom + 0.2f, t);    // left high → right low
//    }
//    case MapChipType::kSlope2L:{
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.top + 0.22f, rect.bottom + 0.9f, t);    // left high → right low
//    }
//    case MapChipType::kSlope3L: {
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.top - 0.85f, rect.bottom + 0.2f, t);    // left high → right low
//    }
//    // right slopes
//    case MapChipType::kSlope1R: {
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.bottom + 0.2f, rect.top + 0.4f, t);    // left low → right high
//    }
//    case MapChipType::kSlope2R: {
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.bottom + 0.9f, rect.top + 0.25f, t);    // left low → right high
//    }
// case MapChipType::kSlope3R: {
//        float t = std::clamp((worldX - rect.left) / kBlockWidth_, 0.0f, 1.0f);
//        return Lerp(rect.bottom + 0.2f, rect.top - 0.85f, t);    // left low → right high
//    }
//
//    default:
//        return -10000.0f;                         // not solid
//    }
//}

float MapChip::GetSlopeSurfaceY(uint32_t xIndex, uint32_t yIndex, float worldX) {
   MapChipType type = GetMapChipTypeByIndex(xIndex, yIndex);
	Rect rect = GetRectByIndex(xIndex, yIndex);

	float L = (worldX - rect.left) / (kBlockWidth_);
	L = std::clamp(L, 0.2f, 2.0f);

    float R = (worldX - rect.left - 0.01f) / (kBlockWidth_);
    R = std::clamp(R, -0.5f, 0.8f);

	switch (type)
	{
   // case MapChipType::kBlock:
        //        return rect.top - 0.05f;                          // flat top
        //

	case MapChipType::kSlope1L:	// left high → right low (45°)
		return Lerp(rect.top + 0.43f, rect.bottom + 0.38f, L);

	case MapChipType::kSlope1R:	// left low → right high (45°)
		return Lerp(rect.bottom + 0.38f, rect.top + 0.43f, R);

	// ----- ready for future slopes -----
    case MapChipType::kSlope2L:	// left high → right low (23°)
        return Lerp(rect.top + 0.3f, rect.bottom + 1.1f, L);

    case MapChipType::kSlope2R:	// left low → right high (23°)
        return Lerp(rect.bottom + 1.1f, rect.top + 0.3f, R);

    case MapChipType::kSlope3L:	// left high → right low (23°)
        return Lerp(rect.top - 0.75f, rect.bottom + 0.15f, L);

    case MapChipType::kSlope3R:	// left low → right high (23°)
        return Lerp(rect.bottom + 0.15f, rect.top - 0.75f, R);
	default:
		return -10000.0f;		// not a slope
	}
}



bool MapChip::IsInBottomHalf(uint32_t xIndex, uint32_t yIndex, float posY) {
    Rect rect = GetRectByIndex(xIndex, yIndex);
    float midY = (rect.top + rect.bottom) * 0.5f;
    return posY < midY;
}

MapChip::IndexSet MapChip::GetMapChipIndexSetByPosition(const Vector3& position) {
    IndexSet indexSet = {};

    // X axis (left to right)
    indexSet.xIndex = static_cast<uint32_t>(std::floor(position.x + (kBlockWidth_ / 2.0f)) / kBlockWidth_);

    // Y axis - Important: Invert because world Y increases upward,
    // but map array[0] is the top row
    float invertedY = (kNumBlockVertical_ * kBlockHeight_) - position.y;
    indexSet.yIndex = static_cast<uint32_t>(std::floor(invertedY - kBlockHeight_ / 2.0f) / kBlockHeight_);

    // Clamp to valid range to prevent crashes
    indexSet.xIndex = std::clamp(indexSet.xIndex, 0u, kNumBlockHorizontal_ - 1);
    indexSet.yIndex = std::clamp(indexSet.yIndex, 0u, kNumBlockVertical_ - 1);

    return indexSet;
}

MapChip::Rect MapChip::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
    Rect rect;
    Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

    rect.left = center.x - kBlockWidth_ / 2.0f;
    rect.right = center.x + kBlockWidth_ / 2.0f;
    rect.bottom = center.y - kBlockHeight_ / 2.0f;
    rect.top = center.y + kBlockHeight_ / 2.0f;
    return rect;
}

MapChip::MapChip() {}

MapChip::~MapChip() {
}
