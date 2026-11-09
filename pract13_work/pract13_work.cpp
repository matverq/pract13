#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <Windows.h>

using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    if (argc != 3) {
        cout << "Ошибка: передайте начало и конец диапазона.\n";
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    DWORD startTime = GetTickCount();
    int count = 0;

    // Поиск простых чисел
    for (int i = start; i <= end; i++) {
        if (i <= 1) continue;

        bool isPrime = true;
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) count++;
    }
    DWORD endTime = GetTickCount();
    DWORD duration = endTime - startTime;

    cout << "ПРОЦЕСС (PID: " << GetCurrentProcessId() << ") Завершил работу\n";
    cout << "Диапазон поиска: от " << start << " до " << end << "\n";
    cout << "Найдено простых чисел: " << count << "\n";
    cout << "Время выполнения: " << duration << " мс\n";
    cout << "Нажмите любую клавишу для выхода.\n";

    _getch();
    return duration;
}