#pragma once
#include <Windows.h>
#undef max

LARGE_INTEGER getTime();

template<typename T>
T getting()
{
	T a;
	std::cin >> a;
	while (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Error, try again: ";
		std::cin >> a;
	}
	return a;
}

// Dzieñ dobry,
// 
// funkcja celu zwraca / przyporz¹dkowuje wartoœæ dla pojedynczego przyk³adu w zale¿noœci od kryterium optymalizacyjnego dla danego problemu.
// W wypadku problemu komiwoja¿era  jest to d³ugoœæ œcie¿ki jak¹ musi przebyæ komiwoja¿er dla danej permutacji.
// Algorytm przegl¹du zupe³nego(Brute Force) sprawdza wartoœæ funkcji celu dla wszystkich permutacji(n!) 
// i zwraca rozwi¹zanie dla którego funkcja celu ma najlepsze mo¿liwe rozwi¹zanie(w tym wypadku najkrótsz¹ œcie¿kê).
// Rozwi¹zaniem problemu jest permutacja(kolejnoœæ odwiedzanych miast) i d³ugoœæ œcie¿ki.
// 
// Podsumowuj¹c program w etapie 0 powinien:
// Wczytaæ instancjê problemu z pliku,
// Wygenerowaæ permutacje naturaln¹(1, 2, 3, ..., n).
// Policzyæ i wyœwietliæ  d³ugoœæ œcie¿ki : d(1, 2) + d(2, 3) + ... d(n - 1, n) + d(n, 1), gdzie d(j, k) to odleg³oœæ miêdzy miastem "j" i "k".
// Dodatkowo mo¿na dodaæ mo¿liwoœæ wygenerowania permutacji losowej i / lub zadanej przez u¿ytkownika.
// 
// Pozdrawiam,
// Rados³aw Idzikowski