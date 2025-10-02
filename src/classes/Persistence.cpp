#include "../../include/Persistence.hpp"

Persistence::Persistence(std::string log_filepath) : log_file(log_filepath)
{
}

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
}

void Persistence::writeBlocksToLogFile()
{
    /*
        1. open file
        2. loop over every item in the hash map
        3. write in the proper format (chunk id, block coords, block type, some sort of delimiter or end line)
    */
}

void Persistence::addBlockToMap(BlockCoord chunk_coord, int block_coord, std::string block_type)
{
}

void Persistence::saveFile()
{
    /*
        1. use the write blocks to log file function
        2. then compress the file
    */
}