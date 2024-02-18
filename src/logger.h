#pragma once

#include <iostream>

// Toggle logger here
#define ENABLE_LOGGER true

#if ENABLE_LOGGER
#define Error(msg) std::cerr << "[ERROR] " << msg << std::endl
#define Info(msg) std::cout << "[INFO] " << msg << std::endl
#define Debug(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define Info(msg)
#define Error(msg)
#define Debug(msg)
#endif

// Reason for using #define for logging: no extra function will be called at runtime -> more efficient.
// All "Calls" to Info, Error, Debug will be replaced by the precompiler.
// IDK if it's actually smart, but it seems like a good idea at this time.