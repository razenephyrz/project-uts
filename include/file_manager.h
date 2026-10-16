#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "models.h"
#include <cstddef>
#include <string>

bool loadData(const std::string& filePath, Jadwal data[], std::size_t& count, std::size_t& invalidCount);
bool saveData(const std::string& filePath, const Jadwal data[], std::size_t count);

#endif
