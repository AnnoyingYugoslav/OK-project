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
#include<stdexcept>

#define MAX_TIME 150
#define PARM 0.1
#define PARP 0.7

using Task = std::tuple<int,int>;
using Job = std::vector<Task>;

bool isMember(int value, std::vector<int>* testCase){
    for(int i = 0; i < testCase->size(); i++){;
        if(value == testCase->at(i)){
            return true;
        }
    }
    return false;
}
bool compareSecond(const std::tuple<int, int>& a, const std::tuple<int, int>& b) {
    return std::get<1>(a) < std::get<1>(b);
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
    problem.reserve(100);
    for(int i = 0; i < rows; i++){
        problem.push_back({});
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
    std::string line;
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
    std::getline(file, line);
    std::getline(file, line);
    std::vector<Job> problem;
    problem.reserve(100);
    for(int i = 0; i < rows; i++){
        problem.push_back({});
        for(int j = 0; j < colm; j++){
            int x;
            if (!(file >> x)) {
                std::cerr << "Error: blad czytania w lini " << (i + 1) << "\n";
                return {};
            }
            problem[i].push_back(std::make_tuple(0, x));
        }
    }
    std::getline(file, line);
    std::getline(file, line);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < colm; j++){
            int x;
            if (!(file >> x)) {
                std::cerr << "Error: blad czytania w lini" << (i + 1) << "\n";
                return {};
            }
            std::get<0>(problem[i][j]) = x- 1;
        }

    }
    return problem;
}

std::vector<std::vector<int>> solveProblem(std::vector<Job>* danezad, int maxtime, std::tuple<int,int>* info1){ //rozwiązuje vector problemu z limitem czasowym, metoda GRASP
    const std::chrono::seconds timeout(maxtime);
    auto start_time = std::chrono::steady_clock::now();
    std::tuple<int,int> info = *info1;
    int par0 = PARM * std::get<1>(info); //ile maszyn - tunning parametr
    int par1 = PARP * std::get<0>(info); //ile procesóww - tunning parametr+
    if(par1 == 0){
        par1 = 1;
    }
    std::vector<Job> zad;
    for(int i = 0; i < std::get<0>(info); i++){
        zad.push_back({});
        for(int j = 0; j < danezad->at(i).size(); j++){
            zad.at(i).push_back(danezad->at(i).at(j));
        }
    }
    std::vector<std::vector<int>> sol = {};
    std::vector<std::vector<int>> sol2 = {}; //rozwiązanie, po jobach
    std::vector<int> localCurrentEndTimeP; // czas, w którym dany proces się aktualnie kończy
    std::vector<int> localCurrentEndTimeM; // czas, w którym dana mszyna się aktualnie kończy
    std::vector<int> machineUse; //ile jeszcze czasu na maszynach
    std::vector<int> longestTime; //najkrótsze czasy
    std::vector<int> longestMach; // -||- maszyny
    std::vector<Task> processes;
    std::vector<Job> localZad; //do wybierania procesów
    std::vector<std::vector<int>> localSol; // rozwiązanie lokalne
    int endTime = -1;
    int ilePow = 0;

    //experimental reserve
    ///*
    localCurrentEndTimeM.reserve(std::get<0>(info));
    localCurrentEndTimeP.reserve(std::get<1>(info));
    machineUse.reserve(std::get<1>(info));
    longestTime.reserve(std::get<0>(info));
    longestMach.reserve(std::get<1>(info));
    processes.reserve(std::get<0>(info));
    localZad.reserve(std::get<0>(info));
    sol.reserve(std::get<0>(info));
    sol2.reserve(std::get<0>(info));
    localSol.reserve(std::get<0>(info));
    for(int i = 0; i < std::get<0>(info); i++){
            sol[i].reserve(std::get<1>(info));
            sol2[i].reserve(std::get<1>(info));
            localSol[i].reserve(std::get<1>(info));
    }
    //*/


    std::random_device rd;
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    while(1){
        //ilePow++;
        //std::cout<<"KOLEJNA PETLA\n";
        localZad = zad;
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        if(sol.size() > 0){
            /*
            for(int i = 0; i < std::get<0>(info); i++){
                for(int j = 0; j <  std::get<1>(info); j++){
                std::cout << sol[i][j] - std::get<1>(zad[i][j]) << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";
            */
            int works = 1;
            for(int i = 0; i < std::get<0>(info); i++){
                for(int j = 0; j <  std::get<1>(info); j++){
                    if(sol[i][j] < 0){
                        works = 0;
                    }
                }
            }
            if(works == 0){
                sol = sol2;
            }
            else{
                sol2 = sol;
            }
        }
        //std::cout << "Checkpoint timeout in\n";
        if (elapsed_time >= timeout){
            std::cout << endTime << "\n";
            for(int i = 0; i < std::get<0>(info); i++){
                for(int j = 0; j <  std::get<1>(info); j++){
                std::cout << sol[i][j] - std::get<1>(zad[i][j]) << " ";
                }
                std::cout << "\n";
            }
            //std::cout << ilePow;
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
        //std::cout << "Checkpoint timeout out\n";
        int notDone = 1;
        localCurrentEndTimeP.clear(); // czas, w którym dany proces się aktualnie kończy
        localCurrentEndTimeM.clear(); // czas, w którym dana mszyna się aktualnie kończy
        machineUse.clear(); //ile jeszcze czasu na maszynach
        //std::cout << "Checkpoint generation in\n";
        //generacja danych startowych
        for(int i = 0; i < std::get<1>(info); i++){
            localCurrentEndTimeM.push_back(0);
            machineUse.push_back(0);
        }
        for(int i = 0; i < std::get<0>(info); i++){
            localCurrentEndTimeP.push_back(0);
            localSol.push_back({});
            for(int j = 0; j < localZad.at(i).size(); j++){
                machineUse[std::get<0>(localZad[i][j])] = machineUse[std::get<0>(localZad[i][j])] + std::get<1>(localZad[i][j]);
            }
        }
        //std::cout << "Checkpoint geeration out\n";
        while(1){
            try{
            //td::cout << "Checkpoint small loop in\n";
            //sprawdz, czy koniec
            notDone = 0;
            for(int i = 0; i < std::get<0>(info); i++){
                if(localZad.at(i).size() != 0){
                    /*std::cout << "Next: \n";
                    for(int i = 0; i < localZad.size(); i++){
                        for(int j = 0; j < localZad[i].size(); j++){
                            std::cout << std::get<1>(localZad[i][j]) << " ";
                        }
                        std::cout << "\n";
                    }
                    */
                    notDone = 1;
                }
            }
            if (notDone==0){
                break;
            }
            //std::cout << "Checkpoint Machines in\n";
            //wybierz najbardziej używane maszyny
            longestTime.clear(); //najkrótsze czasy
            longestMach.clear(); // -||- maszyny
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
            //std::cout << "Checkpoint Machines out\n";
            //wybierz najkrótsze procesy (sprawdzając, czy conajmniej 1 jest poprawne)
            //std::cout << "Checkpoint Processes in\n";
            processes.clear();
            for(int i = 0; i < localZad.size(); i++){
                if(localZad.at(i).size() > 0){
                    if(isMember(std::get<0>(localZad.at(i).at(0)), &longestMach)){
                        //std::cout << "Checkpoint Processes mid\n";
                            processes.push_back(localZad.at(i).at(0));
                    }
                }
            }
            if(processes.size() == 0){
                for(int i = 0; i < std::get<0>(info); i++){
                    if(localZad.at(i).size() > 0){
                            processes.push_back(localZad[i][0]);
                    }
                }
            }

            if(processes.size() == 0){
                std::cerr << "Erorr: no processes detected";
            }
            std::sort(processes.begin(), processes.end(),compareSecond); //to jest źle, segreguje jakby to była jedna liczba -> naprawione
            if(processes.size() > par1){
                processes.resize(par1);
            }
            //std::cout << "Checkpoint Processes out\n";
            //wylosuj jeden z nich, dodaj go go do skończonych, usuń z oczekujących
            //std::cout << "Checkpoint Random in\n";
            std::uniform_int_distribution<int> distribution(0, processes.size()-1);
            int randomInt = distribution(gen);
            std::tuple<int, int> chosenElement;
            chosenElement = processes[randomInt];
            //std::cout << "Checkpoint Random out\n";
            //std::cout << std::get<1>(chosenElement) << " ";
            //std::cout << "Checkpoint FindOriginal in\n";
            int originalTaskId = -1;
            for(int i = 0; i < std::get<0>(info); i++){
                if((std::get<0>(localZad[i][0]) == std::get<0>(chosenElement)) && (std::get<1>(localZad[i][0]) == std::get<1>(chosenElement)) && (!localZad.at(i).empty())){
                    originalTaskId = i;
                    //std::cout << std::get<1>(chosenElement) << " "<<  originalTaskId << " Found!\n";
                }
            }
            if(originalTaskId == -1){
                std::cerr << "Error: value not found";
            }
            //std::cout << "Checkpoint FindOriginal out\n";
            //std::cout << "Checkpoint Erase1 in\n";
            if (!localZad.at(originalTaskId).empty()) {
                localZad.at(originalTaskId).erase(localZad[originalTaskId].begin());
            }
            //std::cout << "Checkpoint Erase1 out\n";
            //std::cout << "Checkpoint2";
            /* testing mechanism
            for (int i = 0; i < std::get<1>(info); i++) {
                std::cout << "Machine " << i << " queue size: " << localZad.at(i).size() << std::endl;
                }
            */
            //update currentEndTime, machineUse i localSol
            //std::cout << "Checkpoint Update in\n";
            if(localCurrentEndTimeP[originalTaskId] >= localCurrentEndTimeM[std::get<0>(chosenElement)]){
                //std::cout << (localCurrentEndTimeP[originalTaskId] + std::get<1>(chosenElement)) << " ";
                localSol[originalTaskId].push_back((localCurrentEndTimeP[originalTaskId] + std::get<1>(chosenElement)));
                localCurrentEndTimeP[originalTaskId] = localCurrentEndTimeP[originalTaskId] + std::get<1>(chosenElement);
                localCurrentEndTimeM[std::get<0>(chosenElement)] = localCurrentEndTimeP[originalTaskId];
                //std::cout << std::get<1>(chosenElement) << " ";
            }
            else{
                //std::cout << (localCurrentEndTimeM[std::get<0>(chosenElement)] + std::get<1>(chosenElement)) << " ";
                localSol[originalTaskId].push_back((localCurrentEndTimeM[std::get<0>(chosenElement)] + std::get<1>(chosenElement)));
                localCurrentEndTimeM[std::get<0>(chosenElement)] = localCurrentEndTimeM[std::get<0>(chosenElement)] + std::get<1>(chosenElement);
                localCurrentEndTimeP[originalTaskId] = localCurrentEndTimeM[std::get<0>(chosenElement)];
                //std::cout << std::get<1>(chosenElement) << " ";
            }
            longestTime[originalTaskId] = longestTime[originalTaskId] - std::get<1>(chosenElement);
            //std::cout << "Checkpoint Update out\n";
            //std::cout << "Checkpoint3\n";
        }
        catch(const std::runtime_error& e){
            std::cerr << "Error";
        }
        }
        //if max(localEndTime) < EndTime (if not -1) -> sol = localSol, EndTime = max(localEndTime)
        int localEndTime = -1;
        for(int i = 0; i < std::get<0>(info); i++){
            if(localCurrentEndTimeP[i] > localEndTime){
                localEndTime = localCurrentEndTimeP[i];
            }
        }
        if(localEndTime == -1){
            std::cerr << "Error: no solution found";
        }
        //std::cout<<"Local end time: "<<localEndTime<<"\n";
        int brakmin = 1;
        for(int i = 0; i < std::get<0>(info); i++){
            for(int j = 0; j < std::get<1>(info); j++){
                if((localSol[i][j] - std::get<1>(zad[i][j])) < 0){
                    brakmin = 0;
                    break;
                }
            }
        }
        if((brakmin == 1) && ilePow > 2){
            if(endTime == -1){
                /*
                for(int i = 0; i < std::get<0>(info); i++){
                    for(int j = 0; j < std::get<1>(info); j++){
                    std::cout << localSol[i][j] << " ";
                    }
                std::cout << "\n";
                }
                std::cout << "\n";
                */
                sol = localSol;
                endTime = localEndTime;
                /*
                for(int i = 0; i < std::get<0>(info); i++){
                    for(int j = 0; j < std::get<1>(info); j++){
                    std::cout << sol[i][j] << " ";
                    }
                std::cout << "\n";
                }
                std::cout << "\n\n";
                */
            }
            else if(endTime > localEndTime){
                /*
                for(int i = 0; i < std::get<0>(info); i++){
                    for(int j = 0; j < std::get<1>(info); j++){
                        std::cout << localSol[i][j] << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << "\n";
                */
                sol = localSol;
                endTime = localEndTime;
                /*
                for(int i = 0; i < std::get<0>(info); i++){
                    for(int j = 0; j < std::get<1>(info); j++){
                    std::cout << sol[i][j] << " ";
                    }
                std::cout << "\n";
                }
                std::cout << "\n\n";
                */
            }
        }
        ilePow++;
        localSol.clear();
    }
}

//wywoałne : ./a.out plikodczytu format(01) ewentualnymaxczas(ms)
int main(int argc, char* argv[]){
    std::tuple<int,int> info;
    std::vector<std::vector<int>> solution;
    std::vector<Job> problem;
    if(argc < 3){
        std::cerr << "Niepoprawny format\n";
        return 1;
    }
    int x = atoi(argv[2]);
    if(x == 0){
        problem = readProblem1(argv[1], &info);
    }
    else if(x == 1){
        problem = readProblem2(argv[1], &info);
    }
    else{
        std::cerr << "Error: Nie poprawna metoda wywłoania. Wybierz zakres 0 do 1 \n";
        return 1;
    }
    /*
    for(int i = 0; i < problem.size(); i++){
        for(int j = 0; j < problem[i].size(); j++){
            std::cout << std::get<0>(problem[i][j]) << " ";
        }
        std::cout << "\n";
    }
    */
   //std::get<0>(info) = std::get<0>(info) - 16; -> zmniejszenie instancji
    if(argc > 3){
        int y = atoi(argv[3]);
        solution = solveProblem(&problem, y, &info);
    }
    else{
        solution = solveProblem(&problem, MAX_TIME, &info);
    }
    return 0;
}