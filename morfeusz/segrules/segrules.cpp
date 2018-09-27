
#include "segrules.hpp"
#include "fsa/fsa.hpp"
#include "fsa/const.hpp"
#include "deserialization/deserializationUtils.hpp"

using namespace std;

namespace morfeusz {

static inline void skipSeparatorsList(const unsigned char*& ptr) {
    uint16_t listSize = readInt16(ptr);
    ptr += 4 * listSize;
}

static inline const unsigned char* getSeparatorsListPtr(const unsigned char* ptr) {
    const unsigned char* epiloguePtr = getEpiloguePtr(ptr);
    const unsigned char* res = epiloguePtr + readInt32Const(epiloguePtr) + 4;
    return res;
}

static inline const unsigned char* getFSAsMapPtr(const unsigned char* ptr) {
    const unsigned char* res = getSeparatorsListPtr(ptr);
    skipSeparatorsList(res);
    return res;
}

static inline SegrulesOptions deserializeOptions(const unsigned char*& ptr) {
    SegrulesOptions res;
    unsigned char optsNum = *ptr;
    ptr++;
    for (unsigned char i = 0; i < optsNum; i++) {
        string key = readString(ptr);
        res[key] = readString(ptr);
    }
    return res;
}

static inline SegrulesFSA* deserializeFSA(const unsigned char*& ptr) {
    uint32_t fsaSize = readInt32(ptr);
//    static SegrulesDeserializer deserializer;
    SegrulesFSA* res = new SegrulesFSA(ptr);
    ptr += fsaSize;
    return res;
}

map<SegrulesOptions, SegrulesFSA*> createSegrulesFSAsMap(const unsigned char* analyzerPtr) {
    map<SegrulesOptions, SegrulesFSA*> res;
    const unsigned char* fsasMapPtr = getFSAsMapPtr(analyzerPtr);
    const unsigned char* currPtr = fsasMapPtr;
    unsigned char fsasNum = *currPtr;
    currPtr++;
    for (unsigned char i = 0; i < fsasNum; i++) {
        SegrulesOptions options = deserializeOptions(currPtr);
        SegrulesFSA* fsa = deserializeFSA(currPtr);
        res[options] = fsa;
    }
    return res;
}

SegrulesOptions getDefaultSegrulesOptions(const unsigned char* ptr) {
    const unsigned char* fsasMapPtr = getFSAsMapPtr(ptr);
    const unsigned char* currPtr = fsasMapPtr;
    unsigned char fsasNum = *currPtr;
    currPtr++;
    for (unsigned char i = 0; i < fsasNum; i++) {
        deserializeOptions(currPtr);
        deserializeFSA(currPtr);
    }
    return deserializeOptions(currPtr);
}

SegrulesFSA* getDefaultSegrulesFSA(
        const map<SegrulesOptions, SegrulesFSA*>& map, 
        const unsigned char* ptr) {
    SegrulesOptions opts = getDefaultSegrulesOptions(ptr);
    return (*(map.find(opts))).second;
}

vector<uint32_t> getSeparatorsList(const unsigned char* ptr) {
    ptr = getSeparatorsListPtr(ptr);
    vector<uint32_t> res;
    uint16_t listSize = readInt16(ptr);
    for (unsigned int i = 0; i < listSize; i++) {
        res.push_back(readInt32(ptr));
    }
    return res;
}

void debugMap(const map<SegrulesOptions, SegrulesFSA*>& res) {
    map<SegrulesOptions, SegrulesFSA*>::const_iterator it = res.begin();
    while (it != res.end()) {
        SegrulesOptions::const_iterator it1 = it->first.begin();
        while (it1 != it->first.end()) {
            cerr << it1->first << " --> " << it1->second << endl;
            it1++;
        }
        cerr << it->second << endl;
        it++;
    }
}

}
