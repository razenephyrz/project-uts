#include "file_manager.h"

#include "data_manager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

bool parseRecord(const std::string& line, Jadwal& jadwal, std::string& reason) {
    const std::vector<std::string> fields = splitLine(line);
    if (fields.size() != 10) {
        reason = "jumlah kolom harus 10";
        return false;
    }
    try {
        const int hour = std::stoi(fields[2]);
        const int minute = std::stoi(fields[3]);
        if (hour < 0 || hour > 255 || minute < 0 || minute > 255) {
            reason = "nilai waktu bukan bilangan byte yang valid";
            return false;
        }
        jadwal.id = std::stoi(fields[0]);
        jadwal.waktu.jam = static_cast<std::uint8_t>(hour);
        jadwal.waktu.menit = static_cast<std::uint8_t>(minute);
        jadwal.namaMataKuliah = fields[1];
        jadwal.hari = fields[4];
        jadwal.tanggal = fields[5];
        jadwal.namaDosen = fields[6];
        jadwal.penanggungJawab = fields[7];
        jadwal.nomorHp = fields[8];
        jadwal.tempat = fields[9];
    } catch (const std::exception&) {
        reason = "ID atau waktu bukan angka yang valid";
        return false;
    }
    return true;
}

} // namespace

bool loadData(const std::string& filePath,
              Jadwal data[],
              std::size_t& count,
              std::size_t& invalidCount) {
    std::ifstream input(filePath);
    if (!input) {
        std::cerr << "File data belum tersedia: " << filePath << '\n';
        return false;
    }

    count = 0;
    invalidCount = 0;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }
        Jadwal jadwal{};
        std::string reason;
        if (!parseRecord(line, jadwal, reason) ||
            !isValidJadwal(jadwal, data, count, reason)) {
            ++invalidCount;
            std::cerr << "Baris " << lineNumber << " dilewati: " << reason << '\n';
            continue;
        }
        if (!addJadwal(data, count, jadwal, reason)) {
            ++invalidCount;
            std::cerr << "Baris " << lineNumber << " dilewati: " << reason << '\n';
        }
    }
    return true;
}

bool saveData(const std::string& filePath, const Jadwal data[], std::size_t count) {
    std::ofstream output(filePath);
    if (!output) {
        std::cerr << "Gagal membuka file untuk menyimpan data: " << filePath << '\n';
        return false;
    }
    for (std::size_t index = 0; index < count; ++index) {
        const Jadwal& jadwal = data[index];
        output << jadwal.id << '|'
               << jadwal.namaMataKuliah << '|'
               << static_cast<int>(jadwal.waktu.jam) << '|'
               << static_cast<int>(jadwal.waktu.menit) << '|'
               << jadwal.hari << '|'
               << jadwal.tanggal << '|'
               << jadwal.namaDosen << '|'
               << jadwal.penanggungJawab << '|'
               << jadwal.nomorHp << '|'
               << jadwal.tempat << '\n';
    }
    return true;
}
