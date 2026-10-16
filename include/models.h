#ifndef MODELS_H
#define MODELS_H

#include<bits/stdc++.h>

constexpr std::size_t MAX_JADWAL = 100;

struct Waktu {
    std::uint8_t jam;
    std::uint8_t menit;
};

struct Jadwal {
    int id;
    Waktu waktu;
    std::string namaMataKuliah;
    std::string hari;
    std::string tanggal;
    std::string namaDosen;
    std::string penanggungJawab;
    std::string nomorHp;
    std::string tempat;
};

#endif
