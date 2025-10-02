#pragma once
#include <unordered_map>
#include <string>

struct BlockCoord
{
    int x;
    int y;
    int z;
};
// unordered_map<ChunkCoord, unordered_map<BlockCoord, BlockType>>
class Persistence
{

public:
    std::string log_file;
    std::unordered_map<int, std::unordered_map<BlockCoord, std::string>> modifiedBlocksMap;

    Persistence(std::string log_filepath);
    void loadBlocksFromLogFile();
    void writeBlocksToLogFile();
    void addBlockToMap(BlockCoord chunk_coord, int block_coord, std::string block_type);
    void saveFile();
};