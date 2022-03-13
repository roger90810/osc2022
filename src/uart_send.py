import sys
import os

KERNEL_PATH = sys.argv[1]
SERIAL_PATH = sys.argv[2]

with open(KERNEL_PATH, 'rb', buffering=0) as kernel_file:
    kernel_size = os.path.getsize(KERNEL_PATH)
    print(f'size is      : {kernel_size}')
    print(f'size in byte : {kernel_size.to_bytes(4, "big")}')
    with open(SERIAL_PATH, 'wb', buffering=0) as tty:
        tty.write(kernel_size.to_bytes(4, "big"))
        tty.write(kernel_file.read())
        print("Send kernel done.")