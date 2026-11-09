#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <string>

using namespace std;

struct Racer {
    string name;
    PROCESS_INFORMATION pi;
    DWORD priorityClass;
    DWORD duration;
    bool isFinished;
    int place;
};

string getPriorityName(DWORD priority) {
    if (priority == IDLE_PRIORITY_CLASS) return "IDLE";
    if (priority == BELOW_NORMAL_PRIORITY_CLASS) return "BELOW_NORMAL";
    if (priority == NORMAL_PRIORITY_CLASS) return "NORMAL";
    if (priority == ABOVE_NORMAL_PRIORITY_CLASS) return "ABOVE_NORMAL";
    if (priority == HIGH_PRIORITY_CLASS) return "HIGH";
    return "UNKNOWN";
}

string changPriority(Racer& racer, bool increase) {
    string oldPri = getPriorityName(racer.priorityClass);

    if (increase) {
        if (racer.priorityClass == IDLE_PRIORITY_CLASS) racer.priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == BELOW_NORMAL_PRIORITY_CLASS) racer.priorityClass = NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == NORMAL_PRIORITY_CLASS) racer.priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == ABOVE_NORMAL_PRIORITY_CLASS) racer.priorityClass = HIGH_PRIORITY_CLASS;
    }
    else {
        if (racer.priorityClass == HIGH_PRIORITY_CLASS) racer.priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == ABOVE_NORMAL_PRIORITY_CLASS) racer.priorityClass = NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == NORMAL_PRIORITY_CLASS) racer.priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
        else if (racer.priorityClass == BELOW_NORMAL_PRIORITY_CLASS) racer.priorityClass = IDLE_PRIORITY_CLASS;
    }

    if (SetPriorityClass(racer.pi.hProcess, racer.priorityClass)) {
        string newPri = getPriorityName(racer.priorityClass);
        return ">>> (СМЕНА ПРИОРИТЕТА) " + racer.name + ": " + oldPri + " -> " + newPri + " <<<";
    }
    return ">>> (ОШИБКА) Не удалось сменить приоритет для " + racer.name;
}

int main() {
    setlocale(LC_ALL, "Russian");

    int startRange, endRange;
    cout << "Введите начало диапазона: ";
    cin >> startRange;
    cout << "Введите конец диапазона: ";
    cin >> endRange;

    wchar_t appName[] = L"C:(путь)pract13_work.exe";
    int numRacers = 0;
    while (numRacers < 2 || numRacers > 5) {
        cout << "Введите количество участников (2-5): ";
        cin >> numRacers;
        cin.ignore();
    }

    Racer racers[5];

    cout << "\n--- НАСТРОЙКА ПРИОРИТЕТОВ ---\n";
    cout << "1. IDLE\n2. BELOW_NORMAL\n3. NORMAL\n4. ABOVE_NORMAL\n5. HIGH\n";

    for (int i = 0; i < numRacers; i++) {
        racers[i].name = "Racer_" + to_string(i + 1);
        racers[i].isFinished = false;
        racers[i].place = 0;
        racers[i].duration = 0;

        int choice;
        cout << "Выберите приоритет для " << racers[i].name << " (1-5): ";
        cin >> choice;

        switch (choice) {
        case 1: racers[i].priorityClass = IDLE_PRIORITY_CLASS; break;
        case 2: racers[i].priorityClass = BELOW_NORMAL_PRIORITY_CLASS; break;
        case 3: racers[i].priorityClass = NORMAL_PRIORITY_CLASS; break;
        case 4: racers[i].priorityClass = ABOVE_NORMAL_PRIORITY_CLASS; break;
        case 5: racers[i].priorityClass = HIGH_PRIORITY_CLASS; break;
        default: racers[i].priorityClass = NORMAL_PRIORITY_CLASS; break;
        }
    }

    cout << "--- ГОНКА НАЧАЛАСЬ ---\n";
    cout << "Все процессы запущены. Ожидание финиша...\n";
    cout << "Управление: нажмите номер робота (1-" << numRacers << "), затем '+' или '-' для смены приоритета.\n";

    for (int i = 0; i < numRacers; i++) {
        wstring cmd = L"pract13_work.exe " + to_wstring(startRange) + L" " + to_wstring(endRange);
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        if (!CreateProcess(appName, &cmd[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &racers[i].pi)) {
            cout << "ОШИБКА Не удалось запустить " << racers[i].name << ". Код: " << GetLastError() << "\n";
            return 1;
        }

        SetProcessAffinityMask(racers[i].pi.hProcess, 1);
        SetPriorityClass(racers[i].pi.hProcess, racers[i].priorityClass);

        cout << "(СТАРТ) " << racers[i].name << " (PID: " << racers[i].pi.dwProcessId
            << ") | Приоритет: " << getPriorityName(racers[i].priorityClass) << "\n";
    }
    int finishCount = 0;
    int selRacer = 0;

    while (finishCount < numRacers) {

        for (int i = 0; i < numRacers; i++) {
            if (!racers[i].isFinished) {
                if (WaitForSingleObject(racers[i].pi.hProcess, 0) == WAIT_OBJECT_0) {
                    racers[i].isFinished = true;

                    DWORD exitCode = 0;
                    GetExitCodeProcess(racers[i].pi.hProcess, &exitCode);
                    racers[i].duration = exitCode;

                    finishCount++;

                    cout << "(ФИНИШ) " << racers[i].name << " завершил вычисления. Время: " << racers[i].duration << " мс.\n";
                }
            }
        }

        if (_kbhit()) {
            char c = _getch();
            if (c >= '1' && c <= '0' + numRacers) {
                selRacer = c - '1';
                cout << "(ВЫБОР) Выбран " << racers[selRacer].name << "\n";
            }
            else if (c == '+') {
                cout << changPriority(racers[selRacer], true) << "\n";
            }
            else if (c == '-') {
                cout << changPriority(racers[selRacer], false) << "\n";
            }
        }

        Sleep(50);
    }

    for (int i = 0; i < numRacers - 1; i++) {
        for (int j = 0; j < numRacers - i - 1; j++) {
            if (racers[j].duration > racers[j + 1].duration) {
                Racer temp = racers[j];
                racers[j] = racers[j + 1];
                racers[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < numRacers; i++) {
        racers[i].place = i + 1;
    }

    cout << "                    ИТОГИ ГОНКИ\n";
    cout << "Место | PID    | Имя        | Приоритет     | Время (мс)\n";

    for (int i = 0; i < numRacers; i++) {
        cout << "  " << racers[i].place << "   | "
            << racers[i].pi.dwProcessId << " | "
            << racers[i].name << " | "
            << getPriorityName(racers[i].priorityClass) << " | "
            << racers[i].duration << "\n";

        CloseHandle(racers[i].pi.hThread);
        CloseHandle(racers[i].pi.hProcess);
    }
    system("pause");
}