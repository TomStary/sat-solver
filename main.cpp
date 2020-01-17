#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class Clause {
public:
    Clause(std::vector<int> clause);
    bool Fulfilled(std::vector<bool> genom);
private:
    std::vector<int> variables;
};

Clause::Clause(std::vector<int> clause)
{
    variables = clause;
}

bool Clause::Fulfilled(std::vector<bool> genom)
{
    bool res = true;
    for (auto& var: variables) {
        if (var < 0) {
            res = res && !genom[std::abs(var)];
        } else {
            res = res && genom[var];
        }
    }

    return res;
}

class SAT {
public:
    SAT(std::vector<int> weights, std::vector<std::vector<int>> clauses);
    ~SAT();
    int CalculateWeight(std::vector<bool> genom);
    int CalculateFulfilled(std::vector<bool> genom);
private:
    std::vector<Clause> clauses;
    std::vector<int> weights;
};

SAT::SAT(std::vector<int> weights, std::vector<std::vector<int>> clauses)
{
    this->weights = weights;
    for (auto &clause: clauses) {
        this->clauses.push_back(Clause(clause));
    }
}

SAT::~SAT()
{}

int SAT::CalculateWeight(std::vector<bool> genom)
{
    int res = 0;
    for (int i = 0; i < weights.size(); i++) {
        if (genom[i]) {
            res += weights[i];
        }
    }
    return res;
}

int SAT::CalculateFulfilled(std::vector<bool> genom)
{
    int res = 0;
    for (auto& clause: clauses) {
        if (clause.Fulfilled(genom)) {
            res++;
        }
    }
    return res;
}

class Specimen {
public:
    Specimen(std::shared_ptr<SAT> sat, int numberOfVariables);
    int Fitness();
private:
    std::shared_ptr<SAT> sat;
    std::vector<bool> genom;
};

Specimen::Specimen(std::shared_ptr<SAT> sat, int numberOfVariables) : sat(sat)
{
    for (int i = 0; i < numberOfVariables; i++) { // Random genom initialization
        genom.push_back(rand()%2);
    }
}

int Specimen::Fitness()
{
    return sat->CalculateFulfilled(genom);
}

class SATSolver {
public:
    SATSolver(std::vector<int> weights, std::vector<std::vector<int>> clauses);
    void Solve();
private:
    int variablesCount;
    int clausesCount;
    int pop_size = 100;
    std::shared_ptr<SAT> satProblem;
    std::vector<Specimen> population;
};

SATSolver::SATSolver(std::vector<int> weights, std::vector<std::vector<int>> clauses)
{
    variablesCount = weights.size();
    clausesCount = clauses.size();
    satProblem = std::make_shared<SAT>(weights, clauses);
    for (int i = 0; i < pop_size; i++) {
        population.push_back(Specimen(satProblem, variablesCount));
    }
}

void SATSolver::Solve()
{
    for (auto& specimen: population) {
        std::cout << "Fitness: " << specimen.Fitness() << std::endl;
    }
}

void initiate_algorithm(std::string filename)
{
    std::ifstream file (filename);

    if (!file.is_open()) {
        throw std::runtime_error("File could not be open.");
    } else {
        //now we can read and parse file
        std::string line;
        int count_variables, count_clauses;
        std::vector<int> weights = std::vector<int>();
        std::vector<std::vector<int>> clauses = std::vector<std::vector<int>>();
        while (std::getline(file, line)) {
            std::istringstream iss (line);
            char firstChar;
            std::string type;
            if (line[0] == 'c') {
                continue;
            } else if (line[0] == 'p') {
                iss >> firstChar;
                iss >> type >> count_variables >> count_clauses;
            } else if (line[0] == 'w') {
                iss >> firstChar;
                for (int i = 0; i < count_variables; i++) { //read all weights
                    int weight;
                    iss >> weight;
                    weights.push_back(weight);
                }
            } else if (line[0] == '%') {
                break;
            } else {
                int a, b, c;
                iss >> a >> b >> c;
                std::vector<int> clause = { a, b, c };
                clauses.push_back(clause);
            }
        }

        auto satSolver = SATSolver(weights, clauses);
        satSolver.Solve();
    }
}

int main(int argc, char ** argv)
{
    std::srand(std::time(0));
    if (argc != 2) {
        std::cerr << "You have to provide a file to analyze." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    initiate_algorithm(filename);
    return 0;
}