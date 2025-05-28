#pragma once
#include <functional>
#include <list>
#include <vector>
#include <iterator>

int bestFit(int sizeInWords, void *list) {
    uint16_t* ptr = static_cast<uint16_t*>(list);
    uint16_t totalHoles = *ptr;
    int bestFit = -1;
    uint16_t* prev;
    int offset = -1;


    for (int i = 0; i < totalHoles; i++) {
        if (i % 2 != 0 || i == 0) {
            prev = ptr;
            ptr++;
            continue;
        }
        else {
            if (*ptr == sizeInWords) {
                offset = *prev;
                bestFit = *ptr;
                break;
            }
            else if (*ptr > sizeInWords) {
                if (bestFit == -1) {
                    offset = *prev;
                    bestFit = *ptr;
                }
                else if (bestFit > *ptr) {
                    offset = *prev;
                    bestFit = *ptr;
                }
            }
        }
    }

    return offset;
}

int worstFit(int sizeInWords, void *list) {
    uint16_t* ptr = static_cast<uint16_t*>(list);
    uint16_t totalHoles = *ptr;
    int worstFit = -1;
    uint16_t* prev;
    int offset = -1;


    for (int i = 0; i < totalHoles; i++) {
        if (i % 2 != 0 || i == 0) {
            prev = ptr;
            ptr++;
            continue;
        }
        else {
            if (*ptr >= sizeInWords) {
                if (worstFit == -1) {
                    offset = *prev;
                    worstFit = *ptr;
                }
                else if (worstFit < *ptr) {
                    offset = *prev;
                    worstFit = *ptr;
                }
            }
        }
    }

    return offset;
}


class MemoryManager {
    struct Block {
        bool hole;
        size_t offset;
        unsigned length;

        Block() 
        : hole(true), offset(0), length(0) {}

        Block(bool isHole, unsigned off, unsigned len) 
        : hole(isHole), offset(off), length(len) {}
    };

    private:
        unsigned wordSize;
        size_t totalMem;
        std::function<int(int, void *)> allocator;
        uint8_t* memory;
        std::list<Block> blocks;
        std::vector<uint16_t> holes;
        bool initialized;


    public:
        MemoryManager();
        MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
        ~MemoryManager();
        void initialize(size_t sizeInWords);
        void shutdown();
        void *allocate(size_t sizeInBytes);
        void *getList();
        void free(void *address);
        void setAllocator(std::function<int(int, void *)> allocator);
        unsigned getWordSize();
        void *getMemoryStart();
        unsigned getMemoryLimit();
        int dumpMemoryMap(char *filename);
        void *getBitmap();

};

