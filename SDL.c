//This file contains visualisations with SDL
//It simulates wave of water after throwing smth in it
//It was used for final project for OOP in 2019 in MIPT by me

#include <iostream>
#include <SDL.h>
#include <vector>
#include <thread>
#include <chrono>
#include <math.h>
#include <stdlib.h>

using namespace std;

void foo(const int& N, const int& M, const double& h, const double& t, double*** u, const int& Num_of_thr, const int& Num_of_thr_all)
{
	for (int n = (Num_of_thr - 1 )* ((N) / Num_of_thr_all); n < Num_of_thr * ((N) / Num_of_thr_all); n++) {
		for (int i = 1; i < M; i++)
			for (int j = 1; j < M; j++) {
				if ((n == 1) && (i == 50) && (j == 50)) { // создаем "бульк" (задаем начальное возмущение волны)
					u[n][i][j] = 3000;
				}
				else {
					if ((n == 0) || (n == 1)) { //задаем начальные условия
						u[n][i][j] = 0;
					}
					else { // все остальные точки
						u[n][i][j] = (t / h) * (t / h) * (u[n - 1][i + 1][j] + u[n - 1][i][j + 1] - 4 * u[n - 1][i][j] + u[n - 1][i - 1][j] + u[n - 1][i][j - 1]) - u[n - 2][i][j] + 2 * u[n - 1][i][j];
					}
				}
			}
		for (int i = 0; i < M + 1; i++) { // тут задаем граничные условия
			u[n][i][0] = 0;
			u[n][0][i] = 0;
			u[n][i][M] = 0;
			u[n][M][i] = 0;
		}
	}
}

int main(int, char**) {

	double L = 20, T = 10;
	int N = 300, M = 300; // N должен быть равен M
	
	double h = L / (M - 1),
		t = T / (N - 1);

	int Num_of_thr = 0,
		Num_of_thr_all = 0;
    
  chrono::steady_clock::time_point start;
	chrono::steady_clock::time_point end;

	// будем хранить данные в трехмерном массиве (поскольку отрисовываем по кадрам то есть [момент времени][коор по х][коор по у])
	double*** u = new double** [N];
	for (int i = 0; i < N; i++) {
		u[i] = new double* [M];
		for (int j = 0; j < M + 1; j++)
			u[i][j] = new double[M + 1];
	}
  
  // 1 поток (следовало бы распараллелить на 4 потока, но нет....)
	start = chrono::high_resolution_clock::now();
	Num_of_thr_all = 1;	
	Num_of_thr = 1;
	thread t_1([&N, &M, &h, &t, u, &Num_of_thr_all, &Num_of_thr]()
		{foo(N, M, h, t, u, Num_of_thr, Num_of_thr_all); });
	t_1.join();
	end = chrono::high_resolution_clock::now();
	cout << "Time with " << Num_of_thr_all << " thread(s) : " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;

	// необходимые константы для варьирования вывода (вместо 1 пикселя А/В)
	const int A = 8;
	const int B = 8;

  // сейчас начинается реализация SDL
  // используем события
	SDL_Event event;
	
	// инициализируем SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}	

	// создаем окно
	SDL_Window* window = SDL_CreateWindow("Visualisation", 150, 50, 1600, 1024, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	
	// создаем рендерер
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	bool flag = false;
  
  SDL_RenderClear(renderer);
	while (1) {
		if ((SDL_PollEvent(&event) && event.type == SDL_QUIT) || (flag == true)) 	//окно висит пока не отработает событие
			break;
		for (int n = 0; n < N; n++) {
			if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
				flag = true;
				break;
			}
			SDL_RenderClear(renderer);
			for (int i = 0; i < M; i++)
				for (int j = 0; j < M; j++) {
					SDL_SetRenderDrawColor(renderer, u[n][i][j], u[n][i][j], 0, 255);
					for (int k = 0; k < A / B; k++) { // тут просто уведичиваем кратинку путем рисования вместо 1 пикселя А/В пикселей
						for (int l = 0; l < A / B; l++) {
							SDL_RenderDrawPoint(renderer, i * A + l, j * A + k);
						}
					}
				}
			SDL_RenderPresent(renderer);
		}
	}

	//окно висит пока не отработает событие (чтобы не сразу все удалялось)
	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}

	// закрываем все
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
