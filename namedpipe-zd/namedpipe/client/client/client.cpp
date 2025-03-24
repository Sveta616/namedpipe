#include <windows.h>
#include <iostream>
#include <string>

int main() {
    setlocale(LC_ALL, "ru");
    // Подключаемся к именованному каналу
    HANDLE hPipe = CreateFile(
        L"\\\\.\\pipe\\MyPipe",             // Имя канала
        GENERIC_READ | GENERIC_WRITE,      // Режим доступа
        0,                                 // Нет совместного доступа
        NULL,                              // Атрибуты безопасности
        OPEN_EXISTING,                     // Открываем существующий канал
        0,                                 // Атрибуты файла
        NULL                               // Шаблон файла
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка открытия: " << GetLastError() << std::endl;
        return 1;
    }

    // Вводим строку от пользователя
    std::string message;
    std::cout << "Напишите сообщение: ";
    std::getline(std::cin, message);

    // Проверяем, что строка не превышает размер буфера
    if (message.size() >= 512) {
        std::cerr << "Сообщение слишком длинное!" << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Отправляем строку в канал
    DWORD bytesWritten;
    if (!WriteFile(hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, NULL)) {
        std::cerr << "Ошибка записи: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Читаем ответ от сервера
    char buffer[512];
    DWORD bytesRead;
    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Добавляем завершающий нуль
        std::cout << "Перевернутое сообщение: " << buffer << std::endl;
    }
    else {
        std::cerr << "Ошибка чтения: " << GetLastError() << std::endl;
    }

    // Закрываем канал
    CloseHandle(hPipe);

    return 0;
}