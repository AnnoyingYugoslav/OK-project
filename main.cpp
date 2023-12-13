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
#define PAR 70
#define CUTTER 0.2
#define MUTATE 0.2 //od 0 do 1

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

void mutate(std::vector<int>* funcToMutate, std::mt19937& gen){
    int size = funcToMutate->size();
    std::uniform_int_distribution<int> dist(0, size - 1);
    for(int i = 0; i < size; i++){
        if(dist(gen) < (MUTATE*(size-1))){
            int a = dist(gen); 
            int b = dist(gen); 
            while (a == b) {
                b = dist(gen);
            }
            int tempor = funcToMutate->at(a);
            funcToMutate->at(a) = funcToMutate->at(b);
            funcToMutate->at(b) = tempor;
        }
    }
}


void fixVector(std::vector<int>* exaSol, std::vector<int>* forFix,int ileProc, int ileMasz){
    int solLength = ileProc*ileMasz;
    int fixingChoice = -1;
    forFix->resize(0);
    exaSol->resize(solLength);
    /*
    for(int i = 0; i < exaSol->size(); i++){
        std::cout << exaSol->at(i) << " ";
    }
    */
    for(int i = 0; i < ileProc; i++){
        forFix->push_back(0);
    }
    //zsumuj, co już jest
   
    for(int i = 0; i < solLength; i++){
        if((exaSol->at(i) < ileProc) && (exaSol->at(i) > -1)){
            forFix->at(exaSol->at(i))++;
        }
    }
    /*
    for(int i = 0; i < ileProc; i++){
        std::cout << forFix->at(i);
    }
    std::cout << "\n";
    */
    //weryfikuj, który trzeba usunac - podmien
    for(int i = 0; i < solLength; i++){
        fixingChoice = -1;
        if((exaSol->at(i) > ileProc-1) || (exaSol->at(i) < 0)){
            exaSol->at(i) = 0;
            forFix->at(0)++;
        }
        if((forFix->at(exaSol->at(i)) > ileMasz)){
           // std::cout << "Fix " << exaSol->at(i) << " at " << i << "\n";
            for(int j = 0; j < ileProc; j++){
                if(forFix->at(j) < ileMasz){
                    fixingChoice = j;
                }
            }
           // std::cout << "Co wybrano " << fixingChoice <<"\n";
            if(fixingChoice == -1){
                std::cerr << "Error: fixing value not found";
                return;
            }
            forFix->at(exaSol->at(i))--;
            exaSol->at(i) = fixingChoice;
            forFix->at(fixingChoice)++;
        }
    }
}

int calculateTimes(std::vector<int>* locAlgo, std::vector<Job>* problem, std::vector<std::vector<int>>* locSol, std::vector<int>* endP, std::vector<int>* endM, int ileP, int ileM){
    locSol->clear();
    endP->clear();
    endM->clear();
    //std::cout << "Start101 \n";
    int max = -1;
    for(int i = 0; i < ileP; i++){
        endP->push_back(0);
        locSol->push_back({});
    }
    for(int i = 0; i < ileM; i++){
        endM->push_back(0);
    }
    for(int i = 0; i < (ileM*ileP); i++){

       // std::cout << "Start102\n" ;
        /*
        for(int j = 0; j < problem->size(); j++){
            std::cout << problem->at(j).size();
        }
        std::cout << " ";
        for(int j = 0; j< locAlgo->size(); j++){
            std::cout << locAlgo->at(j);
        }
        std::cout << "\n" << i << "\n";
        */
        if(!problem->at(locAlgo->at(i)).empty()){
            if(endP->at(locAlgo->at(i)) >= endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0)))){
                //std::cout << "Start1021 \n";
                endP->at(locAlgo->at(i)) = endP->at(locAlgo->at(i)) + std::get<1>(problem->at(locAlgo->at(i)).at(0));
                //std::cout << "Start1022 \n";
                endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0))) = endP->at(locAlgo->at(i));
                //std::cout << "Start1023 \n";
                locSol->at(locAlgo->at(i)).push_back(endP->at(locAlgo->at(i)));
                //std::cout << "Start1024 \n";
            }
            else if(endP->at(locAlgo->at(i)) < endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0)))){
                //std::cout << "Start1025 \n";
                endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0))) = endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0))) + std::get<1>(problem->at(locAlgo->at(i)).at(0));
                //std::cout << "Start1026 \n";
                endP->at(locAlgo->at(i)) = endM->at(std::get<0>(problem->at(locAlgo->at(i)).at(0)));
                //std::cout << "Start1027 \n";
                locSol->at(locAlgo->at(i)).push_back(endP->at(locAlgo->at(i)));
                //std::cout << "Start1028 \n";
            }
            if (problem->at(locAlgo->at(i)).size() > 1) {
                int newSize = problem->at(locAlgo->at(i)).size() - 1;
                for (int m = 0; m < newSize; m++) {
                    problem->at(locAlgo->at(i)).at(m) = problem->at(locAlgo->at(i)).at(m+1);
                }
                problem->at(locAlgo->at(i)).resize(newSize);
            }
            else {
                problem->at(locAlgo->at(i)).resize(0);  // Clear the vector when it's empty
            }
        }
        else{
        std::cerr << "Error: not correct format";
        return -1;}
    }
    //std::cout << "Start103 \n";
    for(int i = 0; i < problem->size(); i++){
        //std::cout << problem->at(i).size() << " ";
    }
    //std::cout << "\n";
    for(int i = 0; i < ileP; i++){
        if(max < locSol->at(i).at(ileM-1)){
            max = locSol->at(i).at(ileM-1);
        }
    }
    //std::cout << "Start104 \n";
    return max;
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

void solveProblem(std::vector<Job>* danezad, int maxtime, std::tuple<int,int>* info1){ //rozwiązuje vector problemu z limitem czasowym, metoda GRASP
    //std::cout << "Start1 \n";
    int howmany = 0;
    std::tuple<int, int> info = (*info1);
    const std::chrono::seconds timeout(maxtime);
    auto start_time = std::chrono::steady_clock::now();
    int par = PAR;
    int par2 = par*par;
    //seed do losowania
    std::random_device rd;
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());

    //deklaracja używanych vectorów
    std::vector<std::vector<int>> sol;
    std::vector<int> bestInstance;
    std::vector<int> instance;
    std::vector<std::vector<int>> allInstances;
    std::vector<std::tuple<int,int>> resultTimes;
    std::vector<int> forFix;
    std::vector<int> endM;
    std::vector<int> endP;
    std::vector<Job> localZad;
    std::vector<std::vector<int>> parBest;
    int bestTime = -1;
    int counter = 0;

    //std::cout << "Start2 \n";
    //rezerwacja pamięci
    parBest.resize(par);
    for(int i = 0; i < par; i++){
        //std::cout << "Start20 \n";
        parBest.at(i).reserve(par2);
    }
    //std::cout << "Start21 \n";
    localZad.resize(std::get<0>(info));
    for(int i = 0; i < std::get<0>(info); i++){
        localZad[i].reserve(std::get<1>(info));
    }
    //std::cout << "Start22 \n";
    endP.reserve(std::get<0>(info));
    endM.reserve(std::get<1>(info));
    forFix.reserve(std::get<0>(info));
    bestInstance.reserve((std::get<0>(info)*std::get<1>(info)));
    resultTimes.reserve((par2*2));
    allInstances.resize((par2*2));
    instance.reserve((par2*2));
    //std::cout << "Start23 \n";
    for(int i = 0; i < (par2*2); i++){
        allInstances[i].reserve((std::get<0>(info)*std::get<1>(info)));
    }
    //std::cout << "Start24 \n";
    sol.resize(std::get<0>(info));
    for(int i = 0; i < std::get<0>(info); i ++){
        sol[i].reserve(std::get<1>(info));
    }
    //std::cout << "Start3 \n";
    //generacje wejściowych losowych danych (po par)
    for(int i = 0; i < (par2*2); i++){
        for(int j = 0; j < std::get<0>(info); j++){
            for(int k = 0; k < std::get<1>(info); k++){
                allInstances[i].push_back(j);
            }
        }
        std::shuffle(allInstances[i].begin(), allInstances[i].end(), gen);
    }
    //std::cout << "Start4 \n";
    while(1){
        //std::cout << "Start5 \n";
        //sprawdz czy koniec
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        if (elapsed_time >= timeout){
            //interpretuj bestInstance do sol
            sol.resize(0);
            endM.resize(0);
            endP.resize(0);
            for(int i = 0; i < std::get<0>(info); i++){
                endP.push_back(0);
                sol.push_back({});
            }
            for(int i = 0; i < std::get<1>(info); i++){
                endM.push_back(0);
            }
            localZad = (*danezad);
            for(int i = 0; i < (std::get<0>(info) * std::get<1>(info)); i++){
                //std::cout << "Start6 \n";
                if(endP.at(bestInstance.at(i)) >= endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0)))){
                    sol.at(bestInstance.at(i)).push_back(endP.at(bestInstance.at(i)));
                    endP.at(bestInstance.at(i)) = endP.at(bestInstance.at(i)) + std::get<1>(localZad.at(bestInstance.at(i)).at(0));
                    endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0))) = endP.at(bestInstance.at(i));
                }
                else{
                    sol.at(bestInstance.at(i)).push_back(endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0))));
                    endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0))) = endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0))) + std::get<1>(localZad.at(bestInstance.at(i)).at(0));
                    endP.at(bestInstance.at(i)) = endM.at(std::get<0>(localZad.at(bestInstance.at(i)).at(0)));
                }
                if(localZad.at(bestInstance.at(i)).size() > 1){
               	    int newSize = localZad.at(bestInstance.at(i)).size() - 1;
		            for(int m = 0; m < newSize; m++){
			            localZad.at(bestInstance.at(i)).at(m) = localZad.at(bestInstance.at(i)).at(m+1);
		            }
		            localZad.at(bestInstance.at(i)).resize(newSize);
		        }
		        else{
		            localZad.at(bestInstance.at(i)).resize(0);
		        }
            }
            //wypluj wynik
            //std::cout << "Start7 \n";
            std::cout << bestTime << " "<< howmany << "\n";
            for(int i = 0; i < sol.size(); i++){
                for(int j = 0; j < sol.at(i).size(); j++){
                    std::cout << sol[i][j] << " ";
                }
                std::cout << " \n";
            }
            return;
        }
        //napraw aktualnie zapisane - zewnętrzna funckja
        //std::cout << "Start9 \n";
        for(int i = 0; i < (par2*2); i++){
            fixVector(&allInstances[i], &forFix, std::get<0>(info), std::get<1>(info));
        }
        //std::cout << "Start10 \n";
        //policz i zapisz dla każdego czas - zewnętrzna funkcja
        resultTimes.resize(0);
        for(int i = 0; i < (par2*2); i++){
            localZad = (*danezad);
            resultTimes.push_back(std::make_tuple(i, calculateTimes(&allInstances[i], &localZad, &sol, &endP, &endM, std::get<0>(info), std::get<1>(info))));
        }
        //std::cout << "Start11 \n";
        //wez 10 najlepszych - zapisz zupełnie najlepszą
        std::sort(resultTimes.begin(), resultTimes.end(), compareSecond);
        if((bestTime == -1) || (bestTime > std::get<1>(resultTimes[0]))){
            bestInstance = allInstances[std::get<0>(resultTimes[0])];
            bestTime = std::get<1>(resultTimes[0]);
        }
        //std::cout << "Start12 \n";
        //tutaj z 0 na par, bo potem wyciagasz dane z tego -? zmienione na push_back
        parBest.resize(0);
        //std::cout << "Test1 \n";
        for(int i = 0; i < par; i++){
            parBest.push_back(allInstances.at(std::get<0>(resultTimes[i])));
        }
        //std::cout << "Test2 \n";
        //pokrój je w parametr (wstępnie 70-30) i stwórz możliwe
        allInstances.resize(0);
        //std::cout << "Test3 \n";
        counter = 0;
        //std::cout << "Start13 \n";
        for(int i = 0; i < par; i++){
            for(int j = 0; j < par; j++){
                allInstances.push_back({});
                for(int m = 0; m < (CUTTER * (std::get<0>(info)*std::get<1>(info))); m++){
                    allInstances[counter].push_back(parBest.at(i).at(m));
                    //std::cout << "1 ";
                }
                for(int m = (CUTTER * (std::get<0>(info)*std::get<1>(info))); m < (std::get<0>(info)*std::get<1>(info)); m++){
                    allInstances[counter].push_back(parBest.at(j).at(m));
                    //std::cout << "2 ";
                }
                counter++;
            }
        }
        for(int i = 0; i < par2; i++){
            allInstances.push_back(allInstances[i]);
            mutate(&allInstances.at(counter), gen);
            counter++;
        }
        howmany++;
    }
}    


//wywoałne : ./a.out plikodczytu format(01) ewentualnymaxczas(ms)
int main(int argc, char* argv[]){
    std::tuple<int,int> info;
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
   //std::get<0>(info) = std::get<0>(info) -0; //-> zmniejszenie instancji
    if(argc > 3){
        int y = atoi(argv[3]);
        solveProblem(&problem, y, &info);
    }
    else{
        solveProblem(&problem, MAX_TIME, &info);
    }
    return 0;
}