#pragma once

#include <iostream>

// Toggle logger here
#define ENABLE_ERROR true
#define ENABLE_INFO true
#define ENABLE_DEBUG false

#if ENABLE_ERROR
#define Error(msg) std::cerr << "[ERROR] " << msg << std::endl
#else
#define Error(msg)
#endif

#if ENABLE_INFO
#define Info(msg) std::cout << "[INFO] " << msg << std::endl
#else
#define Info(msg)
#endif

#if ENABLE_DEBUG
#define Debug(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define Debug(msg)
#endif

// Reason for using #define for logging: no extra function will be called at runtime -> more efficient.
// All "Calls" to Info, Error, Debug will be replaced by the precompiler.
// IDK if it's actually smart, but it seems like a good idea at this time.