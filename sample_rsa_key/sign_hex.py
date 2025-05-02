import os
import subprocess
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend

HEX_IN = "NUCLEO-U575ZIQ-learn.hex"
BIN_TEMP = "temp_fw.bin"
SIGNATURE_BIN = "signature.bin"
HEX_OUT = "NUCLEO-U575ZIQ-learn_with_sig.hex"
PRIVATE_KEY_FILE = "private.pem"

# 対象範囲（署名対象: FLASH先頭〜署名手前まで）
START_ADDR = 0x08000000
END_ADDR = 0x081FC000  # 署名前まで

# 1. HEX -> BIN (署名対象のみ抽出)
print("[1] Converting .hex to binary (firmware area only)...")
subprocess.run([
    "srec_cat", HEX_IN, "-Intel",
    "-crop", f"{START_ADDR}", f"{END_ADDR}",
    "-o", BIN_TEMP, "-Binary"
], check=True)

# 2. SHA256署名作成
print("[2] Generating signature with private key...")
with open(BIN_TEMP, "rb") as f:
    firmware_data = f.read()

digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
digest.update(firmware_data)
hash_value = digest.finalize()

# 秘密鍵読み込み
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

# 署名バイナリ保存
with open(SIGNATURE_BIN, "wb") as f:
    f.write(signature)

print(f"[+] Signature ({len(signature)} bytes) saved to '{SIGNATURE_BIN}'")

# 3. HEXに署名を追記
print("[3] Appending signature to hex file...")
subprocess.run([
    "srec_cat", HEX_IN, "-Intel",
    SIGNATURE_BIN, "-Binary", "-offset", f"{hex(END_ADDR)}",
    "-o", HEX_OUT, "-Intel"
], check=True)

print(f"[✔] Output written to: {HEX_OUT}")

# 4. Cleanup
os.remove(BIN_TEMP)
os.remove(SIGNATURE_BIN)
