#include "../../include/Persistence.hpp"
#include "../../include/huffman_encoder.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

void Persistence::loadBlocksFromLogFile()
{
    /*
        1. open file
        2. Decode/Decompress file
        3. parse by column get the chunk id, block coords, and block type
        4. place into map
            4.1 if it doesn't exist make new record
            4.2 if it does, then just replace with the new block type
    */

    std::ifstream decoded_file("../saved_worlds/world1_encoded", std::ios::in | std::ios::out);

    if (!decoded_file.is_open())
    {
        // empty so fresh world
        return;
    }

    if (decoded_file.is_open())
    {
        huff::decodeFile("../saved_worlds/world1_encoded", "../saved_worlds/world1_decoded");
        std::cout << "File opened successfully\n";

        std::ifstream file("../saved_worlds/world1_decoded", std::ios::in | std::ios::out);

        // Use file << or file >> here
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::istringstream iss(line);
                uint64_t chunk_id;
                int block_x, block_y, block_z;
                std::string block_type;

                if (!(iss >> chunk_id >> block_x >> block_y >> block_z >> block_type))
                {
                    std::cerr << "Line parse error: " << line << "\n";
                    continue; // skip bad line
                }

                // add to map
                BlockCoord coord = {block_x, block_y, block_z};
                modifiedBlocksMap[chunk_id][coord] = block_type;
                std::cout << chunk_id << " " << block_x << " " << block_y << " " << block_z << " " << block_type << "\n";
            }
            printf("map size: %d\n", modifiedBlocksMap.size());
        }
    }
}

void Persistence::writeBlocksToLogFile()
{
    /*
        1. open file
        2. loop over every item in the hash map
        3. write in the proper format (chunk id, block coords, block type, some sort of delimiter or end line)
    */
    std::ofstream file("../saved_worlds/world1_decoded", std::ios::out);
    for (auto &outer_pair : modifiedBlocksMap)
    {
        uint64_t chunk_id = outer_pair.first;
        auto &inner_map = outer_pair.second;
        for (auto &inner_pair : inner_map)
        {
            const BlockCoord &block_coord = inner_pair.first;
            const std::string &block_type = inner_pair.second;
            file << std::setw(20) << std::setfill('0') << chunk_id << " "
                 << block_coord.x << " " << block_coord.y << " " << block_coord.z << " " << block_type << "\n";
            // file << chunk_id << " " << block_coord.x << " " << block_coord.y << " " << block_coord.z << " " << block_type << "\n";
        }
    }
}

void Persistence::addBlockToMap(BlockCoord block_coord, uint64_t chunk_id, std::string block_type)
{

    modifiedBlocksMap[chunk_id][block_coord] = block_type;
}

void Persistence::saveFile()
{
    writeBlocksToLogFile();
    huff::encodeFile("../saved_worlds/world1_decoded", "../saved_worlds/world1_encoded");

    /*
        1. use the write blocks to log file function
        2. then compress the file
    */
}