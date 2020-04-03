#!/usr/bin/env python3

import sys
from struct import *

def decrypt8(int0, int1, key):
    garbage = 0xc6ed3720
    for i in range(32):
        edi = (key[3] + (int0 >> 5)) & 0xffffffff
        ebx = (key[2] + (int0 << 4)) & 0xffffffff
        edi ^= ebx
        edi ^= (garbage + int0) & 0xffffffff
        int1 -= edi
        if int1 < 0:
            int1 += 0x100000000
        int1 &= 0xffffffff

        edi = (key[0] + (int1 << 4)) & 0xffffffff
        ebx = (key[1] + (int1 >> 5)) & 0xffffffff
        edi ^= ebx
        edi ^= (garbage + int1)  & 0xffffffff
        int0 -= edi
        if int0 < 0:
            int0 += 0x100000000
        int0 &= 0xffffffff

        garbage = (garbage + 0x61c89647) & 0xffffffff
    return int0,int1

def decrypt(data, key):
    buf = []
    
    for i in range(len(data)//4):
        buf.append(unpack_from(">I", data, i*4)[0])

    for i in range(len(data)//8):
        int0 = buf[(i*2)+0]
        int1 = buf[(i*2)+1]
        int0,int1 = decrypt8(int0, int1, key)
        buf[(i*2)+0] = int0
        buf[(i*2)+1] = int1

    return b"".join([pack(">I", val) for val in buf])

fw_magic = b"\x5a\xa5" + "SINO".encode()

def main():
    if len(sys.argv) < 2:
        print("Usage: {} <fw installer>".format(sys.argv[0]))
        exit(1)

    with open(sys.argv[1], "rb") as fw_installer:
        i = 0
        while i < len(fw_magic):
            byte = fw_installer.read(1)
            if len(byte) < 1:
                print("Couldn't find a fw header in the file.")
                exit(1)
            elif byte[0] != fw_magic[i]:
                i = 0
                continue
            i += 1
        print("Found a fw header.")

        var_98c0,var_98c4,fw_len = unpack(">HHH", fw_installer.read(6))
    
        fw_data = fw_installer.read(fw_len);
        print("Successfully read encrypted fw data from file.")
    
        key = fw_installer.read(4)
        print("Decryption key: {:08x}".format(unpack("<I", key)[0]))

    print("Image size: {} bytes.".format(fw_len))

    with open("fw_encrypted.bin", "wb") as fw_encrypted:
        fw_encrypted.write(fw_data)
    print("Wrote fw_encrypted.bin.")

    fw_data = decrypt(fw_data, key)
    print("Successfully decrypted fw data.")

    with open("fw.bin", "wb") as fw:
        fw.write(fw_data)
    print("Wrote fw.bin.")

if __name__ == "__main__":
    main()
