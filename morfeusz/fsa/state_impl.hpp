/* 
 * File:   _state_impl.hpp
 * Author: mlenart
 *
 * Created on 21 październik 2013, 15:20
 */

#ifndef _STATE_IMPL_HPP
#define	_STATE_IMPL_HPP

#include <typeinfo>
#include <cstdio>
#include "fsa.hpp"

namespace morfeusz {

template <class T>
State<T>::State()
: offset(0), accepting(false), sink(false), value(), valueSize(0) {
}

template <class T>
State<T> State<T>::getSink() {
    State<T> res;
    res.setNextAsSink();
    return res;
}

template <class T>
inline bool State<T>::isSink() const {
    return this->sink;
}

template <class T>
inline bool State<T>::isAccepting() const {
    return this->accepting;
}

template <class T>
inline void State<T>::proceedToNext(const FSA<T>& fsa, const char c) {
    if (this->sink) {
        return;
    }
    else {
        fsa.proceedToNext(c, *this);
    }
}

template <class T>
unsigned long State<T>::getOffset() const {
    assert(!this->isSink());
    return this->offset;
}

template <class T>
inline const T& State<T>::getValue() const {
    assert(this->isAccepting());
    return this->value;
}

template <class T>
inline unsigned long State<T>::getValueSize() const {
    assert(this->isAccepting());
    return this->valueSize;
}

template <class T>
State<T>::~State() {

}

template <class T>
void State<T>::setNext(const unsigned long offset) {
    assert(!this->isSink());
//    this->sink = false;
    this->offset = offset;
    this->accepting = false;
}

template <class T>
void State<T>::setNext(const unsigned long offset, const T& value, const unsigned long valueSize) {
    assert(!this->isSink());
//    this->sink = false;
    this->offset = offset;
    this->accepting = true;
    this->value = value;
    this->valueSize = valueSize;
}

template <class T>
void State<T>::setNextAsSink() {
    this->sink = true;
    this->accepting = false;
}

}

#endif	/* _STATE_IMPL_HPP */

