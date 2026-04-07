#include "dir.hpp"

DirHolder Dir(const std::string& path, bool rec) {
    return DirHolder(path, rec);
}