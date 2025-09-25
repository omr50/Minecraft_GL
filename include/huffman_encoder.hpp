#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace huff
{

    // Public node type (kept for minimal change)
    struct hNode
    {
        unsigned char data;
        int count;
        hNode *left;
        hNode *right;
    };

    // Core API
    std::unordered_map<unsigned char, int> *getCount(std::string filePath);
    hNode *makeHuffmanTree(std::unordered_map<unsigned char, int> *counts);

    std::unordered_map<unsigned char, std::string> *createEncodings(hNode *huffmanTree);
    void recursiveEncode(hNode *node,
                         std::unordered_map<unsigned char, std::string> *encodings,
                         std::string currEncoding);

    void encodeFile(std::string inputFileName, std::string outputFileName);

    std::unordered_map<std::string, unsigned char> *
    createDecodings(std::unordered_map<unsigned char, int> *counts);

    void decodeFile(std::string encodedFileName, std::string outputFileName);

} // namespace huff
