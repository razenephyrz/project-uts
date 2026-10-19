# Program Pergantian Jadwal Kelas

Program konsol C++ untuk mengelola data pergantian jadwal kelas mahasiswa
Kecerdasan Artifisial. Data disimpan permanen dalam `data/jadwal.txt`.

## Fitur

- Menambah jadwal dengan validasi input dan pesan yang menjelaskan kesalahan
  format, rentang, tanggal, nomor HP, atau bentrok jadwal.
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

Array data memiliki kapasitas tetap 100 record. Struct `Jadwal` memuat dua
struct `Waktu`, yaitu `mulai` dan `selesai`, sehingga jam dan menit
dikelompokkan sebagai satu anggota.

## Format Data

Setiap baris menggunakan delimiter `|` dengan urutan:

```text
id|nama_mata_kuliah|jam_mulai|menit_mulai|jam_selesai|menit_selesai|hari|tanggal|nama_dosen|penanggung_jawab|nomor_hp|tempat
```

Tanggal menggunakan format `DD-MM-YYYY`. Nomor HP disimpan sebagai teks agar
angka nol di awal tetap dipertahankan.

File format lama (10 kolom, tanpa jam selesai) tetap bisa dibaca. Jam selesai
diisi otomatis dengan durasi bawaan 90 menit dan akan tersimpan dalam format
baru (12 kolom) saat program ditutup.

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
fungsi, string, file I/O, header file, dan Makefile. Data awal berisi 17 baris
valid. Baris yang tidak valid (jumlah kolom salah, waktu di luar rentang, ID
duplikat, hari tidak cocok dengan tanggal, atau jadwal bentrok) dilaporkan lalu
dilewati saat program membaca file.

Jadwal baru atau jadwal hasil perubahan ditolak jika rentang waktunya
tumpang tindih dengan jadwal lain pada tanggal yang sama. Jadwal yang berurutan
(selesai tepat saat jadwal lain mulai) tidak dianggap bentrok.

## Validasi Langsung Saat Input

Saat menambah atau mengubah jadwal, setiap input langsung divalidasi terhadap
data yang sudah ada, sehingga pengguna tidak perlu mengisi ulang semua kolom:

- ID yang sudah dipakai langsung ditolak (tambah) atau ID yang tidak ada langsung
  ditolak (ubah).
- Kapasitas array dicek sebelum input dimulai.
- Hari harus Senin sampai Minggu (huruf besar/kecil otomatis dirapikan).
- Hari harus cocok dengan tanggal. Contoh: `Senin` dengan `18-10-2026` ditolak
  karena tanggal tersebut hari Minggu; hari dan tanggal diminta ulang.
- Tanggal dan nomor HP dicek per kolom.
- Jam selesai harus setelah jam mulai (dalam hari yang sama).
- Bentrok jadwal (rentang waktu tumpang tindih pada tanggal yang sama) langsung
  diberi tahu beserta ID, mata kuliah, dan jam jadwal yang bentrok; jam mulai
  dan selesai diminta ulang.
- Karakter `|` ditolak pada kolom teks karena dipakai sebagai pemisah file.
