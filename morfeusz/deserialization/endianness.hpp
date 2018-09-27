/* 
 * File:   endianness.hpp
 * Author: lennyn
 *
 * Created on December 1, 2013, 8:45 PM
 */

#ifndef ENDIANNESS_HPP
#define	ENDIANNESS_HPP

#if defined _WIN64 || defined _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#endif	/* ENDIANNESS_HPP */

