#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include "compressed/roaring.hh"
#include "alg/hist_mem.h"
#include "include/leaf_contents.h"
#include "alg/counter_interval.h"
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

typedef dyn::wt_str compressed_string;

using namespace std;


uint64_t execute_test(const char alg, const string& file_name, uint64_t size) {

    printf("\n:: file name: %s\n", file_name.c_str());

    auto oram = new h_oram(size, alg);

    position_map *pm;
    string str;
    ifstream ifs(file_name, ifstream::in);
    printf("start reading file (max element: %lu)\n", size);
    auto start = chrono::high_resolution_clock::now();
    uint64_t count = 0, max = 0;
    while (getline(ifs, str) && count < 3058934) {
        auto addr = (uint64_t) stoul(str);
        oram->access(addr);
        //pm->add_address(addr);
        count++;
    }
    printf("end reading file\n");
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "count: " << count << endl;
    cout << "time: " << duration.count() << endl;
    ifs.close();

    //delete pm;
    return duration.count();
}

int main() {

    string file_path = "/home/student.unimelb.edu.au/wholland/Dropbox/oram_posmap/data/";

    string files[3] = {"ssdtrace.txt", "k5b.txt", "cloud_storage.txt"};
    uint64_t sizes[3] = {3057934, 1065643040, 4370466280};

    vector<uint64_t> times;

    string file_name_output = file_path + "results.txt";
    ofstream ofs;
    ofs.open(file_name_output);

    uint8_t file = SSD;

    execute_test(BENCHMARK, file_path + files[file], sizes[file]);
    uint64_t run_time;

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








