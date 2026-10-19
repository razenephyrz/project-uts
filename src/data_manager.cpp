#include "data_manager.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string lowerText(const std::string& text) {
    std::string result = text;
    for (char& character : result) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return result;
}

bool isDigits(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    for (char character : text) {
        if (!std::isdigit(static_cast<unsigned char>(character))) {
            return false;
        }
    }
    return true;
}

bool parseDate(const std::string& date, int& day, int& month, int& year) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-') {
        return false;
    }

    const std::string dayText = date.substr(0, 2);
    const std::string monthText = date.substr(3, 2);
    const std::string yearText = date.substr(6, 4);
    
    if (!isDigits(dayText) || !isDigits(monthText) || !isDigits(yearText)) {
        return false;
    }

    day = std::stoi(dayText);
    month = std::stoi(monthText);
    year = std::stoi(yearText);
    return true;
}

int dateKey(const std::string& date) {
    int day = 0;
    int month = 0;
    int year = 0;
    parseDate(date, day, month, year);
    return year * 10000 + month * 100 + day;
}

bool isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    return 31;
}

void printOneRow(const Jadwal& jadwal) {
    std::cout << std::left << std::setw(4) << jadwal.id
              << std::setw(24) << jadwal.namaMataKuliah.substr(0, 23)
              << std::setw(13) << (formatTime(jadwal.mulai) + "-" + formatTime(jadwal.selesai))
              << std::setw(12) << jadwal.hari
              << std::setw(12) << jadwal.tanggal
              << std::setw(22) << jadwal.namaDosen.substr(0, 21)
              << std::setw(20) << jadwal.penanggungJawab.substr(0, 19)
              << std::setw(16) << jadwal.nomorHp
              << jadwal.tempat << '\n';
}

} // namespace

bool isValidDate(const std::string& date, std::string& reason) {
    int day = 0;
    int month = 0;
    int year = 0;
    if (!parseDate(date, day, month, year)) {
        reason = "tanggal harus berformat DD-MM-YYYY";
        return false;
    }
    if (year < 2020 || year > 2100 || month < 1 || month > 12 ||
        day < 1 || day > daysInMonth(month, year)) {
        reason = "nilai tanggal berada di luar rentang";
        return false;
    }
    return true;
}

bool isValidPhone(const std::string& phone, std::string& reason) {
    if (phone.size() < 10 || phone.size() > 15 || !isDigits(phone)) {
        reason = "nomor HP harus 10-15 digit angka";
        return false;
    }
    return true;
}

bool normalizeDay(std::string& day, std::string& reason) {
    static const char* const names[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};

    std::size_t begin = 0;
    std::size_t end = day.size();
    while (begin < end && std::isspace(static_cast<unsigned char>(day[begin]))) {
        ++begin;
    }
    while (end > begin && std::isspace(static_cast<unsigned char>(day[end - 1]))) {
        --end;
    }

    const std::string key = lowerText(day.substr(begin, end - begin));
    for (const char* name : names) {
        if (key == lowerText(name)) {
            day = name;
            return true;
        }
    }
    reason = "hari harus salah satu dari Senin, Selasa, Rabu, Kamis, Jumat, Sabtu, Minggu";
    return false;
}

std::string formatTime(const Waktu& waktu) {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << static_cast<int>(waktu.jam) << ':'
        << std::setw(2) << static_cast<int>(waktu.menit);
    return out.str();
}

bool isDayMatchingDate(const std::string& hari, const std::string& tanggal, std::string& reason) {
    int day = 0;
    int month = 0;
    int year = 0;
    if (!parseDate(tanggal, day, month, year) || month < 1 || month > 12) {
        reason = "tanggal harus berformat DD-MM-YYYY";
        return false;
    }

    // Algoritma Sakamoto: 0 = Minggu, 1 = Senin, ..., 6 = Sabtu
    static const int offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    static const char* const names[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
    const int adjustedYear = month < 3 ? year - 1 : year;
    const int weekday = (adjustedYear + adjustedYear / 4 - adjustedYear / 100 +
                         adjustedYear / 400 + offsets[month - 1] + day) % 7;

    if (lowerText(hari) == lowerText(names[weekday])) {
        return true;
    }
    reason = "tanggal " + tanggal + " jatuh pada hari " + names[weekday] + ", bukan " + hari;
    return false;
}

int findScheduleConflict(const Jadwal data[], std::size_t count, const std::string& tanggal,
                         const Waktu& mulai, const Waktu& selesai, int ignoredIndex) {
    for (std::size_t index = 0; index < count; ++index) {
        if (static_cast<int>(index) == ignoredIndex) {
            continue;
        }
        const Jadwal& existing = data[index];
        // Bentrok bila rentang waktu tumpang tindih pada tanggal yang sama.
        // Selesai tepat saat jadwal lain mulai (berurutan) tidak dianggap bentrok.
        if (existing.tanggal == tanggal &&
            toMinutes(mulai) < toMinutes(existing.selesai) &&
            toMinutes(existing.mulai) < toMinutes(selesai)) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

bool isValidJadwal(const Jadwal& jadwal,
                  const Jadwal data[],
                  std::size_t count,
                  std::string& reason,
                  bool checkDuplicate,
                  int ignoredIndex) {
    if (jadwal.id <= 0) {
        reason = "ID harus lebih besar dari 0";
        return false;
    }
    if (checkDuplicate && findJadwalById(data, count, jadwal.id) >= 0) {
        reason = "ID jadwal sudah digunakan";
        return false;
    }
    if (jadwal.namaMataKuliah.empty() || jadwal.hari.empty() || jadwal.namaDosen.empty() ||
        jadwal.penanggungJawab.empty() || jadwal.tempat.empty()) {
        reason = "field teks tidak boleh kosong";
        return false;
    }
    std::string normalizedDay = jadwal.hari;
    if (!normalizeDay(normalizedDay, reason)) {
        return false;
    }
    if (jadwal.mulai.jam > 23 || jadwal.mulai.menit > 59 ||
        jadwal.selesai.jam > 23 || jadwal.selesai.menit > 59) {
        reason = "jam harus 0-23 dan menit harus 0-59";
        return false;
    }
    if (toMinutes(jadwal.selesai) <= toMinutes(jadwal.mulai)) {
        reason = "waktu selesai harus setelah waktu mulai";
        return false;
    }
    if (!isValidDate(jadwal.tanggal, reason) || !isValidPhone(jadwal.nomorHp, reason)) {
        return false;
    }
    if (!isDayMatchingDate(jadwal.hari, jadwal.tanggal, reason)) {
        return false;
    }
    if (findScheduleConflict(data, count, jadwal.tanggal, jadwal.mulai, jadwal.selesai, ignoredIndex) >= 0) {
        reason = "jadwal bentrok: waktu tumpang tindih dengan jadwal lain pada tanggal yang sama";
        return false;
    }
    return true;
}

int findJadwalById(const Jadwal data[], std::size_t count, int id) {
    for (std::size_t index = 0; index < count; ++index) {
        if (data[index].id == id) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

bool addJadwal(Jadwal data[], std::size_t& count, const Jadwal& jadwal, std::string& reason) {
    if (count >= MAX_JADWAL) {
        reason = "kapasitas array sudah penuh";
        return false;
    }
    if (!isValidJadwal(jadwal, data, count, reason)) {
        return false;
    }
    data[count] = jadwal;
    ++count;
    return true;
}

bool updateJadwal(Jadwal data[], std::size_t count, int id, const Jadwal& replacement, std::string& reason) {
    const int index = findJadwalById(data, count, id);
    if (index < 0) {
        reason = "ID jadwal tidak ditemukan";
        return false;
    }
    Jadwal candidate = replacement;
    candidate.id = id;
    if (!isValidJadwal(candidate, data, count, reason, false, index)) {
        return false;
    }
    data[index] = candidate;
    return true;
}

bool deleteJadwal(Jadwal data[], std::size_t& count, int id, std::string& reason) {
    const int index = findJadwalById(data, count, id);
    if (index < 0) {
        reason = "ID jadwal tidak ditemukan";
        return false;
    }
    for (std::size_t current = static_cast<std::size_t>(index); current + 1 < count; ++current) {
        data[current] = data[current + 1];
    }
    --count;
    return true;
}

void printTable(const Jadwal data[], std::size_t count) {
    if (count == 0) {
        std::cout << "Belum ada data jadwal.\n";
        return;
    }
    std::cout << std::left << std::setw(4) << "ID"
              << std::setw(24) << "Mata Kuliah"
              << std::setw(13) << "Waktu"
              << std::setw(12) << "Hari"
              << std::setw(12) << "Tanggal"
              << std::setw(22) << "Dosen"
              << std::setw(20) << "Penanggung Jawab"
              << std::setw(16) << "No. HP"
              << "Tempat\n";
    std::cout << std::string(150, '-') << '\n';
    for (std::size_t index = 0; index < count; ++index) {
        printOneRow(data[index]);
    }
}

void searchJadwal(const Jadwal data[], std::size_t count, const std::string& keyword) {
    const std::string query = lowerText(keyword);
    std::size_t matches = 0;
    for (std::size_t index = 0; index < count; ++index) {
        const Jadwal& jadwal = data[index];
        if (lowerText(jadwal.namaMataKuliah).find(query) != std::string::npos ||
            lowerText(jadwal.namaDosen).find(query) != std::string::npos ||
            lowerText(jadwal.hari).find(query) != std::string::npos ||
            lowerText(jadwal.tempat).find(query) != std::string::npos) {
            if (matches == 0) {
                printTable(data + index, 1);
            } else {
                printOneRow(jadwal);
            }
            ++matches;
        }
    }
    if (matches == 0) {
        std::cout << "Data dengan kata kunci tersebut tidak ditemukan.\n";
    } else {
        std::cout << "Jumlah data ditemukan: " << matches << '\n';
    }
}

void sortByDateTime(Jadwal data[], std::size_t count) {
    for (std::size_t index = 0; index < count; ++index) {
        std::size_t smallest = index;
        for (std::size_t current = index + 1; current < count; ++current) {
            const int currentDate = dateKey(data[current].tanggal);
            const int smallestDate = dateKey(data[smallest].tanggal);
            const int currentMinutes = toMinutes(data[current].mulai);
            const int smallestMinutes = toMinutes(data[smallest].mulai);
            if (currentDate < smallestDate ||
                (currentDate == smallestDate && currentMinutes < smallestMinutes)) {
                smallest = current;
            }
        }
        if (smallest != index) {
            std::swap(data[index], data[smallest]);
        }
    }
}

void printStatistics(const Jadwal data[], std::size_t count) {
    std::size_t morning = 0;
    std::size_t afternoon = 0;
    std::size_t evening = 0;
    for (std::size_t index = 0; index < count; ++index) {
        if (data[index].mulai.jam < 12) {
            ++morning;
        } else if (data[index].mulai.jam < 18) {
            ++afternoon;
        } else {
            ++evening;
        }
    }
    std::cout << "Total jadwal       : " << count << '\n'
              << "Jadwal pagi (00-11): " << morning << '\n'
              << "Jadwal siang (12-17): " << afternoon << '\n'
              << "Jadwal sore/malam  : " << evening << '\n';
}

void printMemoryInfo() {
    std::cout << "Ukuran struct Waktu       : " << sizeof(Waktu) << " byte\n"
              << "Ukuran struct Jadwal      : " << sizeof(Jadwal) << " byte\n"
              << "Kapasitas array           : " << MAX_JADWAL << " record\n"
              << "Total memori array Jadwal: " << sizeof(Jadwal) * MAX_JADWAL << " byte\n";
}
