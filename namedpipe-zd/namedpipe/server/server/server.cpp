#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>

int main() {
    setlocale(LC_ALL, "ru");
    // Создаем именованный канал
    HANDLE hPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\MyPipe",             // Имя канала
        PIPE_ACCESS_DUPLEX,                // Двусторонний канал
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // Режим работы
        PIPE_UNLIMITED_INSTANCES,          // Максимальное количество экземпляров
        512,                               // Размер выходного буфера
        512,                               // Размер входного буфера
        0,                                 // Таймаут по умолчанию
        NULL                               // Атрибуты безопасности
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Создать именованный канал не удалось: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Ждем соединение..." << std::endl;

    // Ожидаем подключения клиента
    if (!ConnectNamedPipe(hPipe, NULL)) {
        std::cerr << "Соединение не установлено: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Клиент соединен" << std::endl;

    char buffer[512];
    DWORD bytesRead;

    // Читаем данные из канала
    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Добавляем завершающий нуль
        std::string message(buffer);

        // Переворачиваем строку
        std::reverse(message.begin(), message.end());
        std::cout << "Сообщение от клиента получено" << std::endl;

        // Отправляем перевернутую строку обратно клиенту
        DWORD bytesWritten;
        if (!WriteFile(hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, NULL)) {
            std::cerr << "Ошибка записи " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Ошибка чтения: " << GetLastError() << std::endl;
    }

    // Закрываем канал
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    std::cout << "Сервер закончен" << std::endl;
    return 0;
}