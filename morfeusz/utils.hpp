/* 
 * File:   utils.hpp
 * Author: mlenart
 *
 * Created on 24 październik 2013, 17:56
 */

#ifndef UTILS_HPP
#define	UTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

#ifdef DEBUG_BUILD
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#else
#define DEBUG(x)
#endif

namespace morfeusz {

    inline void validate(const bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << std::endl;
            throw msg;
        }
    }

    inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    inline std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }

    //string &rtrim(string &s) {
    //        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    //        return s;
    //}

    template <class T>
    inline T* readFile(const char* fname) {
        std::ifstream ifs;
        ifs.open(fname, std::ios::in | std::ios::binary | std::ios::ate);
        if (ifs.is_open()) {
            long size = ifs.tellg();
            T* memblock = new T[size];
            ifs.seekg(0, std::ios::beg);
            ifs.read(reinterpret_cast<char*> (memblock), size);
            ifs.close();
            return memblock;
        }
        else {
            std::stringstream ss;
            ss << "Failed to open file: " << fname;
            throw std::ios_base::failure(ss.str());
        }
    }

}

#endif	/* UTILS_HPP */

