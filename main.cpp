#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class Clause
{
public:
    Clause(std::vector<int> clause);
    bool Fulfilled(std::vector<bool> genom);
    int ClauseWeight(std::vector<bool> genom, std::vector<int> weights);

private:
    std::vector<int> variables = std::vector<int>();
};

class SAT
{
public:
    SAT(std::vector<int> weights, std::vector<std::vector<int>> clauses);
    ~SAT();
    double CalculateWeight();
    double CalculateFulfilled(std::vector<bool> genom);
    double CalculateClausesWeight(std::vector<bool> genom);
    double ClausesSize();
    bool Fullfiled(std::vector<bool> genom);

private:
    std::vector<Clause> clauses = std::vector<Clause>();
    std::vector<int> weights = std::vector<int>();
};

class Specimen
{
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

class SATSolver
{
public:
    SATSolver(std::vector<int> weights, std::vector<std::vector<int>> clauses);
    int Solve(bool print = true);

private:
    int variablesCount;
    int clausesCount;
    int pop_size = 1000;
    int generation = 100;
    std::shared_ptr<SAT> satProblem;
    std::vector<Specimen> population = std::vector<Specimen>();
    void Crossbreeding();
    void Mutation(int probability = 100);
    void MassExtenction();
};

Clause::Clause(std::vector<int> clause)
{
    variables = clause;
}

bool Clause::Fulfilled(std::vector<bool> genom)
{
    bool res = false;
    for (auto &var : variables)
    {
        if (var < 0)
        {
            res = res || !genom[std::abs(var) - 1];
        }
        else
        {
            res = res || genom[var - 1];
        }
    }
    return res;
}

int Clause::ClauseWeight(std::vector<bool> genom, std::vector<int> weights)
{
    int res = 0;
    for (auto &var : variables)
    {
        if (genom[std::abs(var) - 1])
        {
            res += weights[std::abs(var) - 1];
        }
    }
    return res;
}

SAT::SAT(std::vector<int> weights, std::vector<std::vector<int>> clauses)
{
    this->weights = weights;
    for (auto &clause : clauses)
    {
        this->clauses.push_back(Clause(clause));
    }
}

SAT::~SAT()
{
}

double SAT::CalculateWeight()
{
    double res = 0;
    for (int i = 0; i < weights.size(); i++)
    {
        res += weights[i];
    }
    return res;
}

double SAT::CalculateFulfilled(std::vector<bool> genom)
{
    double res = 0;
    int i = 0;
    for (auto &clause : clauses)
    {
        if (clause.Fulfilled(genom))
        {
            res++;
        }
    }
    return res;
}

bool SAT::Fullfiled(std::vector<bool> genom)
{
    for (auto &clause : clauses)
    {
        if (!clause.Fulfilled(genom))
        {
            return false;
        }
    }
    return true;
}

double SAT::CalculateClausesWeight(std::vector<bool> genom)
{
    double res = 0;
    for (int i = 0; i < weights.size(); i++)
    {
        if (genom[i])
        {
            res += weights[i];
        }
    }
    return res;
}

double SAT::ClausesSize()
{
    return (double)clauses.size();
}

Specimen::Specimen(std::shared_ptr<SAT> sat, int numberOfVariables) : sat(sat)
{
    for (int i = 0; i < numberOfVariables; i++)
    { // Random genom initialization
        genom.push_back(rand() % 2);
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
    int random = (rand() % (int)genom.size());
    genom[random] = !genom[random];
}

void Specimen::Fitness()
{
    if (sat->CalculateFulfilled(genom))
    {
        fitness = sat->CalculateClausesWeight(genom);
    }
    else
    {
        fitness = 0;
    }
}

void Specimen::Crossbreed(Specimen specimen, int position)
{
    std::vector<bool> secondGenom = specimen.GetGenom();
    for (int i = position; i < genom.size(); i++)
    {
        genom[i] = secondGenom[i];
    }
}

std::vector<bool> Specimen::GetGenom()
{
    return genom;
}

SATSolver::SATSolver(std::vector<int> weights, std::vector<std::vector<int>> clauses)
{
    variablesCount = weights.size();
    clausesCount = clauses.size();
    satProblem = std::make_shared<SAT>(weights, clauses);
    for (int i = 0; i < pop_size; i++)
    {
        population.push_back(Specimen(satProblem, variablesCount));
    }
}

int SATSolver::Solve(bool print)
{
    std::cout << std::fixed << std::setprecision(15);
    for (int i = 0; i < generation; i++)
    {
        for (auto &specimen : population)
        {
            specimen.Fitness();
        }
        std::sort(population.begin(), population.end(), [](const Specimen &a, const Specimen &b) {
            return a.fitness > b.fitness;
        });
        MassExtenction(); //check mass extenction, then execute it
        //max - min - med - weight
        if (print)
        {
            std::cout << i << " " << population[0].fitness << " " << population[pop_size - 1].fitness << " " << ((population[(pop_size / 2) - 1].fitness + population[pop_size / 2].fitness) / 2.0)
                      << " " << satProblem->CalculateClausesWeight(population[0].GetGenom()) << std::endl;
        }
        Crossbreeding();
        Mutation();
    }
    for (auto &specimen : population)
    {
        specimen.Fitness();
    }
    std::sort(population.begin(), population.end(), [](const Specimen &a, const Specimen &b) {
        return a.fitness > b.fitness;
    });
    //max - min - med - weight
    if (print)
    {
        std::cout << generation << " " << population[0].fitness << " " << population[pop_size - 1].fitness << " " << ((population[(pop_size / 2) - 1].fitness + population[pop_size / 2].fitness) / 2.0)
                  << " " << satProblem->CalculateClausesWeight(population[0].GetGenom()) << std::endl;
    }

    return satProblem->CalculateClausesWeight(population[0].GetGenom()); //return best weight
}

void SATSolver::Crossbreeding()
{
    std::vector<Specimen> theBestOutOfBest = std::vector<Specimen>(); //dont ask just accept it
    while (theBestOutOfBest.size() < pop_size)
    {
        std::vector<Specimen> braveWarriors = std::vector<Specimen>();
        for (int i = 0; i < 50; i++)
        {
            int poorBoi = std::rand() % (pop_size);
            braveWarriors.push_back(population[poorBoi]);
        }
        std::sort(braveWarriors.begin(), braveWarriors.end(), [](const Specimen &a, const Specimen &b) {
            return a.fitness > b.fitness;
        });

        double prob = 1; //percentual probability for selecting the best one
        int i = 0;
        for (auto &specimen : braveWarriors)
        {
            double resProb = double(std::rand() % 100) / 100.0;
            if (i == 0 && resProb <= prob)
            {
                theBestOutOfBest.push_back(specimen);
                prob = prob * (1 - prob);
                i++;
                break;
            }
            else if (resProb <= prob)
            {
                theBestOutOfBest.push_back(specimen);
                prob = 0.65 * std::pow(0.35, i);
                i++;
                break;
            }
        }
    }

    std::vector<Specimen> tmp = std::vector<Specimen>{population};
    std::sort(tmp.begin(), tmp.end(), [](const Specimen &a, const Specimen &b) {
        return a.fitness > b.fitness;
    });
    // Elitism
    population = std::vector<Specimen>{theBestOutOfBest};
    population[0] = tmp[0];
    population[1] = tmp[1];

    for (int i = 2; i < pop_size; i = i + 2)
    {
        int cut = (rand() % variablesCount);
        int probability = rand() % 100;
        if (probability <= 95)
        {
            auto tmp = Specimen(population[i]);
            population[i].Crossbreed(population[i + 1], cut);
            population[i + 1].Crossbreed(tmp, cut);
        }
    }
}

void SATSolver::Mutation(int probability)
{
    for (int i = 2; i < pop_size * 5; i++)
    {
        int sel = (rand() % 1000);
        if (sel > probability)
        {
            continue;
        }
        population[i % pop_size].Mutate();
    }
}

void SATSolver::MassExtenction()
{
    //If median is same as max then fire mass extenction
    if (((population[(pop_size / 2) - 1].fitness + population[pop_size / 2].fitness) / 2.0) == population[0].fitness)
    {
        auto survivors = std::vector<Specimen>();
        for (int i = 0; i < 10; i++)
        { //fill survivors, only select first 10
            survivors.push_back(population[i]);
        }
        for (int i = 10; i < pop_size; i++)
        {
            survivors.push_back(Specimen(satProblem, variablesCount));
        }
        population = std::vector<Specimen>(survivors);
        for (int i = 0; i < 100; i++)
        {
            Mutation(); //violent mutation
        }
    }
}

void initiate_algorithm(std::string filename, int calculatedResult)
{
    //std::chrono::duration<double> sum_time = std::chrono::duration<double>(0);
    //for (int e = 0; e < 100; e++) {
    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error("File could not be open.");
    }
    else
    {
        //now we can read and parse file
        std::string line;
        int count_variables, count_clauses;
        std::vector<int> weights = std::vector<int>();
        std::vector<std::vector<int>> clauses = std::vector<std::vector<int>>();
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            char firstChar;
            std::string type;
            if (line[0] == 'c')
            {
                continue;
            }
            else if (line[0] == 'p')
            {
                iss >> firstChar;
                iss >> type >> count_variables >> count_clauses;
            }
            else if (line[0] == 'w')
            {
                iss >> firstChar;
                for (int i = 0; i < count_variables; i++)
                { //read all weights∂
                    int weight;
                    iss >> weight;
                    weights.push_back(weight);
                }
            }
            else if (line[0] == '%')
            {
                break;
            }
            else
            {
                int a, b, c;
                iss >> a >> b >> c;
                std::vector<int> clause = {a, b, c};
                clauses.push_back(clause);
            }
        }

        auto satSolver = SATSolver(weights, clauses);
        // auto start = std::chrono::steady_clock::now();
        if (calculatedResult == 0)
        {
            satSolver.Solve();
        }
        else
        {
            auto result = satSolver.Solve(true);
            std::cout << (double)(std::abs((double)(calculatedResult - result) / (double)calculatedResult) * 100) << std::endl;
        }
        //auto end = std::chrono::steady_clock::now();
        //sum_time += (end-start);
    }
    //}
    //std::cout << "time " << (double)(sum_time.count()/100.0) << std::endl;
}

int main(int argc, char **argv)
{
    std::srand(std::time(0));
    if (argc != 3)
    {
        std::cerr << "You have to provide a file to analyze." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int result = std::stoi(argv[2]);
    initiate_algorithm(filename, result);
    return 0;
}