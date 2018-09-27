/* 
 * File:   ResultsManager.cpp
 * Author: mlenart
 * 
 * Created on 24 czerwiec 2014, 17:09
 */

#include "ResultsManager.hpp"
#include "morfeusz2.h"
#include <iostream>

using namespace std;

namespace morfeusz {

    static const int initialSize = 1024;

    ResultsManager::ResultsManager(const Morfeusz* morfeusz)
    : morfeusz(morfeusz), results(new InterpMorf[initialSize]()),
    resultsArraySize(initialSize) {
    }

    ResultsManager::~ResultsManager() {
        delete[] results;
    }

    InterpMorf* ResultsManager::convertResults(const std::vector<MorphInterpretation>& res) {
        if (res.size() + 1 > resultsArraySize) {
            reserve(res.size() + 1);
        }
        for (unsigned int i = 0; i < res.size(); i++) {
            results[i] = convertOneResult(res[i]);
        }
        results[res.size()].p = -1;
        return results;
    }

    void ResultsManager::reserve(size_t size) {
        while (resultsArraySize <= size) {
            resultsArraySize *= 2;
        }
        delete[] results;
        results = new InterpMorf[resultsArraySize];
    }

    InterpMorf ResultsManager::convertOneResult(const MorphInterpretation& res) {
        InterpMorf convertedRes;
        convertedRes.p = res.startNode;
        convertedRes.k = res.endNode;
        convertedRes.forma = const_cast<char*>(res.orth.c_str());
        convertedRes.haslo = const_cast<char*>(res.lemma.c_str());
        convertedRes.interp = const_cast<char*>(morfeusz->getIdResolver().getTag(res.tagId).c_str());
        return convertedRes;
    }
}
