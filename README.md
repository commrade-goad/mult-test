# mult test
a multiplayer test using tcp and udp

## Build

1. Bootstrap nob with your `c` compiler.

```sh
cc nob.c -o nob
```

2. Build the project

```sh
./nob
```

## Usage

- Launch with `-server` to start the server.
- Launch with `-ip <IP>` to use custom ip.
- Launch with `-port <PORT>` to use custom port.
- Launch without the `-server` to start the client


## Konteks Permasalahan

Pengembangan game multiplayer membutuhkan sistem komunikasi jaringan yang handal, efisien, dan real-time antara pemain. Permasalahan yang sering dihadapi adalah bagaimana menjaga sinkronisasi data antar pemain, mengatasi latensi, packet loss, dan memilih protokol jaringan yang sesuai. TCP menawarkan keandalan namun dengan latensi lebih tinggi, sedangkan UDP lebih cepat namun rawan kehilangan data. Pengembang perlu memahami karakteristik kedua protokol tersebut sebelum mengimplementasikan komunikasi multiplayer yang optimal.

## Solusi dari mult-test

`mult-test` adalah aplikasi uji coba multiplayer yang dirancang untuk membantu pengembang game melakukan eksperimen komunikasi antar pemain menggunakan TCP dan UDP. Aplikasi ini memungkinkan simulasi skenario multiplayer secara langsung dan visual, sehingga pengembang dapat mempelajari dampak penggunaan masing-masing protokol pada responsivitas dan sinkronisasi data.

### Cara Kerja
- **Server:** Menyediakan dua socket (TCP dan UDP) untuk menerima koneksi dan data dari client. Menggunakan epoll untuk event-driven I/O.
- **Client:** Terkoneksi ke server dengan TCP untuk memperoleh ID, lalu menggunakan UDP untuk mengirim posisi pemain secara berkala serta menerima update posisi pemain lain.
- **Game Loop:** Setiap pemain dapat bergerak dengan input keyboard (WASD), posisi dikirim ke server dan didistribusikan ke semua client lain secara real-time.
- **Visualisasi:** Pergerakan dan posisi pemain divisualisasikan di window, sehingga perubahan dapat diamati langsung.

### Fitur Utama
- Simulasi komunikasi multiplayer menggunakan TCP dan UDP.
- Pengiriman dan penerimaan posisi pemain secara real-time.
- Sinkronisasi objek pemain antar client.

### Manfaat
- Memahami perbedaan dan trade-off antara penggunaan TCP dan UDP dalam game multiplayer.
- Menjadi dasar untuk pengembangan game multiplayer berbasis custom network protocol.
- Mempermudah debugging dan tuning komunikasi jaringan multiplayer sebelum masuk tahap produksi.
