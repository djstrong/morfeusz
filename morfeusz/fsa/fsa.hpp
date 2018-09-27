/* 
 * File:   fsa.hh
 * Author: mlenart
 *
 * Created on October 17, 2013, 2:00 PM
 */

#ifndef FSA_HPP
#define FSA_HPP

//#include <iostream>
#include <cstring>
#include <cassert>
#include <typeinfo>
#include <exception>
#include <string>
#include <vector>
#include <inttypes.h>

#include "morfeusz2.h"

namespace morfeusz {

template <class T> class State;
template <class T> class FSA;
template <class T> class Deserializer;

template <class T>
class Deserializer {
public:

    /**
     * Deserialize object from ptr.
     * Returns number of bytes read or -1 on error.
     */
    virtual long deserialize(const unsigned char* ptr, T& object) const = 0;
    virtual ~Deserializer() {}
};

/**
 * Finite state automaton.
 */
template <class T>
class FSA {
public:
    /**
     * Get this automaton's initial state.
     */
    State<T> getInitialState() const;

    bool tryToRecognize(const char* input, T& value) const;

    virtual ~FSA() {
    }
    
    /**
     * Create an FSA object from pointer
     */
    static FSA<T>* getFSA(const unsigned char* ptr, const Deserializer<T>& deserializer);
    
    /**
     * Create an FSA object from file
     */
    static FSA<T>* getFSA(const std::string& filename, const Deserializer<T>& deserializer);
    
protected:

    /**
     * Create FSA object for givent initial state pointer and deserializer
     */
    FSA(const unsigned char* initialStatePtr, const Deserializer<T>& deserializer);
    
    /**
     * Proceed to next state
     */
    virtual void proceedToNext(const char c, State<T>& state) const = 0;
    const unsigned char* initialStatePtr;
    const Deserializer<T>& deserializer;
    friend class State<T>;
private:
    //    FSA();
};

template <class T>
class SimpleFSA : public FSA<T> {
public:
    SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer, bool isTransducer=false);
    virtual ~SimpleFSA();
protected:
    void proceedToNext(const char c, State<T>& state) const;
private:
    bool isTransducer;
};

template <class T>
class CompressedFSA1 : public FSA<T> {
public:
    CompressedFSA1(const unsigned char* ptr, const Deserializer<T>& deserializer);
    virtual ~CompressedFSA1();
protected:
    void proceedToNext(const char c, State<T>& state) const;
private:
    const std::vector<unsigned char> label2ShortLabel;
    
    std::vector< char > hasTransition;
    std::vector< State<T> > initialTransitions;
    
    static std::vector<unsigned char> initializeChar2PopularCharIdx(const unsigned char* ptr);
    
    void initializeInitialTransitions();
    
    void doProceedToNext(const char c, State<T>& state, bool initial) const;
    
    void doProceedToNextByList(
        const char c,
        const unsigned char shortLabel,
        const unsigned char* ptr,
        const unsigned int transitionsNum,
        State<T>& state) const;
    void reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const;
    void doProceedToNextByArray(
        const unsigned char shortLabel,
        const uint32_t* ptr,
        State<T>& state) const;
};


template <class T>
class CompressedFSA2 : public FSA<T> {
public:
    CompressedFSA2(const unsigned char* ptr, const Deserializer<T>& deserializer);
    virtual ~CompressedFSA2();
    
protected:
    void proceedToNext(const char c, State<T>& state) const;
    
private:
    
    static std::vector<unsigned char> initializeChar2PopularCharIdx(const unsigned char* ptr);
    
    void doProceedToNextByList(
        const char c,
        const unsigned char* ptr, 
        State<T>& state) const;
    void reallyDoProceed(
        const unsigned char* statePtr,
        const bool accepting,
        State<T>& state) const;
};

/**
 * A state in an FSA.
 */
template <class T>
class State {
public:

    /**
     * Is this a "sink" state - non-accepting state without outgoing transitions
     */
    inline bool isSink() const;

    /**
     * Is this an accepting state
     */
    inline bool isAccepting() const;

    /**
     * Get next state proceeding a transition for given character.
     */
    inline void proceedToNext(const FSA<T>& fsa, const char c);

    /**
     * Get value of this state.
     * Makes sense only for accepting states.
     * For non-accepting states is throws an exception.
     */
    inline const T& getValue() const;

    /**
     * Get the size (in bytes) of this state's value.
     * Makes sense only for accepting states.
     * For non-accepting states is throws an exception.
     */
    inline unsigned long getValueSize() const;

    inline unsigned long getOffset() const;

    void setNext(const unsigned long offset);
    void setNext(const unsigned long offset, const T& value, const unsigned long valueSize);
    void setNextAsSink();

    State();
    
    static State<T> getSink();

    virtual ~State();
    
    friend class CompressedFSA1<T>;
private:
    
//    const FSA<T>& fsa;
    unsigned long offset;
    bool accepting;
    bool sink;
    T value;
    long valueSize;
};

}

#include "state_impl.hpp"
#include "fsa_impl.hpp"
#include "simplefsa_impl.hpp"
#include "cfsa1_impl.hpp"
#include "cfsa2_impl.hpp"

#endif	/* FSA_HPP */


