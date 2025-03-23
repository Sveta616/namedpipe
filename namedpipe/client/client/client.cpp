#include <windows.h>
#include <iostream>
#include <string>

int main() {
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
        std::cerr << "CreateFile failed: " << GetLastError() << std::endl;
        return 1;
    }

    // Вводим строку от пользователя
    std::string message;
    std::cout << "Enter a message: ";
    std::getline(std::cin, message);

    // Проверяем, что строка не превышает размер буфера
    if (message.size() >= 512) {
        std::cerr << "Message is too long!" << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Отправляем строку в канал
    DWORD bytesWritten;
    if (!WriteFile(hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, NULL)) {
        std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Читаем ответ от сервера
    char buffer[512];
    DWORD bytesRead;
    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Добавляем завершающий нуль
        std::cout << "Reversed message: " << buffer << std::endl;
    }
    else {
        std::cerr << "ReadFile failed: " << GetLastError() << std::endl;
    }

    // Закрываем канал
    CloseHandle(hPipe);

    return 0;
}