#!/usr/bin/env python3
"""
Конвертация датасета USPS из H5 в BMP для обучения и теста.
По умолчанию: data/usps.h5 → data/train/ и data/test/
Имена файлов: "{метка} {индекс:04d}.bmp".
Обучение: ./neural_network data/train
Тест:     ./neural_network data/test
"""

import os
import sys
import struct

try:
    import numpy as np
except ImportError:
    print("Установите numpy: pip install numpy")
    sys.exit(1)
try:
    import h5py
except ImportError:
    print("Установите h5py: pip install h5py")
    sys.exit(1)


def write_bmp_16x16_gray(filepath, pixels_0_1):
    """Пишет 16x16 grayscale (0..1) как 24-bit BMP (BGR). Совместимо с BMP_reading.h."""
    w, h = 16, 16
    row_bytes = (w * 3 + 3) & ~3
    pix_size = row_bytes * h
    headers_size = 14 + 40
    file_size = headers_size + pix_size

    with open(filepath, "wb") as f:
        # BITMAPFILEHEADER
        f.write(b"BM")
        f.write(struct.pack("<I", file_size))
        f.write(struct.pack("<HH", 0, 0))
        f.write(struct.pack("<I", headers_size))
        # BITMAPINFOHEADER
        f.write(struct.pack("<I", 40))
        f.write(struct.pack("<ii", w, h))
        f.write(struct.pack("<HH", 1, 24))
        f.write(struct.pack("<I", 0))
        f.write(struct.pack("<I", pix_size))
        f.write(struct.pack("<iiII", 0, 0, 0, 0))
        # Пиксели: снизу вверх, BGR, 0-255 (R=G=B для grayscale)
        arr = (np.clip(pixels_0_1, 0, 1) * 255).astype(np.uint8)
        padding = row_bytes - w * 3
        for y in range(h - 1, -1, -1):
            for x in range(w):
                v = arr[y, x]
                f.write(struct.pack("BBB", v, v, v))
            if padding:
                f.write(b"\x00" * padding)
    return filepath


def _export_set(data, target, out_dir, prefix=""):
    n = data.shape[0]
    assert data.shape[1] == 256 and target.shape[0] == n
    os.makedirs(out_dir, exist_ok=True)
    for i in range(n):
        img = data[i].reshape(16, 16)
        label = int(target[i])
        name = f"{label} {i:04d}.bmp"
        write_bmp_16x16_gray(os.path.join(out_dir, name), img)
        if (i + 1) % 1000 == 0:
            print(f"  {prefix}{i + 1}/{n}")
    return n


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    default_h5 = os.path.join(project_root, "data", "usps.h5")
    default_data = os.path.join(project_root, "data")

    path_h5 = sys.argv[1] if len(sys.argv) > 1 else default_h5
    out_base = sys.argv[2] if len(sys.argv) > 2 else default_data
    train_dir = os.path.join(out_base, "train")
    test_dir = os.path.join(out_base, "test")

    if not os.path.isfile(path_h5):
        print(f"Файл не найден: {path_h5}")
        sys.exit(2)

    with h5py.File(path_h5, "r") as f:
        train_data = f["train/data"][:]
        train_target = f["train/target"][:]
        test_data = f["test/data"][:]
        test_target = f["test/target"][:]

    print("Train...")
    n_train = _export_set(train_data, train_target, train_dir, "train ")
    print("Test...")
    n_test = _export_set(test_data, test_target, test_dir, "test ")
    print(f"Готово: train {n_train} BMP → {train_dir}, test {n_test} BMP → {test_dir}")


if __name__ == "__main__":
    main()
