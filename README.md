# External Loader untuk STM32H523CET6 + Macronix Quad-SPI Flash

External Loader (`MX25L_STM32H523CET6.stldr`) ini dibuat khusus agar **STM32CubeProgrammer** dan **STM32CubeIDE Debugger** dapat membaca, menghapus (erase), dan memprogram Flash Eksternal (Macronix Quad-SPI 32MB) di alamat `0x90000000` secara langsung via ST-LINK (SWD).

---

## 1. Spesifikasi Hardware

* **Target MCU**: STM32H523CET6 (LQFP48)
* **Flash Eksternal**: Macronix MX25L / Quad-SPI (32 MBytes)
* **Base Address**: `0x90000000`
* **Sector Size**: 4096 Bytes (4 KB)
* **Page Size**: 256 Bytes

### Konfigurasi Pin OctoSPI1
| Pin STM32 | Fungsi Pin OctoSPI | Alternate Function |
| :--- | :--- | :--- |
| **PA1** | OCTOSPI1_IO3 | AF9 |
| **PA3** | OCTOSPI1_CLK | AF3 |
| **PA7** | OCTOSPI1_IO2 | AF10 |
| **PB0** | OCTOSPI1_IO1 | AF6 |
| **PB1** | OCTOSPI1_IO0 | AF6 |
| **PB10** | OCTOSPI1_NCS | AF9 |

---

## 2. Cara Build & Install

Untuk compile dan install otomatis ke direktori STM32CubeProgrammer dan STM32CubeIDE:
```bash
make clean
make install
```

File `.stldr` akan otomatis disalin ke:
1. `~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader/MX25L_STM32H523CET6.stldr`
2. `/opt/st/stm32cubeide_2.2.0_2/plugins/.../tools/bin/ExternalLoader/MX25L_STM32H523CET6.stldr`

---

## 3. Cara Menggunakan di STM32CubeProgrammer

### A. Melalui GUI (Aplikasi Grafis)
1. Buka **STM32CubeProgrammer**.
2. Di menu sebelah kiri, klik tab **"External Loaders"** (ikon IC flash).
3. Cari nama **`MX25L_STM32H523CET6`**, lalu centang kotaknya.
4. Hubungkan ST-LINK dan klik **Connect**.
5. Buka tab **"Erasing & Programming"** atau **"Memory"**:
   * Masukkan alamat `0x90000000` untuk membaca atau mem-flash file binari langsung ke external flash!

### B. Melalui Command Line (CLI)
```bash
# Contoh membaca 64 byte dari flash eksternal
STM32_Programmer_CLI -c port=SWD -el /home/maruf/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader/MX25L_STM32H523CET6.stldr -r32 0x90000000 64

# Contoh memprogram file html / asset langsung ke alamat 0x90211000
STM32_Programmer_CLI -c port=SWD -el /home/maruf/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader/MX25L_STM32H523CET6.stldr -w web_data.bin 0x90211000 -v
```

---

## 4. Cara Menggunakan di STM32CubeIDE Debugger

1. Di STM32CubeIDE, buka menu **Run** -> **Debug Configurations...**
2. Pilih konfigurasi project Anda (misal `tsamota stm32h523cet6`).
3. Masuk ke tab **Debugger**.
4. Gulir ke bawah hingga menemukan bagian **External Loader**.
5. Centang **"Initialize external loader"**.
6. Klik tombol **Scan** atau **Browse**, lalu pilih **`MX25L_STM32H523CET6`**.
7. Klik **Apply** dan **Debug**.

Debugger ST-LINK sekarang dapat langsung mendownload data yang dialokasikan di region `0x90000000` (seperti `.rodata` atau asset HTML di memory mapped) tanpa perlu upload manual melalui web!
