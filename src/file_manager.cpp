#include "file_manager.h"

#include "data_manager.h"

#include <algorithm>
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
    // Format baru : 12 kolom (ada jam & menit selesai).
    // Format lama : 10 kolom (tanpa jam selesai), selesai diisi durasi bawaan.
    const bool legacy = fields.size() == 10;
    if (fields.size() != 12 && !legacy) {
        reason = "jumlah kolom harus 12 (format lama 10 kolom juga diterima)";
        return false;
    }
    const std::size_t shift = legacy ? 0 : 2;
    try {
        const int startHour = std::stoi(fields[2]);
        const int startMinute = std::stoi(fields[3]);
        int endHour = 0;
        int endMinute = 0;
        if (legacy) {
            const int endTotal = std::min(startHour * 60 + startMinute + DURASI_DEFAULT_MENIT, 23 * 60 + 59);
            endHour = endTotal / 60;
            endMinute = endTotal % 60;
        } else {
            endHour = std::stoi(fields[4]);
            endMinute = std::stoi(fields[5]);
        }
        if (startHour < 0 || startHour > 255 || startMinute < 0 || startMinute > 255 ||
            endHour < 0 || endHour > 255 || endMinute < 0 || endMinute > 255) {
            reason = "nilai waktu bukan bilangan byte yang valid";
            return false;
        }
        jadwal.id = std::stoi(fields[0]);
        jadwal.mulai.jam = static_cast<std::uint8_t>(startHour);
        jadwal.mulai.menit = static_cast<std::uint8_t>(startMinute);
        jadwal.selesai.jam = static_cast<std::uint8_t>(endHour);
        jadwal.selesai.menit = static_cast<std::uint8_t>(endMinute);
        jadwal.namaMataKuliah = fields[1];
        jadwal.hari = fields[4 + shift];
        jadwal.tanggal = fields[5 + shift];
        jadwal.namaDosen = fields[6 + shift];
        jadwal.penanggungJawab = fields[7 + shift];
        jadwal.nomorHp = fields[8 + shift];
        jadwal.tempat = fields[9 + shift];
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
               << static_cast<int>(jadwal.mulai.jam) << '|'
               << static_cast<int>(jadwal.mulai.menit) << '|'
               << static_cast<int>(jadwal.selesai.jam) << '|'
               << static_cast<int>(jadwal.selesai.menit) << '|'
               << jadwal.hari << '|'
               << jadwal.tanggal << '|'
               << jadwal.namaDosen << '|'
               << jadwal.penanggungJawab << '|'
               << jadwal.nomorHp << '|'
               << jadwal.tempat << '\n';
    }
    return true;
}
