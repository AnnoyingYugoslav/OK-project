#include<iostream>
#include<ctime>
#include<fstream>
#include<vector>
#include<tuple>
#include<array>

#define MAX_TIME 500

using Task = std::tuple<int,int>;
using Job = std::vector<Task>;


std::vector<Job> readProblem1(const std::string& filename, std::tuple<int,int>* info){ //tworzy vector problemu
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Nie można otworzyć pliku " << filename << "\n";
        return {};
    }
    int rows, colm;
    if (!(file >> rows >> colm)) {
        std::cerr << "Error: Błąd w pierwszej lini" << "\n";
        return {};
    }
    *info = std::make_tuple(rows, colm);
    std::vector<Job> problem;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < colm; j++){
            int x, y;
            if (!(file >> x >> y)) {
                std::cerr << "Error: błąd czytania w lini" << (i + 1) << "\n";
                return {};
            }
            problem[i][j] = std::make_tuple(x, y);
    }
    }
    return problem;
}

std::vector<Job> readProblem2(const std::string& filename, std::tuple<int,int>* info){
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Nie można otworzyć pliku " << filename << "\n";
        return {};
    }
    int rows, colm;
    if (!(file >> rows >> colm)) {
        std::cerr << "Error: Błąd w pierwszej lini" << "\n";
        return {};
    }
    *info = std::make_tuple(rows, colm);
    std::vector<Job> problem;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < colm; j++){
            int x;
            if (!(file >> x)) {
                std::cerr << "Error: błąd czytania w lini" << (i + 1) << "\n";
                return {};
            }
            problem[i][j] = std::make_tuple(0, x);
        }
    }
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < colm; j++){
            int x;
            if (!(file >> x)) {
                std::cerr << "Error: błąd czytania w lini" << (i + 1) << "\n";
                return {};
            }
            std::get<0>(problem[i][j]) = x;
        }

    }
    return problem;
}

std::vector<std::vector<int>> solveProblem(std::vector<Job>, int maxtime){ //rozwiązuje vector problemu z limitem czasowym, metoda GRASP
    int parametr = 1;
}

//wywoałne : ./a.out plikodczytu format(01) ewentualnymaxczas(ms)
int main(int argc, char* argv[]){
    std::tuple<int,int> info;
    std::vector<std::vector<int>> solution;
    std::vector<Job> problem;
    if(*argv[2] == '0'){
        problem = readProblem1(argv[1], &info);
    }
    else if(*argv[2] == '1'){
        problem = readProblem2(argv[1], &info);
    }
    else{
        std::cerr << "Error: Nie poprawna metoda wywłoania. Wybierz zakres 0 do 1 \n";
    }
    if(sizeof(argv) > 3){
        solution = solveProblem(problem, *argv[3]);
    }
    else{
        solution = solveProblem(problem, MAX_TIME);
    }
}