#pragma once

#include <cstdio>
#include <algorithm>
#include <string>
#include <unistd.h>

#ifdef _WIN32

#include <windows.h>

#endif

std::string getPath() {
    char pBuf[256];
    size_t len = sizeof(pBuf);
#ifdef _WIN32
    int bytes = GetModuleFileName(NULL, pBuf, len);
#elif defined(__unix__)
    int bytes = (std::min<unsigned int>)(readlink("/proc/self/exe", pBuf, len), len - 1);
    if (bytes >= 0)
        pBuf[bytes] = '\0';
#endif
    std::string path(pBuf);
    std::replace(path.begin(), path.end(), '\\', '/');
    path.erase(path.find_last_of('/') + 1);
    return path;
}
