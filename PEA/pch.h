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

// Dzie� dobry,
// 
// funkcja celu zwraca / przyporz�dkowuje warto�� dla pojedynczego przyk�adu w zale�no�ci od kryterium optymalizacyjnego dla danego problemu.
// W wypadku problemu komiwoja�era  jest to d�ugo�� �cie�ki jak� musi przeby� komiwoja�er dla danej permutacji.
// Algorytm przegl�du zupe�nego(Brute Force) sprawdza warto�� funkcji celu dla wszystkich permutacji(n!) 
// i zwraca rozwi�zanie dla kt�rego funkcja celu ma najlepsze mo�liwe rozwi�zanie(w tym wypadku najkr�tsz� �cie�k�).
// Rozwi�zaniem problemu jest permutacja(kolejno�� odwiedzanych miast) i d�ugo�� �cie�ki.
// 
// Podsumowuj�c program w etapie 0 powinien:
// Wczyta� instancj� problemu z pliku,
// Wygenerowa� permutacje naturaln�(1, 2, 3, ..., n).
// Policzy� i wy�wietli�  d�ugo�� �cie�ki : d(1, 2) + d(2, 3) + ... d(n - 1, n) + d(n, 1), gdzie d(j, k) to odleg�o�� mi�dzy miastem "j" i "k".
// Dodatkowo mo�na doda� mo�liwo�� wygenerowania permutacji losowej i / lub zadanej przez u�ytkownika.
// 
// Pozdrawiam,
// Rados�aw Idzikowski