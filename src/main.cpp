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

        dynAlloc(int m_capacity = 1024) : m_capacity(m_capacity){
            
            void* startAddr = mmap(NULL, 2*m_capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1,  0);

            MemNode initialChunk(m_capacity, true);

            
            m_startAddr = (MemNode*)(startAddr);

            // We have structure like [header] | [data]
            *m_startAddr = initialChunk; 

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

            throw std::runtime_error("No memory found :\(");

        }

        //TODO Just loop through until you get to the next node, then i guess sharedptr new node and link it?
        void free(void* addr){

        }

};
