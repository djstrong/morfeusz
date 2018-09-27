/* 
 * File:   ResultsManager.hpp
 * Author: mlenart
 *
 * Created on 24 czerwiec 2014, 17:09
 */

#ifndef RESULTSMANAGER_HPP
#define	RESULTSMANAGER_HPP

#include <vector>
#include "morfeusz2.h"
#include "morfeusz2_c.h"

namespace morfeusz {

    class ResultsManager {
    public:
        ResultsManager(const Morfeusz* morfeusz);
        InterpMorf* convertResults(const std::vector<MorphInterpretation>& res);
        virtual ~ResultsManager();
    private:
        const Morfeusz* morfeusz;
        InterpMorf* results;
        unsigned int resultsArraySize;
        
        void reserve(size_t size);
        InterpMorf convertOneResult(const MorphInterpretation& res);
    };

}

#endif	/* RESULTSMANAGER_HPP */

