#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>

int main(){
    std::cout << "Hello World!" << '\n'; 
    
    return 0;
}

//A header representing the start of a chunk of memory 
//We return a pointer to this chunk of memory by doing memNode + 1, this gives us a pointer right outside the struct 
struct MemNode{
    //The size of the memory chunk
    std::uint16_t m_size;

    //TODO could bit pack this maybe?
    bool free;

    MemNode(int m_size = 0, bool free = true){}
};

class dynAlloc{
    private:
        int m_capacity;
        MemNode* m_startAddr;

    public:
        static constexpr std::size_t memNodeSize = sizeof(MemNode);

        dynAlloc(int m_capacity = 1024) : m_capacity(m_capacity){

            //TODO We use 
            void* startAddr = mmap(NULL, 2*m_capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1,  0);

            MemNode initialChunk(m_capacity, true);

            
            m_startAddr = (MemNode*)(startAddr);

            // We have structure like [header] | [data]
            *m_startAddr = initialChunk; 

        }

        //TODO call unmap on the 
        ~dynAlloc(){

        }

        void* alloc(size_t size){
            void* chunk = nullptr;

            //Start at the beginning of the mmap
            MemNode* memNode = m_startAddr;
            //TODO Linear search including checks of the taken chunks is not the best, fix this l8r
            while(memNode != nullptr){
                // We can fit the requested size in this chunk yey 
                size_t availBytes = memNode->m_size;
                if(availBytes >= size + memNodeSize){

                    MemNode newChunkMeta = {(uint16_t)size, false};
                    *memNode = newChunkMeta;
                    
                    //If the available amount of bytes is less than or equal to a header, just give it all to the current fframe
                    //TODO WASTED SPACE, more data allocated than given
                    size_t leftOverBytes = availBytes - size - memNodeSize;

                    if(leftOverBytes > memNodeSize){
                        MemNode* nextChunkMeta = getNextNode(memNode);

                        *nextChunkMeta = MemNode(- size - memNodeSize, true);
                    }
                    else {
                        newChunkMeta.m_size = availBytes;
                    }


                    return memNode + 1;
                }
                else{
                    memNode = getNextNode(memNode);
                }
            }

            throw std::runtime_error("No memory found :\(");

        }


        //Just returns the addy right after [MemNode metadata][data(size in bytes)]
        MemNode* getNextNode(MemNode* node){

            char* convertedMetaPtr = (char*)node;
            convertedMetaPtr += node->m_size + memNodeSize;
            return (MemNode*)convertedMetaPtr;

        }

        //TODO Go to the address, and fuse with next chunk if its also free
        void free(void* addr){
            //Get address to the header
            MemNode* memNode = (MemNode*)addr - 1;
            memNode->free = true;

            MemNode* nextNode = getNextNode(memNode);

            //If next node is free, we can combine them :D
            if (nextNode->free){
                memNode->m_size += memNodeSize + nextNode->m_size;
                *nextNode = NULL;
            }
    
        }

};
