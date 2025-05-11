import sys
import argparse
from intelhex import IntelHex
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
import subprocess
import os

def main():
    parser = argparse.ArgumentParser(description="Sign firmware and append signature to .hex")
    parser.add_argument("hex_in", help="Input Intel HEX file")
    parser.add_argument("private_key", help="PEM format private key file")
    parser.add_argument("hex_out", help="Output Intel HEX file with signature")
    parser.add_argument("start_addr", type=lambda x: int(x, 0), help="Start address (e.g. 0x08000000)")
    parser.add_argument("end_addr", type=lambda x: int(x, 0), help="End address (e.g. 0x081FC000)")
    parser.add_argument("--sig_bin", default="signature.bin", help="Temporary signature binary filename")

    args = parser.parse_args()

    expected_size = args.end_addr - args.start_addr
    if expected_size <= 0:
        print("[ERROR] End address must be greater than start address.")
        sys.exit(1)

    print("[1] Loading .hex and reconstructing binary with 0xFF fill...")
    ih = IntelHex(args.hex_in)
    firmware_data = bytearray([0xFF] * expected_size)

    for addr in range(args.start_addr, args.end_addr):
        firmware_data[addr - args.start_addr] = ih[addr]

    print(f"[DEBUG] Reconstructed firmware size: {len(firmware_data)}")

    print("[2] Generating SHA256 hash...")
    digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
    digest.update(firmware_data)
    hash_value = digest.finalize()

    print(f"[DEBUG] Hash: {hash_value.hex()}")
    with open("hash.bin", "wb") as f:
        f.write(hash_value)

    print("[3] Signing hash with private key...")
    with open(args.private_key, "rb") as key_file:
        private_key = serialization.load_pem_private_key(
            key_file.read(),
            password=None,
            backend=default_backend()
        )

    signature = private_key.sign(
        hash_value,
        padding.PKCS1v15(),
        Prehashed(hashes.SHA256())
    )

    with open(args.sig_bin, "wb") as f:
        f.write(signature)

    print(f"[+] Signature saved to '{args.sig_bin}'")

    print("[4] Appending signature to hex...")
    subprocess.run([
        "srec_cat", args.hex_in, "-Intel",
        args.sig_bin, "-Binary", "-offset", f"{hex(args.end_addr)}",
        "-o", args.hex_out, "-Intel"
    ], check=True)

    print(f"[OK] Output written to: {args.hex_out}")

    try:
        os.remove(args.sig_bin)
        print(f"[INFO] Deleted temporary file: {args.sig_bin}")
    except OSError as e:
        print(f"[WARNING] Could not delete temporary file: {args.sig_bin} ({e})")

if __name__ == "__main__":
    main()
