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
            pm = new compressed_metadata(size, 200);
            break;
    }

    //auto *check = new array_map(size);
    string str, type, ID, value;
    ifstream ifs(file_name, ifstream::in);
    uint64_t count = 0;

    //set<uint64_t> items;

    printf("start reading file (max element: %lu)\n", size);
    auto start = chrono::high_resolution_clock::now();
    while (getline(ifs, str) && count < 30579340) {
        // process the input string
        istringstream ss(str);
        if(getline(ss, type, ',')) {
            if(getline(ss, ID,',')) {
                auto addr = (uint64_t) stoul(ID);
                //items.insert(addr);
                if(type == ACCESS) {
                    pm->add_address(addr);
                    //check->add_address(addr);
                } else {
                    if(getline(ss, value, ',')) {
                        auto val = (uint16_t) stoul(value);
                        pm->add_level(addr, val);
                        //check->add_level(addr, val);
                    }
                }
                //printf("count: %d = %d, level: %d = %d\n", pm->auxiliary_info(addr), check->auxiliary_info(addr),
                        //pm->level_query(addr), check->level_query(addr));
            } else {
                printf(">> couldn't parse file address\n");
            }
        } else {
            printf(">> couldn't parse operation type\n");
        }
        count++;
    }
    printf("end reading file\n");
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "count: " << count << endl;
    cout << "time: " << duration.count() << endl;
    ifs.close();

    return duration.count();
}

int main() {


    string file_path = "/home/student.unimelb.edu.au/wholland/Dropbox/oram_posmap/data/";

    string files[3] = {"ssd-level.txt", "k5-level.txt", "cloud-level.txt"};
    uint64_t sizes[3] = {3057934, 1065643040, 4370466280};

    vector<uint64_t> times;

    //string file_name_output = file_path + "results.txt";
    //ofstream ofs;
    //ofs.open(file_name_output);


    uint8_t file = K5;


    execute_test(HISTORICAL, file_path + files[file], sizes[file]);


    /*
    for (int i = 0; i < 3; ++i) {
        // iteration for each file
        string file_name = file_path + files[i];
        uint64_t size = sizes[i];

        ofs << file_name << "\n";
        for (int j = 0; j < 3; ++j) {
            // iteration for each algorithm
            run_time = execute_test(j, file_name, size);
            ofs << run_time << ", ";
        }
        ofs << "\n";
    }
     ofs.close();
    */
}








