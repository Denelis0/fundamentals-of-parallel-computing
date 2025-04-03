#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>  
#include <omp.h>

using namespace std;

const int FIELD_SIZE = 25;  // Размер игрового поля 
const double CELL_LIVE_PROBABILITY = 0.2; // Вероятность появления живой клетки
const char CELL_ALIVE = 'О', CELL_DEAD = '-'; 

// Функция для очистки экрана
void clear_screen() {
    cout << "\033[H\033[J";
}

// Функция для генерации начального состояния поля
void initialize_field_state(vector<vector<char>>& grid) {
    srand(time(nullptr));
    for (int row = 0; row < FIELD_SIZE; ++row)
        for (int col = 0; col < FIELD_SIZE; ++col)
            grid[row][col] = (rand() / (double)RAND_MAX < CELL_LIVE_PROBABILITY) ? CELL_ALIVE : CELL_DEAD;
}

// Подсчёт живых соседей для клетки
int calculate_live_neighbors(const vector<vector<char>>& grid, int x, int y) {
    int live_neighbors_count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; 
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < FIELD_SIZE && ny >= 0 && ny < FIELD_SIZE && grid[nx][ny] == CELL_ALIVE) {
                ++live_neighbors_count;
            }
        }
    }
    return live_neighbors_count;
}

// Обновление состояния поля с параллельной обработкой
void update_field_state_parallel(const vector<vector<char>>& current, vector<vector<char>>& next) {
#pragma omp parallel for collapse(2) 
    for (int row = 0; row < FIELD_SIZE; ++row) {
        for (int col = 0; col < FIELD_SIZE; ++col) {
            int live_neighbors = calculate_live_neighbors(current, row, col);
            if (current[row][col] == CELL_ALIVE) {
                next[row][col] = (live_neighbors == 2 || live_neighbors == 3) ? CELL_ALIVE : CELL_DEAD;
            }
            else {
                next[row][col] = (live_neighbors == 3) ? CELL_ALIVE : CELL_DEAD;
            }
        }
    }
}

// Функция для вывода текущего состояния поля
void display_game_grid(const vector<vector<char>>& grid, int iteration) {
    clear_screen(); 
    cout << "Итерация: " << iteration << "\n";
    for (int row = 0; row < FIELD_SIZE; ++row) {
        for (int col = 0; col < FIELD_SIZE; ++col) {
            if (grid[row][col] == CELL_ALIVE) {
                cout << "\033[34mO\033[0m ";  // Синий цвет для живых клеток
            }
            else {
                cout << "\033[31m.\033[0m ";  // Красный цвет для мертвых клеток
            }
        }
        cout << endl;
    }
    Sleep(100); // Задержка 100 мс
}

int main() {
    setlocale(LC_ALL, "RU");
    vector<vector<char>> current_grid(FIELD_SIZE, vector<char>(FIELD_SIZE));
    vector<vector<char>> next_grid_state(FIELD_SIZE, vector<char>(FIELD_SIZE));

    initialize_field_state(current_grid);
    int iteration_count = 0;

    while (true) {  // Бесконечный цикл
        display_game_grid(current_grid, iteration_count);
        update_field_state_parallel(current_grid, next_grid_state);
        current_grid.swap(next_grid_state); // Меняем местами текущий и следующий кадр
        iteration_count++;
    }

    return 0;
}
