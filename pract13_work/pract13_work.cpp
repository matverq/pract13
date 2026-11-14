#include <iostream>
#include <Windows.h>
#include <conio.h>

using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	if (argc != 3) {
		cout << "Ошибка передачи диапозона";
		return 1;
	}
	
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);

	DWORD sTime = GetTickCount();
	int c = 0;

	for (int i = a; i <= b; i++)
	{
		if (i <= 1) continue;
		bool tf = true;
		for (int j = 2; j * j <= i; j++)
		{
			if (i % j == 0) {
				tf = false;
				break;
			}
		}
		if (tf) c++;
	}

	DWORD eTime = GetTickCount();
	DWORD time = eTime - sTime;

	cout << "Диапозон от " << a << " до " << b << "\n";
	cout << "Найдено простых чисел: " << c << "\n";
	cout << "Потрачено времени: " << time << "м/с";
	_getch();
	return time;
}
