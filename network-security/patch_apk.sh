#!/bin/bash
# Автоматизация интеграции Frida Gadget в целевой APK (No-Root Network Bypass)

TARGET_APK=$1
if [ -z "$TARGET_APK" ]; then
    echo "[-] Использование: ./patch_apk.sh target.apk"
    exit 1
fi

echo "[*] Шаг 1: Декомпиляция целевого мобильного приложения..."
apktool d "$TARGET_APK" -o decompiled_apk

echo "[*] Шаг 2: Создание директории под нативную архитектуру arm64-v8a..."
mkdir -p decompiled_apk/lib/arm64-v8a

echo "[*] Шаг 3: Инжекция Frida Gadget во внутренние библиотеки приложения..."
# Скачиваем официальный нативный гаджет Frida
FRIDA_VER="16.2.1" # Пример актуальной версии
curl -L "https://github.com{FRIDA_VER}/frida-gadget-${FRIDA_VER}-android-arm64.so.xz" -o frida-gadget.so.xz
xz -d frida-gadget.so.xz
mv frida-gadget.so decompiled_apk/lib/arm64-v8a/libfrida-gadget.so

echo "[*] Шаг 4: Настройка конфигурации инжектора (network_bypass.js)..."
# Создаем файл конфигурации, чтобы гаджет автоматически подгружал наш JS-скрипт маскировки сетевого стека
cat <<EOF > decompiled_apk/lib/arm64-v8a/libfrida-gadget.config.so
{
  "interaction": {
    "type": "script",
    "path": "network_bypass.js",
    "on_change": "reload"
  }
}
EOF

# Копируем наш ранее написанный скрипт JA4/SSL-bypass внутрь ассетов приложения
cp network_bypass.js decompiled_apk/assets/network_bypass.js

echo "[*] Шаг 5: Пересборка модифицированного APK пакета..."
apktool b decompiled_apk -o patched_anti_detection.apk

echo "[*] Шаг 6: Генерация отладочного ключа и подпись APK..."
keytool -genkey -v -keystore debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=Android Debug,O=Android,C=US"
apksigner sign --ks debug.keystore --ks-pass pass:android patched_anti_detection.apk

echo "[+] Успех! Файл patched_anti_detection.apk успешно создан и готов к тестам на вашем Xiaomi."
