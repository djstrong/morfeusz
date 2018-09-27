/* 
 * File:   ChunkBounds.hpp
 * Author: lennyn
 *
 * Created on June 27, 2014, 10:59 PM
 */

#ifndef CHUNKBOUNDS_HPP
#define	CHUNKBOUNDS_HPP

namespace morfeusz {

    struct ChunkBounds {
        const char* chunkStartPtr;
        const char* wordStartPtr;
        const char* wordEndPtr;
        const char* chunkEndPtr;
    };
    
}

#endif	/* CHUNKBOUNDS_HPP */

