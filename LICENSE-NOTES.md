## License Notes

This repository contains code under multiple licenses. The root LICENSE file specifies Apache-2.0, but the following exceptions apply:

### 1. ArtInChip Proprietary (Restricted)

Files with ArtInChip copyright headers in the following directories are proprietary:
- `chips/` - BSP driver code
- `boards/` - Board support package code
- `apps/` - Application code with ArtInChip copyright

These files are provided under a restricted license and may not be redistributed without explicit permission from ArtInChip Technology Co., Ltd.

### 2. Vendor Proprietary Tools (Binary)

The following binary tools in `tools/scripts/` are proprietary and provided without source code:
- `firmware_security_encrypt`
- `upgcmd`
- `img2simg`
- `mcopy`
- `mformat`
- `mkenvimage`
- `mkimage`
- `mklittlefs`
- `mkuffs`
- `mtools`
- `spienc`

### 3. Third-Party Open Source

The following components are third-party open source software:

#### Cryptography Library
- `tools/scripts/Cryptodome/` - PyCryptodome library, licensed under BSD-2-Clause

#### Scripts with Open Source License
Python scripts in `tools/scripts/` with ArtInChip copyright headers are licensed under Apache-2.0.

### 4. Apache-2.0 License

All other files not listed above are licensed under Apache-2.0 as specified in the root LICENSE file.

---

For any questions regarding licensing, please contact ArtInChip Technology Co., Ltd.