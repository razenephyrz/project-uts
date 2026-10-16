#include "menu.h"

#include "data_manager.h"

#include <cctype>
#include <iostream>
#include <limits>
#include <string>

namespace {

std::string readText(const std::string& prompt) {
    std::string value;
    do {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (value.empty()) {
            std::cout << "Input tidak boleh kosong.\n";
        }
    } while (value.empty() && std::cin);
    return value;
}

int readInteger(const std::string& prompt, int minimum, int maximum) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minimum && value <= maximum) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Masukkan angka antara " << minimum << " dan " << maximum << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

Jadwal readJadwalInput(int id) {
    Jadwal jadwal{};
    jadwal.id = id;
    jadwal.namaMataKuliah = readText("Nama mata kuliah : ");
    jadwal.waktu.jam = static_cast<std::uint8_t>(readInteger("Jam (0-23)       : ", 0, 23));
    jadwal.waktu.menit = static_cast<std::uint8_t>(readInteger("Menit (0-59)     : ", 0, 59));
    jadwal.hari = readText("Hari              : ");
    jadwal.tanggal = readText("Tanggal (DD-MM-YYYY): ");
    jadwal.namaDosen = readText("Dosen pengampu    : ");
    jadwal.penanggungJawab = readText("Penanggung jawab  : ");
    jadwal.nomorHp = readText("Nomor HP           : ");
    jadwal.tempat = readText("Tempat kelas       : ");
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
    while (running) {
        printMenu();
        const int choice = readInteger("Pilih menu: ", 0, 8);
        switch (choice) {
        case 1: {
            const int id = readInteger("ID jadwal        : ", 1, 999999);
            const Jadwal jadwal = readJadwalInput(id);
            std::string reason;
            printResult(addJadwal(data, count, jadwal, reason),
                        reason.empty() ? "jadwal ditambahkan" : reason);
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
            const Jadwal replacement = readJadwalInput(id);
            std::string reason;
            printResult(updateJadwal(data, count, id, replacement, reason),
                        reason.empty() ? "jadwal diperbarui" : reason);
            break;
        }
        case 6: {
            const int id = readInteger("ID jadwal yang dihapus: ", 1, 999999);
            std::string reason;
            printResult(deleteJadwal(data, count, id, reason),
                        reason.empty() ? "jadwal dihapus" : reason);
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
}
