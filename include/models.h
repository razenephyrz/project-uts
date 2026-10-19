#ifndef MODELS_H
#define MODELS_H
#include<bits/stdc++.h>

constexpr std::size_t MAX_JADWAL = 100;
constexpr int DURASI_DEFAULT_MENIT = 90; // dipakai saat membaca data format lama (tanpa jam selesai)

struct Waktu {
    std::uint8_t jam;
    std::uint8_t menit;
};

inline int toMinutes(const Waktu& waktu) {
    return waktu.jam * 60 + waktu.menit;
}

struct Jadwal {
    int id;
    Waktu mulai;
    Waktu selesai;
    std::string namaMataKuliah;
    std::string hari;
    std::string tanggal;
    std::string namaDosen;
    std::string penanggungJawab;
    std::string nomorHp;
    std::string tempat;
};

#endif
