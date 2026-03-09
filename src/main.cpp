#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unordered_map>

int main(){
    std::cout << "Hello World!" << '\n'; 
    
    return 0;
}

class dynAlloc{
    private:
        unordered_map<void*, size_t>;
        
    public:
        void* alloc();
        void free();
        dynAlloc();
        

};
