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
    int ClauseWeight(std::vector<bool> genom, std::vector<int> weights);
private:
    std::vector<int> variables = std::vector<int>();
};

Clause::Clause(std::vector<int> clause)
{
    variables = clause;
}

bool Clause::Fulfilled(std::vector<bool> genom)
{
    bool res;
    for (auto& var: variables) {
        if (var < 0) {
            res = !genom[std::abs(var)];
        } else {
            res = genom[var];
        }
        if (!res) {
            return res;
        }
    }
    return true;
}

int Clause::ClauseWeight(std::vector<bool> genom, std::vector<int> weights)
{
    int res = 0;
    for (auto& var: variables) {
        if (genom[std::abs(var)]) {
            res += weights[std::abs(var)];
        }
    }
    return res;
}

class SAT {
public:
    SAT(std::vector<int> weights, std::vector<std::vector<int>> clauses);
    ~SAT();
    double CalculateWeight();
    double CalculateFulfilled(std::vector<bool> genom);
    double CalculateClausesWeight(std::vector<bool> genom);
    double ClausesSize();
private:
    std::vector<Clause> clauses = std::vector<Clause>();
    std::vector<int> weights = std::vector<int>();
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

double SAT::CalculateWeight()
{
    double res = 0;
    for (int i = 0; i < weights.size(); i++) {
        res += weights[i];
    }
    return res;
}

double SAT::CalculateFulfilled(std::vector<bool> genom)
{
    double res = 0;
    for (auto& clause: clauses) {
        if (clause.Fulfilled(genom)) {
            res++;
        }
    }
    return res;
}

double SAT::CalculateClausesWeight(std::vector<bool> genom)
{
    double res = 0;
    for (auto& clause: clauses) {
        res += clause.ClauseWeight(genom, weights);
    }
    return res;
}

double SAT::ClausesSize()
{
    return (double)clauses.size();
}

class Specimen {
public:
    Specimen(std::shared_ptr<SAT> sat, int numberOfVariables);
    Specimen(const Specimen &spec);
    void Crossbreed(Specimen specimen, int position);
    std::vector<bool> GetGenom();
    void Mutate();
    void Fitness();
    double fitness;
private:
    std::shared_ptr<SAT> sat;
    std::vector<bool> genom = std::vector<bool>();
};

Specimen::Specimen(std::shared_ptr<SAT> sat, int numberOfVariables) : sat(sat)
{
    for (int i = 0; i < numberOfVariables; i++) { // Random genom initialization
        genom.push_back(rand()%2);
    }
}

Specimen::Specimen(const Specimen &spec)
{
    genom = std::vector<bool>(spec.genom);
    sat = spec.sat;
    fitness = spec.fitness;
}

void Specimen::Mutate()
{
    int random = (rand()%(int)genom.size());
    genom[random] = !genom[random];
}

void Specimen::Fitness()
{
    //fitness = std::pow((sat->CalculateClausesWeight(genom)/sat->CalculateWeight(genom)), (sat->ClausesSize() + 10 - sat->CalculateFulfilled(genom)));
    fitness = sat->CalculateClausesWeight(genom);
}

void Specimen::Crossbreed(Specimen specimen, int position)
{
    std::vector<bool> secondGenom = specimen.GetGenom();
    for (int i = position; i < genom.size(); i++) {
        genom[i] = secondGenom[i];
    }
}

std::vector<bool> Specimen::GetGenom()
{
    return genom;
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
    std::vector<Specimen> population = std::vector<Specimen>();
    void Crossbreeding();
    void Mutation();
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
    for (int i = 0; i < 100; i++) {
        for (auto& specimen: population) {
            specimen.Fitness();
        }
        std::sort(population.begin(), population.end(), [](const Specimen& a, const Specimen& b) {
            return a.fitness < b.fitness;
        });
        std::cout << satProblem->CalculateFulfilled(population[0].GetGenom()) << std::endl;
        auto genom = population[0].GetGenom();
        for (int e = 0; e < genom.size(); e++) {
            std::cout << (genom[e] ? "1" : "-1") << " ";
        }
        std::cout << std::endl;
        Crossbreeding();
        Mutation();
    }
    for (auto& specimen: population) {
        specimen.Fitness();
    }
}

void SATSolver::Crossbreeding()
{
    std::vector<Specimen> theBestOutOfBest = std::vector<Specimen>(); //dont ask just accept it
    while (theBestOutOfBest.size() < pop_size) {
        std::vector<Specimen> braveWarriors = std::vector<Specimen>();
        for (int i = 0; i < 50; i++) {
            int poorBoi = std::rand() % (pop_size);
            braveWarriors.push_back(population[poorBoi]);
        }
        std::sort(braveWarriors.begin(), braveWarriors.end(), [](const Specimen& a, const Specimen& b) {
            return a.fitness < b.fitness;
        });

        double prob = 0.65; //percentual probability for selecting the best one
        int i = 0;
        for (auto &specimen: braveWarriors) {
            double resProb = double(std::rand() % 100)/100.0;
            if (i == 0 && resProb <= prob) {
                theBestOutOfBest.push_back(specimen);
                prob = prob * (1-prob);
                i++;
            } else if (resProb <= prob) {
                theBestOutOfBest.push_back(specimen);
                prob = 0.65 * std::pow(0.35, i);
                i++;
            }
            if (theBestOutOfBest.size() == pop_size) {
                break;
            }
        }
    }

    std::vector<Specimen> tmp = std::vector<Specimen>{ population };
    std::sort(tmp.begin(), tmp.end(), [](const Specimen& a, const Specimen& b) {
        return a.fitness < b.fitness;
    });
    // Elitarism
    population = std::vector<Specimen> { theBestOutOfBest };
    population[0] = tmp[0];
    population[1] = tmp[1];

    for (int i = 2; i < pop_size; i = i+2) {
        int cut = (rand() % variablesCount);
        int probability = rand() % 100;
        if (probability <= 95) {
            auto tmp = Specimen(population[i]);
            population[i].Crossbreed(population[i+1], cut);
            population[i+1].Crossbreed(tmp, cut);
        }
    }
}

void SATSolver::Mutation()
{
    for (int i = 2; i < pop_size; i++) {
        int sel = (rand()%1000);
        if (sel > 50) {
            continue;
        }
        population[i].Mutate();
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