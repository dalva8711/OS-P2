#include "MemoryManager.h"
\
MemoryManager::MemoryManager() {
    wordSize = 0;
    allocator = nullptr;
    initialized = false;
}

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
    this->wordSize = wordSize;
    this->allocator = allocator;
    initialized = false;
}

MemoryManager::~MemoryManager() {
    shutdown();
}

void MemoryManager::initialize(size_t sizeInWords) {
    if (initialized) {
        shutdown();
        initialize(sizeInWords);
    }

    totalMem = sizeInWords * wordSize;
    memory = new uint8_t[totalMem];
    blocks.push_back(Block(true, 0, totalMem));
    initialized = true;
}

void MemoryManager::shutdown() {
    delete[] memory;
    blocks.clear();
    initialized = false;
}

void* MemoryManager::allocate(size_t sizeInBytes) {
    if (!initialized || sizeInBytes > totalMem) {
        return nullptr;
    }

    size_t numWords = sizeInBytes / wordSize;
    if (sizeInBytes % wordSize != 0) {
        numWords++;
    }

    int offset = allocator(numWords, getList());
    if (offset == -1) {
        return nullptr;
    }

    blocks.push_back(Block(false, offset, sizeInBytes));
    return memory + offset;
}

void* MemoryManager::getList(){
    holes.empty();
    holes.push_back(0); // assign that there are zero holes first

    for (const auto& block : blocks) {
        if (block.hole) { 
            holes[0]++; 
            holes.push_back(block.offset);
            holes.push_back(block.length);
        }
    }


    void* ptr = &holes[0];
    return ptr;
}

void MemoryManager::free(void *address) {
    /*if (!initialized) {
        return;
    }*/

    size_t offset = static_cast<uint8_t*>(address) - memory;
    unsigned holeLength = 0;
    unsigned blockSize = 0;

    for (auto& block : blocks) {
        if (block.offset == offset) { 

            blockSize = block.length;
            holeLength += block.length;
            block.hole = true;

            auto it = std::find(blocks.begin(), blocks.end(), block); 
            
            auto prevIt = it;
            auto nextIt = it;

            while (prevIt != blocks.begin() && std::prev(prevIt)->hole) {
                block.offset = std::prev(prevIt)->offset;
                holeLength += std::prev(prevIt)->length;
                --prevIt;
            }
            
            while (std::next(nextIt) != blocks.end() && std::next(nextIt)->hole) {
                holeLength += std::next(nextIt)->length;
                ++nextIt;
            }

            for (auto iter = prevIt; iter == nextIt; iter++) {
                if (iter != it) {
                    blocks.erase(iter);
                }
            }

            block.length = holeLength;
            break;
        }
    }

}

void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
    this->allocator = allocator;
}


unsigned MemoryManager::getWordSize() {
    return wordSize;
}



void* MemoryManager::getMemoryStart(){
    return memory;
}

unsigned MemoryManager::getMemoryLimit(){
    return totalMem;
}

int MemoryManager::dumpMemoryMap(char *filename){
    return -1;
}

void* MemoryManager::getBitmap(){
    return nullptr;
}





