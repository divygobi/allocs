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

//A pointer representing the start of a chunk of memory 
struct MemNode{
    //A pointer to the start of the mem, 
    void* m_start;

    //The size of the memory chunk
    int m_size;

    //A link to the next available chunk
    MemNode* m_next;
    MemNode* m_prev;

    MemNode(void* start = nullptr, int size = 0 , MemNode* next = nullptr, MemNode* prev = nullptr) : m_start(start), m_size(size), m_next(next), m_prev(prev){}
};

class dynAlloc{
    private:
        int m_capacity;
        void* m_startAddr;
        
        //The heam_size > std::si to the available chunk
        MemNode* m_availHead;  
        MemNode* m_takenHead;

    public:

        dynAlloc(int m_capacity = 1025) : m_capacity(m_capacity){
            m_startAddr = mmap(NULL, m_capacity, PROT_READ | PROT_WRITE, MAP_ANON, -1,  0);

            MemNode avail;
            MemNode taken;
            m_availHead = &avail;
            m_takenHead = &taken;

            MemNode initialChunk(m_startAddr, m_capacity, nullptr, m_availHead);
            
            //We have m_capacity bytes available now
            m_availHead->m_next = &initialChunk;

        }

        ~dynAlloc(){

        }

        //size is the size of the memory the pointer is pointing to
        void* alloc(size_t size){
            void* chunk = nullptr;
            MemNode* memNode = m_availHead;
            while(memNode->m_next){
                MemNode* currNode = memNode->m_next;
                if(currNode->m_size >= size){
                    chunk = currNode->m_start;
                    MemNode* nodeNext = currNode->m_next;

                    if(currNode->m_size - size == 0){
                        //delete Node
                        memNode->m_next = nodeNext;
                        if(nodeNext){
                            nodeNext->m_prev = memNode;
                        }
                        
                        //might not need this lol
                        currNode->m_next = nullptr;
                        currNode->m_prev = nullptr;
                        
                    }
                    else{ // Resize chunk 
                        currNode->m_size -= size;
                        
                        char* newAddr = (char*)(currNode->m_start) + size;

                        currNode->m_start = (void*)newAddr;

                    }
                    return chunk;
                }
                else{
                    memNode = currNode;
                }

            }

            if(chunk){
                return chunk;
            }
            else{ 
                throw std::runtime_error("No memory found :\(");
            }

        }

        //TODO Just loop through until you get to 
        void free(void* addr){

        }

};
