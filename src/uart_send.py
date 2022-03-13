import sys
import os

KERNEL_PATH = sys.argv[1]
SERIAL_PATH = sys.argv[2]

with open(KERNEL_PATH, 'rb', buffering=0) as kernel_file:
    kernel_size = os.path.getsize(KERNEL_PATH)
    print(f'size is      : {kernel_size}')
    b = kernel_size.to_bytes(4, "big")
    print(f'size in byte : {hex(b[0])} {hex(b[1])} {hex(b[2])} {hex(b[3])}')
    with open(SERIAL_PATH, 'wb', buffering=0) as tty:
        tty.write(b)
        tty.write(kernel_file.read())
        print("Send kernel done.")