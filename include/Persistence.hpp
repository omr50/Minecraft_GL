#pragma once
#include <unordered_map>
#include <string>

struct BlockCoord
{
    int x;
    int y;
    int z;

    bool operator==(const BlockCoord &o) const
    {
        return x == o.x && y == o.y && z == o.z;
    }
};

struct BlockCoordHash
{
    std::size_t operator()(const BlockCoord &bc) const
    {
        std::size_t h1 = std::hash<int>()(bc.x);
        std::size_t h2 = std::hash<int>()(bc.y);
        std::size_t h3 = std::hash<int>()(bc.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

// unordered_map<ChunkCoord, unordered_map<BlockCoord, BlockType>>
class Persistence
{

public:
    std::string log_file;
    std::unordered_map<int, std::unordered_map<BlockCoord, std::string, BlockCoordHash>> modifiedBlocksMap;

    void loadBlocksFromLogFile();
    void writeBlocksToLogFile();
    void addBlockToMap(BlockCoord block_coord, int chunk_id, std::string block_type);
    void saveFile();
};