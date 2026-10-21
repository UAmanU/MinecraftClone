#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include "Config/block_config.h"
#include "Config/world_config.h"

struct ChunkCoord {
    int x{};
    int z{};
    bool operator==(const ChunkCoord&) const = default;
};

struct ChunkCoordHash {
    std::size_t operator()(const ChunkCoord& coord) const noexcept;
};

struct Chunk {
    ChunkCoord coordinate{};
    std::vector<BlockType> blocks;
};

class World {
public:
    void UpdateAround(float world_x, float world_z);
    BlockType GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType type);
    float SurfaceTop(int x, int z) const;
    const auto& LoadedChunks() const { return chunks_; }

private:
    struct BlockCoord { int x{}, y{}, z{}; bool operator==(const BlockCoord&) const = default; };
    struct BlockCoordHash { std::size_t operator()(const BlockCoord&) const noexcept; };
    static int FloorDiv(int value, int divisor);
    static int Local(int value, int divisor);
    static std::size_t Index(int x, int y, int z);
    static int TerrainHeight(int x, int z);
    static unsigned Noise(int x, int z);
    Chunk& EnsureChunk(ChunkCoord coordinate);
    void GenerateChunk(Chunk& chunk);

    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> chunks_;
    ChunkCoord center_{};
    bool has_center_{false};
    std::unordered_map<BlockCoord, BlockType, BlockCoordHash> edits_;
};
