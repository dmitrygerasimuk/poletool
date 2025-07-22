# POLE.OVL Dictionary Tool
[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/dmitrygerasimuk/poletool?tab=readme-ov-file#poleovl-dictionary-tool)
[![ru](https://img.shields.io/badge/lang-ru-green.svg)](https://github.com/dmitrygerasimuk/poletool?tab=readme-ov-file#poleovl-dictionary-tool-ru)


> 📄 [Читать на русском ↓](https://github.com/dmitrygerasimuk/poletool?tab=readme-ov-file#poleovl-dictionary-tool-ru)

A simple tool to **pack** and **unpack** dictionary files used in the classic MS-DOS game **"Pole Chudes"** (Russian Wheel of Fortune).  
Rewritten in C based on the original C# code by Genjitsu Gadget Lab.

---

## 🧩 Purpose

The `POLE.OVL` file contains key-value pairs ("question–answer") encoded in a modified CP866 format.  
This tool allows you to:

- 📤 **unpack** a `.OVL` file into a human-readable `.txt` (UTF-8)
- 📥 **pack** a `.txt` file back into a binary `.OVL`

---

## ⚙️ Build Instructions

**Dependencies:**
- `iconv` (GNU libiconv or glibc built-in)
- C compiler (`gcc`, `clang`, `tcc`, `OpenWatcom`, etc.)

**Example build:**

```bash
gcc -o poletool poletool.c -liconv
```

---

## 🕹️ Usage

```bash
./poletool unpack dict.ovl dict.txt
./poletool pack dict.txt dict.ovl
```

### Unpacking (`unpack`)

Converts `dict.ovl` into `dict.txt`:

```ini
[Тема 1]
Ответ 1
Ответ 2
[Тема 2]
Ответ 3
```

### Packing (`pack`)

Builds a binary `.OVL` file from a `.txt` file.

**Requirements:**
- At least **3** key-value pairs
- Headers must be in `[KEY]` format, followed by answer lines

---

## 🧠 Format Notes

- Fixed line length: **20 bytes**
- Encoding: **CP866**, with custom byte adjustments:
  - On unpack: bytes `>= 0xB0` are corrected with `+0x30`
  - On pack: bytes `>= 0xE0` are corrected with `-0x30`
- The character `@` in strings is converted to a space during unpack

---

## 📁 `.OVL` Binary Format

1. First byte: length of a string containing the number of records (in CP866)
2. 20 bytes: record count as CP866-encoded string
3. Followed by key-value pairs:
   - byte: word length
   - 20 bytes: word
   - byte: key length
   - 20 bytes: key

---

## 💡 Example Input File

```ini
[научный термин]
синтаксис
пунткуация
флаттер
изотоп
[тип человека]
дылда
карлик
пончик
```

---

## 🧪 Validation

After unpacking, the program prints statistics:

```
TOTAL: 42
Database header count matches
```

If the actual count doesn't match the header, a warning is shown:

```
MISMATCH: header count = 50, actual = 42
```

---

## 🧷 Author

- Ported to C by **[Dmitry Gerasimyuk]**
- Original code: **Genjitsu Gadget Lab (C#)**

---

## 📜 License

MIT License 

# POLE.OVL Dictionary Tool RU
Утилита для упаковки и распаковки словарей из игры **«Поле Чудес»**.  
Основана на оригинальном C#-коде от Genjitsu Gadget Lab, переписана на C

---

## 🧩 Назначение

Файлы `POLE.OVL` в игре содержат пары «вопрос-ответ» в специфическом CP866-кодировочном формате с дополнительной коррекцией байтов.  
Эта утилита позволяет:

- 📤 **распаковать** `.OVL`-файл в читаемый `.txt` (UTF-8)
- 📥 **упаковать** `.txt`-файл обратно в `.OVL`

---

## ⚙️ Сборка

**Зависимости:**
- `iconv` (GNU libiconv или встроенный в glibc)
- Компилятор C (`gcc`, `clang`, `tcc`, `OpenWatcom` и др.)

**Пример сборки:**

```bash
gcc -o poletool poletool.c -liconv
```

---

## 🕹️ Использование

```bash
./poletool unpack dict.ovl dict.txt
./poletool pack dict.txt dict.ovl
```

### Распаковка (`unpack`)
Преобразует `dict.ovl` в `dict.txt`:

```ini
[Тема 1]
Ответ 1
Ответ 2
[Тема 2]
Ответ 3
```

### Упаковка (`pack`)
Формирует бинарный `.OVL` из текстового `.txt`.

**Требования:**
- Файл должен содержать минимум **3** пар "вопрос-ответ"
- Заголовки оформляются как `[Ключ]`, далее — строки значений

---

## 🧠 Особенности формата

- Все строки фиксированной длины: **20 байт**
- Используется кодировка **CP866** с модификацией:
  - при распаковке байты `>= 0xB0` корректируются на `+0x30`
  - при упаковке байты `>= 0xE0` корректируются на `-0x30`
- Символ `@` в словах заменяется на пробел при чтении

---

## 📁 Формат `.OVL`

1. Первый байт — длина строки с количеством записей (в CP866)
2. 20 байт — закодированная строка с числом записей
3. Далее — пары:
   - байт длины слова
   - 20 байт слова
   - байт длины ключа
   - 20 байт ключа

---

## 💡 Пример текстового файла

```ini
[научный термин]
синтаксис
пунткуация
флаттер
изотоп
[тип человека]
дылда
карлик
пончик
```

---

## 🧪 Проверка

После распаковки программа выведет статистику:

```
TOTAL: 42
Database header count matches
```

Если число записей не совпадает с заголовком — появится предупреждение:

```
MISMATCH: header count = 50, actual = 42
```

---

## 🧷 Автор

- Форк и переработка: **[Дмитрий Герасимук]**
- Оригинальный код: **Genjitsu Gadget Lab (C#)**

---

## 📜 Лицензия

MIT License (или иная по вашему выбору)