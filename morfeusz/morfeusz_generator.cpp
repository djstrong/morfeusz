/* 
 * File:   morfeusz_generator.cpp
 * Author: mlenart
 *
 * Created on 21 styczeń 2014, 12:02
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include "morfeusz2.h"
#include "cli/cli.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, const char** argv) {
    cerr << "Morfeusz generator, version: " << Morfeusz::getVersion() << endl;
    Morfeusz* morfeusz;
    try {
        ez::ezOptionParser& opt = *getOptions(argc, argv, GENERATOR);
        morfeusz = initializeMorfeusz(opt, GENERATOR);
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
         //        printf("%s\n", line.c_str());
            res.clear();
            morfeusz->generate(line, res);
            printMorphResults(*morfeusz, res, false);
        }
    }
    catch (const std::exception& ex) {
        cerr << "Failed to perform morphosyntactic synthesis: " << ex.what() << endl;
        exit(1);
    }
    printf("\n");
    return 0;
}
