#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "models.h"
#include <cstddef>
#include <string>

bool isValidJadwal(const Jadwal& jadwal, const Jadwal data[],std::size_t count, std::string& reason, bool checkDuplicate = true, int ignoredIndex = -1);
bool isValidDate(const std::string& date, std::string& reason);
bool isValidPhone(const std::string& phone, std::string& reason);
int findJadwalById(const Jadwal data[], std::size_t count, int id);
bool addJadwal(Jadwal data[], std::size_t& count, const Jadwal& jadwal, std::string& reason);
bool updateJadwal(Jadwal data[], std::size_t count, int id, const Jadwal& replacement, std::string& reason);
bool deleteJadwal(Jadwal data[], std::size_t& count, int id, std::string& reason);
void printTable(const Jadwal data[], std::size_t count);
void searchJadwal(const Jadwal data[], std::size_t count, const std::string& keyword);
void sortByDateTime(Jadwal data[], std::size_t count);
void printStatistics(const Jadwal data[], std::size_t count);
void printMemoryInfo();

#endif
