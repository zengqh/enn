/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_std_header.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_os_headers.h"

/** standard c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>

/** standard c++ */
#include <limits>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <bitset>
#include <cstdint>
#include <tuple>
#include <utility>
#include <memory>

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#if _MSC_VER >= 1600 // vc10
#include <unordered_map>
#include <unordered_set>
#else // vc9
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#endif

#define unordered_map_  std::tr1::unordered_map
#define unordered_set_  std::tr1::unordered_set
#define hash_           std::tr1::hash
#else
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#define unordered_map_  std::unordered_map
#define unordered_set_  std::unordered_set
#define hash_           std::hash
#endif


//////////////////////////////////////////////////////////////////////////
namespace enn
{
	using							std::uint64_t;
	using							std::uint32_t;
	using							std::uint16_t;
	using							std::uint8_t;
	using							std::int64_t;
	using							std::int32_t;
	using							std::int16_t;
	using							std::int8_t;

	typedef int8_t 					int8;
	typedef int16_t       			int16;
	typedef int32_t         		int32;
	typedef int64_t   				int64;

	typedef uint8_t       			uint8;
	typedef uint16_t      			uint16;
	typedef uint32_t        		uint32;
	typedef uint64_t  				uint64;

	typedef unsigned char   		uchar;
	typedef unsigned short  		ushort;
	typedef unsigned int    		uint;
	typedef unsigned long   		ulong;

	typedef char*       			pstr;
	typedef const char* 			pcstr;
}
