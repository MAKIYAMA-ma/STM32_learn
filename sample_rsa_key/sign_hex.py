from intelhex import IntelHex
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend

HEX_IN = "NUCLEO-U575ZIQ-learn.hex"
SIGNATURE_BIN = "signature.bin"
HEX_OUT = "NUCLEO-U575ZIQ-learn_with_sig.hex"
PRIVATE_KEY_FILE = "private.pem"

START_ADDR = 0x08000000
END_ADDR = 0x081FC000  # 署名前まで
expected_size = END_ADDR - START_ADDR

# 1. IntelHexからFLASH領域を再構成
print("[1] Loading .hex and reconstructing binary with 0xFF fill...")
ih = IntelHex(HEX_IN)
firmware_data = bytearray([0xFF] * expected_size)

# 実データのある部分だけ書き込み
for addr in range(START_ADDR, END_ADDR):
    firmware_data[addr - START_ADDR] = ih[addr]

print(f"[DEBUG] Reconstructed firmware size: {len(firmware_data)}")

# 2. SHA256ハッシュ生成
print("[2] Generating SHA256 hash...")
digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
digest.update(firmware_data)
hash_value = digest.finalize()

print(f"[DEBUG] Hash: {hash_value.hex()}")

# 3. 署名
print("[3] Signing hash with private key...")
with open(PRIVATE_KEY_FILE, "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,
        backend=default_backend()
    )

signature = private_key.sign(
    hash_value,
    padding.PKCS1v15(),
    hashes.SHA256()
)

# 4. 署名保存
with open(SIGNATURE_BIN, "wb") as f:
    f.write(signature)

print(f"[+] Signature saved to '{SIGNATURE_BIN}'")

# 5. 署名をHEXに追加
print("[4] Appending signature to hex...")
import subprocess
subprocess.run([
    "srec_cat", HEX_IN, "-Intel",
    SIGNATURE_BIN, "-Binary", "-offset", f"{hex(END_ADDR)}",
    "-o", HEX_OUT, "-Intel"
], check=True)

print(f"[✔] Output written to: {HEX_OUT}")
