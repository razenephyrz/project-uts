# Program Pergantian Jadwal Kelas

Program konsol C++ untuk mengelola data pergantian jadwal kelas mahasiswa
Kecerdasan Artifisial. Data disimpan permanen dalam `data/jadwal.txt`.

## Fitur

- Menambah jadwal dengan validasi input.
- Menampilkan data dalam tabel.
- Mencari berdasarkan mata kuliah, dosen, hari, atau tempat tanpa membedakan huruf kapital.
- Mengurutkan data berdasarkan tanggal dan jam menggunakan selection sort.
- Mengubah dan menghapus jadwal berdasarkan ID unik.
- Menampilkan statistik jadwal.
- Menampilkan ukuran struct dan total memori array.
- Membaca dan menyimpan file teks, termasuk melewati baris invalid tanpa menghentikan program.

## Struktur Proyek

```text
.
|-- Makefile
|-- data/
|   `-- jadwal.txt
|-- src/
|   |-- data_manager.cpp
|   |-- data_manager.h
|   |-- file_manager.cpp
|   |-- file_manager.h
|   |-- main.cpp
|   |-- menu.cpp
|   |-- menu.h
|   `-- models.h
```

Array data memiliki kapasitas tetap 100 record. Struct `Jadwal` memuat struct
`Waktu`, sehingga data jam dan menit dikelompokkan sebagai satu anggota.

## Format Data

Setiap baris menggunakan delimiter `|` dengan urutan:

```text
id|nama_mata_kuliah|jam|menit|hari|tanggal|nama_dosen|penanggung_jawab|nomor_hp|tempat
```

Tanggal menggunakan format `DD-MM-YYYY`. Nomor HP disimpan sebagai teks agar
angka nol di awal tetap dipertahankan.

## Kompilasi dan Eksekusi

Gunakan MinGW g++ dan GNU Make dari direktori utama proyek:

```text
make
make run
make clean
```

Target `make` membangun object file dan executable `pergantian_jadwal`.
Target `make run` membangun program jika diperlukan lalu menjalankannya.
Target `make clean` menghapus executable dan object file.

## Ketentuan UTS yang Diimplementasikan

Program menggunakan dua struct, array of struct, percabangan, perulangan,
fungsi, string, file I/O, header file, dan Makefile. Data awal memiliki 20
baris, termasuk tiga baris invalid berupa jumlah kolom salah, waktu di luar
rentang, dan ID duplikat. Ketiga baris tersebut dilaporkan lalu dilewati saat
program membaca file.

Revisi Bagian Tambah Matkul Yang Bentrok