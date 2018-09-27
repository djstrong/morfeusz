/* 
 * File:   main.cc
 * Author: mlenart
 *
 * Created on October 8, 2013, 12:41 PM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include "morfeusz2.h"
#include "cli/cli.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, const char** argv) {
    cerr << "Morfeusz analyzer, version: " << Morfeusz::getVersion() << endl;
    Morfeusz* morfeusz;
    try {
        ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
        morfeusz = initializeMorfeusz(opt, ANALYZER);
        delete &opt;
    }
    catch (const std::exception& ex) {
        cerr << ex.what() << endl;
        exit(1);
    }
    string line;
    vector<MorphInterpretation> res;
    try {
        while (getline(cin, line)) {
            res.resize(0);
            morfeusz->analyse(line, res);
            printMorphResults(*morfeusz, res, true);
        }
    }
    catch (const std::exception& ex) {
        cerr << "Failed to perform morphosyntactic analysis: " << ex.what() << endl;
        exit(1);
    }
    delete morfeusz;
    printf("\n");
    return 0;
}
