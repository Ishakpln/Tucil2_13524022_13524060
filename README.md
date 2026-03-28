# Tucil 2 IF2211 - Voxelization Objek 3D Menggunakan Octree

## Deskripsi

- Program mengubah objek 3D berformat .obj menjadi mesh voxel .obj.
- Voxelization dilakukan dengan struktur Octree.

## Fitur

- Membaca file .obj
- Membentuk Octree dari bounding box objek
- Menghasilkan mesh voxel
- Menyimpan hasil ke file .obj

## Struktur Folder

- src untuk source code
- bin untuk executable
- test untuk file uji dan output
- doc untuk laporan

## Kebutuhan

- C++20
- Compiler yang mendukung std::jthread

## Kompilasi

```
make run
```

## Menjalankan Program

- Masukkan path file .obj
- Masukkan depth Octree
- Pilih simpan atau tidak
- Ikuti instruksi yang ada di cli

## Output

- File hasil disimpan di folder test
- Nama file output: nama_input_Voxelized.obj

## Anggota

- Ishak Palentino Napitupulu - 13524022
- Steven Tan - 13524060
