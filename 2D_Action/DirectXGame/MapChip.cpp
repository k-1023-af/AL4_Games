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

float MapChip::GetSlopeSurfaceY(uint32_t xIndex, uint32_t yIndex, float worldX) {
    MapChipType type = GetMapChipTypeByIndex(xIndex, yIndex);
    if (type != MapChipType::kSlope1L && type != MapChipType::kSlope1R)
        return -10000.0f;   // not a slope

    Rect rect = GetRectByIndex(xIndex, yIndex);

    float t = (worldX - rect.left) / kBlockWidth_ ;   // 0 → 1 across the tile
    t = std::clamp(t, 0.0f - 0.02f, 1.0f + 0.12f);

    if (type == MapChipType::kSlope1L) {
        // Left side high, right side low (or the opposite – match model)
        return std::lerp(rect.top + 0.5f, rect.bottom + 0.17f, t);
    }
    t = std::clamp(t, 0.0f, 1.0f - 0.18f);
    if (type == MapChipType::kSlope1R) {
        return std::lerp(rect.bottom + 0.15f, rect.top + 0.5f, t);
    }
    else {
        return rect.top;
    }
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
