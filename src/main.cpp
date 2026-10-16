#include "data_manager.h"
#include "file_manager.h"
#include "menu.h"

#include <iostream>

int main() {

    Jadwal data[MAX_JADWAL]{};
    std::size_t count = 0;
    std::size_t invalidCount = 0;
    const std::string dataFile = "data/jadwal.txt";

    loadData(dataFile, data, count, invalidCount);
    std::cout << "Data valid dimuat: " << count
              << " record; data invalid dilewati: " << invalidCount << " baris.\n";

    runMenu(data, count);
    if (saveData(dataFile, data, count)) {
        std::cout << "Data berhasil disimpan. Sampai jumpa.\n";
        return 0;
    }
    return 1;
}
