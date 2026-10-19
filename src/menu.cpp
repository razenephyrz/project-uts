#include "menu.h"

#include "data_manager.h"

#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

namespace {

struct InputEnded {};

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    if (!std::getline(std::cin, value)) {
        throw InputEnded{};
    }
    return value;
}

bool isBlank(const std::string& value) {
    for (char character : value) {
        if (!std::isspace(static_cast<unsigned char>(character))) {
            return false;
        }
    }
    return true;
}

std::string readText(const std::string& prompt) {
    while (true) {
        const std::string value = readLine(prompt);
        if (!isBlank(value)) {
            return value;
        }
        std::cout << "Input tidak boleh kosong atau hanya berisi spasi.\n";
    }
}

int readInteger(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        const std::string input = readLine(prompt);
        const char* begin = input.data();
        const char* end = begin + input.size();
        while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) {
            ++begin;
        }
        while (begin != end && std::isspace(static_cast<unsigned char>(end[-1]))) {
            --end;
        }
        if (begin == end) {
            std::cout << "Input tidak boleh kosong; masukkan bilangan bulat.\n";
            continue;
        }

        const std::string trimmed(begin, end);
        if (trimmed[0] == '+') {
            std::cout << "Format input salah; masukkan bilangan bulat tanpa karakter tambahan.\n";
            continue;
        }

        errno = 0;
        char* parsedEnd = nullptr;
        const long parsedValue = std::strtol(trimmed.c_str(), &parsedEnd, 10);
        if (errno == ERANGE ||
            parsedValue < std::numeric_limits<int>::min() ||
            parsedValue > std::numeric_limits<int>::max()) {
            std::cout << "Angka terlalu besar atau terlalu kecil untuk diproses.\n";
            continue;
        }
        if (parsedEnd == trimmed.c_str() ||
            parsedEnd != trimmed.c_str() + trimmed.size()) {
            std::cout << "Format input salah; masukkan bilangan bulat tanpa karakter tambahan.\n";
            continue;
        }
        const int value = static_cast<int>(parsedValue);
        if (value < minimum || value > maximum) {
            std::cout << "Nilai harus antara " << minimum << " dan " << maximum << ".\n";
            continue;
        }
        return value;
    }
}

std::string readDate(const std::string& prompt) {
    while (true) {
        const std::string date = readText(prompt);
        std::string reason;
        if (isValidDate(date, reason)) {
            return date;
        }
        std::cout << "Tanggal tidak valid: " << reason << ".\n";
    }
}

std::string readPhone(const std::string& prompt) {
    while (true) {
        const std::string phone = readText(prompt);
        std::string reason;
        if (isValidPhone(phone, reason)) {
            return phone;
        }
        std::cout << "Nomor HP tidak valid: " << reason << ".\n";
    }
}

std::string readField(const std::string& prompt) {
    while (true) {
        const std::string value = readText(prompt);
        if (value.find('|') == std::string::npos) {
            return value;
        }
        std::cout << "Input tidak boleh mengandung karakter '|' (dipakai sebagai pemisah file).\n";
    }
}

std::string readDay(const std::string& prompt) {
    while (true) {
        std::string day = readText(prompt);
        std::string reason;
        if (normalizeDay(day, reason)) {
            return day;
        }
        std::cout << "Hari tidak valid: " << reason << ".\n";
    }
}

int readNewId(const Jadwal data[], std::size_t count) {
    while (true) {
        const int id = readInteger("ID jadwal        : ", 1, 999999);
        if (findJadwalById(data, count, id) < 0) {
            return id;
        }
        std::cout << "ID " << id << " sudah digunakan; masukkan ID lain.\n";
    }
}

void readDayAndDate(std::string& hari, std::string& tanggal) {
    while (true) {
        hari = readDay("Hari              : ");
        tanggal = readDate("Tanggal (DD-MM-YYYY): ");
        std::string reason;
        if (isDayMatchingDate(hari, tanggal, reason)) {
            return;
        }
        std::cout << "Hari dan tanggal tidak cocok: " << reason
                  << ". Masukkan hari dan tanggal lagi.\n";
    }
}

void readTimeRange(const Jadwal data[], std::size_t count, const std::string& tanggal,
                   int ignoredIndex, Waktu& mulai, Waktu& selesai) {
    while (true) {
        mulai.jam = static_cast<std::uint8_t>(readInteger("Jam mulai (0-23)     : ", 0, 23));
        mulai.menit = static_cast<std::uint8_t>(readInteger("Menit mulai (0-59)   : ", 0, 59));

        while (true) {
            selesai.jam = static_cast<std::uint8_t>(readInteger("Jam selesai (0-23)   : ", 0, 23));
            selesai.menit = static_cast<std::uint8_t>(readInteger("Menit selesai (0-59) : ", 0, 59));
            if (toMinutes(selesai) > toMinutes(mulai)) {
                break;
            }
            std::cout << "Waktu selesai harus setelah waktu mulai (" << formatTime(mulai) << ").\n";
        }

        const int conflict = findScheduleConflict(data, count, tanggal, mulai, selesai, ignoredIndex);
        if (conflict < 0) {
            return;
        }
        std::cout << "Jadwal bentrok dengan ID " << data[conflict].id << " ("
                  << data[conflict].namaMataKuliah << ", " << formatTime(data[conflict].mulai) << "-"
                  << formatTime(data[conflict].selesai) << ") pada " << tanggal
                  << ". Masukkan waktu lain.\n";
    }
}

Jadwal readJadwalInput(const Jadwal data[], std::size_t count, int id, int ignoredIndex = -1) {
    Jadwal jadwal{};
    jadwal.id = id;
    jadwal.namaMataKuliah = readField("Nama mata kuliah : ");
    readDayAndDate(jadwal.hari, jadwal.tanggal);
    readTimeRange(data, count, jadwal.tanggal, ignoredIndex, jadwal.mulai, jadwal.selesai);
    jadwal.namaDosen = readField("Dosen pengampu    : ");
    jadwal.penanggungJawab = readField("Penanggung jawab  : ");
    jadwal.nomorHp = readPhone("Nomor HP           : ");
    jadwal.tempat = readField("Tempat kelas       : ");
    return jadwal;
}

void printResult(bool success, const std::string& message) {
    std::cout << (success ? "Berhasil: " : "Gagal: ") << message << '\n';
}

void printMenu() {
    std::cout << "\n=== PERGANTIAN JADWAL KELAS AI ===\n"
              << "1. Tambah jadwal\n"
              << "2. Tampilkan semua jadwal\n"
              << "3. Cari jadwal\n"
              << "4. Urutkan berdasarkan tanggal dan jam\n"
              << "5. Ubah jadwal\n"
              << "6. Hapus jadwal\n"
              << "7. Statistik\n"
              << "8. Info Memori\n"
              << "0. Simpan dan keluar\n";
}

} // namespace

void runMenu(Jadwal data[], std::size_t& count) {
    bool running = true;
    try {
        while (running) {
            printMenu();
            const int choice = readInteger("Pilih menu: ", 0, 8);
            switch (choice) {
            case 1: {
                if (count >= MAX_JADWAL) {
                    printResult(false, "kapasitas array sudah penuh");
                    break;
                }
                const int id = readNewId(data, count);
                const Jadwal jadwal = readJadwalInput(data, count, id);
                std::string reason;
                const bool success = addJadwal(data, count, jadwal, reason);
                printResult(success, success ? "jadwal ditambahkan" : reason);
                break;
            }
            case 2:
                printTable(data, count);
                break;
            case 3: {
                const std::string keyword = readText("Kata kunci mata kuliah/dosen/hari/tempat: ");
                searchJadwal(data, count, keyword);
                break;
            }
            case 4:
                sortByDateTime(data, count);
                std::cout << "Data berhasil diurutkan.\n";
                break;
            case 5: {
                const int id = readInteger("ID jadwal yang diubah: ", 1, 999999);
                const int index = findJadwalById(data, count, id);
                if (index < 0) {
                    printResult(false, "ID jadwal tidak ditemukan");
                    break;
                }
                const Jadwal replacement = readJadwalInput(data, count, id, index);
                std::string reason;
                const bool success = updateJadwal(data, count, id, replacement, reason);
                printResult(success, success ? "jadwal diperbarui" : reason);
                break;
            }
            case 6: {
                const int id = readInteger("ID jadwal yang dihapus: ", 1, 999999);
                std::string reason;
                const bool success = deleteJadwal(data, count, id, reason);
                printResult(success, success ? "jadwal dihapus" : reason);
                break;
            }
            case 7:
                printStatistics(data, count);
                break;
            case 8:
                printMemoryInfo();
                break;
            case 0:
                running = false;
                break;
            default:
                break;
            }
        }
    } catch (const InputEnded&) {
        std::cout << "\nInput dihentikan. Data akan disimpan sebelum program ditutup.\n";
    }
}
