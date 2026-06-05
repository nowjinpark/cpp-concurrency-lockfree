#include <iostream>       // std::cout
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <deque>          // std::deque
#include <mutex>          // std::mutex

class MemoryBank {
    private:
        std::mutex mtx_;
        std::deque<std::vector<int>*> garbage_;
    public:
        // retire: store a pointer into a queue for deferred deletion
        void retire(std::vector<int>* ptr) {
            if(!ptr) return;
            std::lock_guard<std::mutex> lock(mtx_);
            garbage_.push_back(ptr);
        }
        void clean() {
            std::lock_guard<std::mutex> lock(mtx_);
            while(!garbage_.empty()) {
                std::vector<int>* p = garbage_.front();
                garbage_.pop_front();
                delete p;
            }
        }
        ~MemoryBank() {
            clean();
        }
    };

    struct Pair {
        std::vector<int>* pointer;
        long              ref_count;
    } __attribute__((aligned(16),packed));

    class LFSV {
        std::atomic< Pair > pdata;
        
        MemoryBank mem_bank;
    
    public:
        LFSV() 
            : pdata( Pair{ new std::vector<int>, 1 } )
        {
        }

        ~LFSV() {
            Pair temp = pdata.load(std::memory_order_relaxed);
            if(temp.pointer) {
                mem_bank.retire(temp.pointer);
            }
        }
    
        void Insert( int const & v ) {
            Pair pdata_new, pdata_old;
            pdata_new.pointer  = nullptr;
    
            do {
                // delete local pointer from the previous loop iteration
                delete pdata_new.pointer;
                pdata_new.pointer = nullptr;
                // load current Pair from atomic
                pdata_old = pdata.load(std::memory_order_acquire);
                // make a new vector by copying the old vector
                pdata_new.pointer = new std::vector<int>( *pdata_old.pointer );

                // sorted insertion logic from original code
                if ( pdata_new.pointer->empty() 
                     || v >= pdata_new.pointer->back() ) {
                    pdata_new.pointer->push_back( v );
                } else {
                    auto b = pdata_new.pointer->begin();
                    auto e = pdata_new.pointer->end();
                    for ( ; b!=e; ++b ) {
                        if ( *b >= v ) {
                            pdata_new.pointer->insert( b, v );
                            break;
                        }
                    }
                }
    
                pdata_new.ref_count = 1;
    
            } while ( !(this->pdata).compare_exchange_weak(
                            pdata_old, 
                            pdata_new,
                            std::memory_order_release,
                            std::memory_order_acquire )
                    );
            
            // after a successful CAS, retire the old vector pointer
            if( pdata_old.pointer ) {
                mem_bank.retire(pdata_old.pointer);
            }
        }
    
        int operator[] ( int pos ) { 
            Pair temp = pdata.load(std::memory_order_acquire);
            return (*temp.pointer)[pos];
        }
};