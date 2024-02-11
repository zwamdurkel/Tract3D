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
