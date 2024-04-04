# Noob-Club topic parser

Простой парсер сайта, написанный на языке C++ с использованием библиотеки CPR.
Для хранения данных выбран формат JSON, используется библиотека nlohmann.json.

Для примера выбран сайт noob-club.ru.
По запросу "Parse" парсер отправляет get-запрос на сайт и собирает полученные данные по темам (id, заголовок, автор и дата) и сохраняет их в json-файл.
По запросу "View" программа выводит в консоль собранные данные из файла parsedData.txt.

CPR: https://github.com/libcpr/cpr

Niels Lohmann's JSON for Modern C++: https://github.com/nlohmann/json
