# 📖 Инструкция по сборке Aura Flow (C++)

Эта инструкция поможет вам собрать и запустить C++ версию Aura Flow на Windows с оптимизацией под AMD (DirectML).

## 🛠 Предварительные требования
1. **Visual Studio 2022**: Установите рабочую нагрузку «Разработка классических приложений на C++».
2. **Qt 6.5 LTS**: Установите через "Qt Maintenance Tool" (версия MSVC 2019 64-bit).
3. **CMake**: Убедитесь, что CMake установлен и добавлен в PATH.

## 📂 Подготовка проекта
1. Перейдите в папку `aura-flow-cpp`.
2. Скачайте/подключите `whisper.cpp` в папку `3rdparty/`:
   - Вы можете клонировать его: `git clone https://github.com/ggerganov/whisper.cpp 3rdparty/whisper.cpp`.
3. Подготовьте модель:
   - Скрипт для конвертации: `3rdparty/whisper.cpp/models/convert-pt-to-ggml.py`.
   - Конвертируйте вашу модель Whisper v3 из PyTorch в формат GGML.
   - Выполните квантование (рекомендуется `q8_0`):
     ```bash
     quantize.exe build/models/ggml-large-v3.bin models/ggml-large-v3-q8_0.bin q8_0
     ```

## 🏗 Сборка в Visual Studio
1. Откройте папку `aura-flow-cpp` в Visual Studio (через "Open a local folder").
2. VS автоматически определит `CMakeLists.txt`.
3. Убедитесь, что выбран профиль `x64-Release`.
4. В настройках CMake (если нужно) укажите путь к Qt: `set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64")`.
5. Нажмите **Build -> Build All**.

## 🚀 Запуск
1. После сборки исполняемый файл `AuraFlow.exe` будет в папке `out/build/...`.
2. Убедитесь, что файл модели `.bin` находится по пути, указанному в `main.cpp` (обычно `models/ggml-large-v3-q8_0.bin`).
3. Запустите приложение. Оно появится в системном трее.

## ⌨️ Использование
- **Ctrl + Space (удерживать)**: Начать запись.
- **Отпустить Ctrl + Space**: Остановить запись и запустить перевод/транскрипцию.
- Результат будет автоматически вставлен в активное окно через системный буфер обмена.

---
> [!TIP]
> Для проверки использования видеокарты AMD откройте **AMD Software: Adrenalin Edition** и следите за нагрузкой на GPU во время обработки (статус "Processing...").
