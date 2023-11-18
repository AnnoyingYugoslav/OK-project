#include<iostream>
#include<ctime>
#include<fstream>
#include<vector>
#include<tuple>
#include<array>
#include <cstdlib>
#include <chrono>
#include<algorithm>
#include<random>

#define MAX_TIME 150

using Task = std::tuple<int,int>;
using Job = std::vector<Task>;

bool isMember(int value, std::vector<int> testCase){
    for(int i = 0; i < testCase.size(); i++){
        if(value == testCase[i]){
            return true;
        }
    }
    return false;
}

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
            problem[i].push_back(std::make_tuple(x, y));
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

std::vector<std::vector<int>> solveProblem(std::vector<Job> zad, int maxtime, std::tuple<int,int> info){ //rozwiązuje vector problemu z limitem czasowym, metoda GRASP
    const std::chrono::seconds timeout(MAX_TIME);
    auto start_time = std::chrono::steady_clock::now();


    int par0 = 1; //ile maszyn - tunning parametr
    int par1 = 4; //ile procesóww - tunning parametr+


    std::vector<std::vector<int>> sol; //rozwiązanie, po jobach
    int endTime = -1;
    int ilePow = 0;

    std::random_device rd;
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    while(1){
        //ilePow++;
        //std::cout<<"KOLEJNA PETLA\n";
        std::vector<Job> localZad = zad;
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        if (elapsed_time >= timeout){
            return sol;
            /*
            for(int i = 0; i < sol.size(); i++){
                for(int j = 0; j < sol.at(i).size(); j++){
                    std::cout << sol[i][j] << " ";
                }
                std::cout << " \n";
            }
            return;
            */
        }
        int notDone = 1;
        std::vector<int> localCurrentEndTimeP; // czas, w którym dany proces się aktualnie kończy
        std::vector<int> localCurrentEndTimeM; // czas, w którym dana mszyna się aktualnie kończy
        std::vector<int> machineUse; //ile jeszcze czasu na maszynach
        std::vector<std::vector<int>> localSol; // rozwiązanie lokalne

        //generacja danych startowych
        for(int i = 0; i < std::get<0>(info); i++){
            machineUse.push_back(0);
            localCurrentEndTimeP.push_back(0);
            localSol.push_back({});
            for(int j = 0; j < localZad.at(i).size(); j++){
                machineUse[i] = machineUse[i] + std::get<1>(localZad[i][j]);
            }
        }
        for(int i = 0; i < std::get<1>(info); i++){
            localCurrentEndTimeM.push_back(0);
        }

        while(1){
            //sprawdz, czy koniec
            notDone = 0;
            for(int i = 0; i < std::get<1>(info); i++){
                if(localZad.at(i).size() != 0){
                    notDone = 1;
                }
            }
            if (notDone==0){
                break;
            }
            //wybierz najbardziej używane maszyny
            std::vector<int> longestTime; //najkrótsze czasy
            std::vector<int> longestMach; // -||- maszyny
            longestTime = machineUse;
            std::sort(longestTime.begin(), longestTime.end(), std::greater<int>());
            longestTime.resize(par0);
            for(int j = 0; j<longestTime.size(); j++){
                for(int i = 0; i < machineUse.size(); i++){
                    if(longestTime[j] == machineUse[i]){
                        longestMach.push_back(i);
                    }
                }
            }
            //wybierz najkrótsze procesy (sprawdzając, czy conajmniej 1 jest poprawne)
            std::vector<Task> processes;
            for(int i = 0; i < std::get<1>(info); i++){
                if(localZad.at(i).size() > 0){
                    if(isMember(std::get<0>(localZad[i][0]), longestMach)){
                        processes.push_back(localZad[i][0]);
                    }
                }
            }
            if(processes.size() == 0){
                for(int i = 0; i < std::get<1>(info); i++){
                    if(localZad.at(i).size() > 0){
                        processes.push_back(localZad[i][0]);
                    }
                }
            }

            if(processes.size() == 0){
                std::cerr << "Erorr: no processes detected";
                return;
            }
            std::sort(processes.begin(), processes.end()); //to jest źle, segreguje jakby to była jedna liczba
            if(processes.size() > par1){
                processes.resize(par1);
            }
            //wylosuj jeden z nich, dodaj go go do skończonych, usuń z oczekujących
            std::uniform_int_distribution<int> distribution(0, processes.size()-1);
            int randomInt = distribution(gen);
            std::tuple<int, int> chosenElement;
            chosenElement = processes[randomInt];
            int originalTaskId = -1;
            for(int i = 0; i <std::get<1>(info); i++){
                if(std::get<0>(localZad[i][0]) == std::get<0>(chosenElement) && std::get<1>(localZad[i][0]) == std::get<1>(chosenElement)){
                    originalTaskId = i;
                }
            }
            if(originalTaskId == -1){
                std::cerr << "Error: value not found";
                return;
            }
            if (!localZad.at(originalTaskId).empty()) {
                localZad.at(originalTaskId).erase(localZad[originalTaskId].begin());
            }
            /* testing mechanism
            for (int i = 0; i < std::get<1>(info); i++) {
                std::cout << "Machine " << i << " queue size: " << localZad.at(i).size() << std::endl;
                }
            */
            //update currentEndTime, machineUse i localSol
            if(localCurrentEndTimeP[originalTaskId] >= localCurrentEndTimeM[std::get<0>(chosenElement)]){
                localSol.at(originalTaskId).push_back((localCurrentEndTimeP[originalTaskId] + std::get<1>(chosenElement)));
                localCurrentEndTimeP[originalTaskId] = localCurrentEndTimeP[originalTaskId] + std::get<1>(chosenElement);
                localCurrentEndTimeM[std::get<0>(chosenElement)] = localCurrentEndTimeP[originalTaskId];
            }
            else{
                localSol[originalTaskId].push_back((localCurrentEndTimeM[std::get<0>(chosenElement)] + std::get<1>(chosenElement)));
                localCurrentEndTimeM[std::get<0>(chosenElement)] = localCurrentEndTimeM[std::get<0>(chosenElement)] + std::get<1>(chosenElement);
                localCurrentEndTimeP[originalTaskId] = localCurrentEndTimeM[std::get<0>(chosenElement)];
            }
            longestTime[originalTaskId] = longestTime[originalTaskId] - std::get<1>(chosenElement);
        }
        //if max(localEndTime) < EndTime (if not -1) -> sol = localSol, EndTime = max(localEndTime)
        int localEndTime = -1;
        for(int i = 0; i < std::get<1>(info); i++){
            if(localCurrentEndTimeP[i] > localEndTime){
                localEndTime = localCurrentEndTimeP[i];
            }
        }
        if(localEndTime == -1){
            std::cerr << "Error: no solution found";
            return;
        }
        std::cout<<"Local end time: "<<localEndTime<<"\n";
        if(endTime == -1){
            sol = localSol;
            endTime = localEndTime;
        }
        else if(endTime > localEndTime){
            sol = localSol;
            endTime = localEndTime;
        }
        ilePow++;
    }
}

//wywoałne : ./a.out plikodczytu format(01) ewentualnymaxczas(ms)
int main(int argc, char* argv[]){
    std::tuple<int,int> info;
    std::vector<std::vector<int>> solution;
    std::vector<Job> problem;
    int x = atoi(argv[2]);
    if(x == 0){
        problem = readProblem1(argv[1], &info);
    }
    else if(x == 1){
        problem = readProblem2(argv[1], &info);
    }
    else{
        std::cerr << "Error: Nie poprawna metoda wywłoania. Wybierz zakres 0 do 1 \n";
    }
    if(argc > 3){
        int y = atoi(argv[3]);
        solution = solveProblem(problem, y, info);
    }
    else{
        solution = solveProblem(problem, MAX_TIME, info);
    }
    for(int i = 0; i < solution.size(); i++){
        for(int j = 0; j <  solution[i].size(); j++){
            std::cout << solution[i][j] - std::get<1>(problem[i][j]) << " ";
        }
        std::cout << "\n";
    }
}