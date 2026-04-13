#include <bits/stdc++.h>
using namespace std;

// CACHE CONFIGURATION (Hardcoded)

const int CACHE_SIZE = 32*1024;  // 32KB
const int BLOCK_SIZE = 64;     // 64 Bytes
const int ASSOC = 4;           // 1 = Direct, 4 = 4-way, (CACHE_SIZE/BLOCK_SIZE) = Fully

struct CacheLine {
    bool valid = false;
    int tag = 0;
    int last_used = 0; 
};

int main() {
    // 1. CACHE INITIALIZATION
  
    int num_lines = CACHE_SIZE / BLOCK_SIZE;
    int num_sets = num_lines / ASSOC;

    if (num_sets == 0) num_sets = 1; 

    // cache[set_index][way_index]
    vector<vector<CacheLine>> cache(num_sets, vector<CacheLine>(ASSOC));

    int total_reads = 0;
    int total_writes = 0;
    int hits = 0;
    int misses = 0;
    int timer = 0; 

    ifstream trace_file("trace.out");
    if (!trace_file.is_open()) {
        cerr << "Error: Could not open trace.out" << endl;
        return 1;
    }

    string mode, addr_hex;
    while (trace_file >> mode >> addr_hex) {
       
        uint64_t address = stoull(addr_hex, nullptr, 16);
        timer++;

        if (mode == "R") total_reads++;
        else if (mode == "W") total_writes++;

        // calculations:
        // Offset bits = log2(BLOCK_SIZE)
        // Index bits = log2(num_sets)
        // Tag = remaining bits
        int set_index = (address / BLOCK_SIZE) % num_sets;
        int tag = address / (BLOCK_SIZE * num_sets);

        bool is_hit = false;
        int lru_way_index = 0;
        uint64_t min_timer = UINT64_MAX;
        int empty_way_index = -1;

        // Search within the specific set
        for (int i = 0; i < ASSOC; i++) {
            if (cache[set_index][i].valid && cache[set_index][i].tag == tag) {
                // HIT
                is_hit = true;
                hits++;
                cache[set_index][i].last_used = timer; 
                break;
            }
          
            if (!cache[set_index][i].valid && empty_way_index == -1) {
                empty_way_index = i;
            }

          
            if (cache[set_index][i].last_used < min_timer) {
                min_timer = cache[set_index][i].last_used;
                lru_way_index = i;
            }
        }

      
        if (!is_hit) {
            misses++;
            int target_way = 0;
            if (empty_way_index != -1) {
               
                target_way = empty_way_index;
            } else {
                target_way = lru_way_index;
            }

          
            cache[set_index][target_way].valid = true;
            cache[set_index][target_way].tag = tag;
            cache[set_index][target_way].last_used = timer;
        }
    }

    trace_file.close();

    //  STATISTICS
    double hit_rate = (double)hits / (hits + misses) * 100.0;

    cout << "Total Reads:  " << total_reads << endl;
    cout << "Total Writes: " << total_writes << endl;
    cout << "Total Access: " << (total_reads + total_writes) << endl;
    cout << "Hits:         " << hits << endl;
    cout << "Misses:       " << misses << endl;
    cout << fixed << setprecision(2);
    cout << "Hit Rate:     " << hit_rate << "%" << endl;


    return 0;
}
