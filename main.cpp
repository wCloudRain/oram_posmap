#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include "compressed/roaring.hh"
#include "alg/hist_mem.h"
#include "include/leaf_contents.h"
#include "alg/compressed_metadata.h"
#include "compressed/dynamic/dynamic.hpp"
#include "compressed/dynamic/internal/wt_string.hpp"
#include "oram/h_oram.h"

#define BENCHMARK 0
#define ARRAY 1
#define HISTORICAL 2
#define COUNTER 3
#define SSD 0
#define K5 1
#define CLOUD 2
#define ACCESS "a"


typedef dyn::wt_str compressed_string;

using namespace std;


uint64_t execute_test(const char alg, const string& file_name, uint64_t size) {

    printf("\n:: file name: %s\n", file_name.c_str());
    position_map *pm;
    switch (alg) {
        case BENCHMARK:
            printf("POS MAP = BENCHMARK\n");
            pm = new benchmark(size);
            break;
        case ARRAY:
            printf("POS MAP = ARRAY\n");
            pm = new array_map(size);
            break;
        case HISTORICAL:
            printf("POS MAP = HISTORICAL\n");
            pm = new hist_mem(size);
            break;
        case COUNTER:
            printf("POS MAP = COUNTER INTERVAL\n");
            pm = new compressed_metadata(size, 20000);
            break;
    }

    //auto *check = new array_map(size);
    string str, type, ID, value;
    ifstream ifs(file_name, ifstream::in);
    uint32_t count = 0;

    //set<uint64_t> items;

    printf("start reading file (max element: %lu)\n", size);
    auto start = chrono::high_resolution_clock::now();
    while (getline(ifs, str)) {
        // process the input string
        istringstream ss(str);
        if(getline(ss, type, ',')) {
            if(getline(ss, ID,',')) {
                auto addr = (uint64_t) stoul(ID);
                //items.insert(addr);
                if(type == ACCESS) {
                    pm->add_address(addr);
                    //check->add_address(addr);
                    //printf("\naddress %d\n", addr);
                    count++;
                } else {
                    if(getline(ss, value, ',')) {
                        auto val = (uint16_t) stoul(value);
                        pm->add_level(addr, val);
                        //check->add_level(addr, val);
                        //printf("\nLevel %d for %d\n", val, addr);
                    }
                }
                //printf("count: %d\n",  count);
                //assert(pm->auxiliary_info(1) == check->auxiliary_info(1));
                //printf("count: %d = %d, level: %d = %d\n", pm->auxiliary_info(addr), check->auxiliary_info(addr),
                        //pm->level_query(addr), check->level_query(addr));
            } else {
                printf(">> couldn't parse file address\n");
            }
        } else {
            printf(">> couldn't parse operation type\n");
        }
    }
    printf("end reading file\n");
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "count: " << count << endl;
    cout << "time: " << duration.count() << endl;
    ifs.close();
    address addr = 1;
    printf("count(%lu) = %d, level(%lu) = %d\n", addr, pm->auxiliary_info(addr), addr, pm->level_query(addr));
    addr = 10;
    printf("count(%lu) = %d, level(%lu) = %d\n", addr, pm->auxiliary_info(addr), addr, pm->level_query(addr));
    addr = 100;
    printf("count(%lu) = %d, level(%lu) = %d\n", addr, pm->auxiliary_info(addr), addr, pm->level_query(addr));
    addr = 1000;
    printf("count(%lu) = %d, level(%lu) = %d\n", addr, pm->auxiliary_info(addr), addr, pm->level_query(addr));
    addr = 10000;
    printf("count(%lu) = %d, level(%lu) = %d\n", addr, pm->auxiliary_info(addr), addr, pm->level_query(addr));

    delete pm;
    return duration.count();
}

int main() {


    string file_path = "/home/student.unimelb.edu.au/wholland/Dropbox/oram_posmap/data/synthetic/";
    //string file_name =  ;//"ssdtrace-level.txt";

    uint32_t max = 10371459;

    //string files[3] = {"ssd-level.txt", "k5-level.txt", "cloud-level.txt"};
    //uint64_t sizes[3] = {3057934, 1065643040, 4370466280};

    uint32_t sizes[5] = {21,23,25,27,29};
    string n[5] = {"21","23","25","27","29"};
    string skews[5] = {" 11", " 12", " 13"," 14"," 15"};

    //auto size = (uint32_t) pow(2,sizes[0]);
    uint32_t size = pow(2,sizes[3]);
    std::string str, token;

    execute_test(COUNTER, file_path + "uniform-level.txt" , size);

}








