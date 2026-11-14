#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>

using namespace std;

struct Racer {
    string name;
    PROCESS_INFORMATION pi;
    DWORD priorityClass;
    DWORD time;
    bool finish;
    int place;
};

string getPriorityName(DWORD priority) {
    if (priority == IDLE_PRIORITY_CLASS) return "IDLE";
    else if (priority == BELOW_NORMAL_PRIORITY_CLASS) return "BELOW_NORMAL";
    else if (priority == NORMAL_PRIORITY_CLASS) return "NORMAL";
    else if (priority == ABOVE_NORMAL_PRIORITY_CLASS) return "ABOVE_NORMAL";
    else if (priority == HIGH_PRIORITY_CLASS) return "HIGH";
    return "UNKOWN";
}

string changePriority(Racer& r, bool pr) {
    string oldPriority = getPriorityName(r.priorityClass);

    if (pr) {
        if (r.priorityClass == IDLE_PRIORITY_CLASS) r.priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == BELOW_NORMAL_PRIORITY_CLASS) r.priorityClass = NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == NORMAL_PRIORITY_CLASS) r.priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == ABOVE_NORMAL_PRIORITY_CLASS) r.priorityClass = HIGH_PRIORITY_CLASS;
    }
    else {
        if (r.priorityClass == HIGH_PRIORITY_CLASS) r.priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == ABOVE_NORMAL_PRIORITY_CLASS) r.priorityClass = NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == NORMAL_PRIORITY_CLASS) r.priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
        else if (r.priorityClass == BELOW_NORMAL_PRIORITY_CLASS) r.priorityClass = IDLE_PRIORITY_CLASS;
    }
    if (SetPriorityClass(r.pi.hProcess, r.priorityClass)) {
        string newPriority = getPriorityName(r.priorityClass);
        return "Смена проиоритета " + r.name + ": " + oldPriority + " -> " + newPriority;
    }
    return "Не удалось сменить приоритет для " + r.name;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    
    int startRange;
    int endRange;

    cout << "Введите начало диапазона: ";
    cin >> startRange;
    cout << "Введите конец диапазона: ";
    cin >> endRange;

    wchar_t appName[] = L"C:\\Users\\st310-02\\Desktop\\Politov_PR-31\\pract13_work\\x64\\Debug\\pract13_work.exe";
    int numRacers = 0;
    while (numRacers<2 || numRacers>5)
    {
        cout << "Введите количество участников (2-5)";
        cin >> numRacers;
    }
    Racer r[5];

    cout << "Установка приоритета каждому процессу\n";
    cout << "1. IDLE_PRIORITY_CLASS\n2. BELOW_NORMAL_PRIORITY_CLASS\n3. NORMAL_PRIORITY_CLASS\n4. ABOVE_NORMAL_PRIORITY_CLASS\n5. HIGH_PRIORITY_CLASS\n";

    for (int i = 0; i < numRacers; i++)
    {
        r[i].name = "Racer_" + to_string(i + 1);
        r[i].finish = false;
        r[i].place = 0;
        r[i].time = 0;
        
        int choice;
        cout << "Выберите приоритет для " << r[i].name << " (1-5)";
        cin >> choice;

        switch (choice)
        {
        case 1: r[i].priorityClass = IDLE_PRIORITY_CLASS; break;
        case 2: r[i].priorityClass = BELOW_NORMAL_PRIORITY_CLASS; break;
        case 3: r[i].priorityClass = NORMAL_PRIORITY_CLASS;  break;
        case 4: r[i].priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;  break;
        case 5: r[i].priorityClass = HIGH_PRIORITY_CLASS; break;
        default:
            r[i].priorityClass = NORMAL_PRIORITY_CLASS;  break;
            break;
        }
    }

    cout << "Начало гонки\n";
    cout << "Для смены приоритета: нажмите номер робота, после чего + или - для смены приоритета\n";

    for (int i = 0; i < numRacers; i++)
    {
        wstring cmd = L"pract13_work.exe " + to_wstring(startRange) + L" " + to_wstring(endRange);
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        
        if (!CreateProcess(appName, &cmd[0], NULL, NULL, FALSE,  CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &si, &r[i].pi)) {
            cout << r[i].name << " не запущен по ошибке: " << GetLastError();
            return 1;
        }
        SetProcessAffinityMask(r[i].pi.hProcess, 1);
        SetPriorityClass(r[i].pi.hProcess, r[i].priorityClass);

        cout << "Старт " << r[i].name << " PID:" << r[i].pi.dwProcessId << ", приоритет: " << getPriorityName(r[i].priorityClass) << "\n";
    }

    for (int i = 0; i < numRacers; i++)
    {
        ResumeThread(r[i].pi.hThread);
    }

    int finishCount = 0;
    int selRacer = 0;

    while (finishCount < numRacers) {
        for (int i = 0; i < numRacers; i++)
        {
            if (!r[i].finish) {
                if (WaitForSingleObject(r[i].pi.hProcess, 0) == WAIT_OBJECT_0) {
                    r[i].finish = true;
                    DWORD exite = 0;
                    GetExitCodeProcess(r[i].pi.hProcess, &exite);
                    r[i].time = exite;
                    finishCount++;

                    cout << "Финиш " << r[i].name << " завершил вычисления, время " << r[i].time << "м/с\n";
                }
            }
        }
        if (_kbhit()) {
            char c = _getch();
            if (c >= '1' && c <= '0' + numRacers) {
                selRacer = c - '1';
                cout << "Выбран " << r[selRacer].name << "\n";
            }
            else if (c == '+') {
                cout << changePriority(r[selRacer], true);
            }
            else if (c == '-') {
                cout << changePriority(r[selRacer], false);
            }
        }
        Sleep(50);
    }
    for (int i = 0; i < numRacers-1; i++)
    {
        for (int j = 0; j < numRacers - i - 1; j++) {
            if (r[j].time > r[i].time) {
                Racer t = r[j];
                r[j] = r[j + 1];
                r[j + 1] = t;
            } 
        }
    }
    for (int i = 0; i < numRacers; i++)
    {
        r[i].place = i + 1;
    }
    cout << "Конец гонки\n";
    cout << "Место | PID | Имя | Приоритет | Время\n";
    for (int i = 0; i < numRacers; i++)
    {
        cout << " " << r[i].place << "  | " << r[i].pi.dwProcessId << " | " << r[i].name << " | " << getPriorityName(r[i].priorityClass)
            << "  | " << r[i].time << "\n";
        CloseHandle(r[i].pi.hThread);
        CloseHandle(r[i].pi.hProcess);
    }
}
