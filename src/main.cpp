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
    //TODO might not even need this 

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
            
            void* startAddr = mmap(NULL, 2*m_capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1,  0);

            MemNode initialChunk(m_capacity, true);

            
            m_startAddr = (MemNode*)(startAddr);

            // We have structure like [header] | [data]
            *m_startAddr = initialChunk; 

        }

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
                    
                    //TODO deal with little annoying fragmented spots that are smaller than sizeof(MemNode)

                    //Create a new free chunk at the end of memNode + (size + memNodeSize)
                    //
                    
                    MemNode* nextChunkMeta = getNextNode(memNode);


                    *nextChunkMeta = MemNode(availBytes - size - memNodeSize, true);

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

        //TODO Just loop through until you get to the next node, then i guess sharedptr new node and link it?
        void free(void* addr){

        }

};
