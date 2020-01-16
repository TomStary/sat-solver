#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Clause {
    
};

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
                std::cout << count_variables << " " << count_clauses << std::endl;
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

        std::cout << "Readback: " << count_variables << " " << count_clauses << std::endl;
        std::cout << weights.size() << std::endl;
        for (auto &val: weights) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        for (auto &clause: clauses) {
            for (auto &variable: clause) {
                std::cout << variable << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cerr << "You have to provide a file to analyze." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    initiate_algorithm(filename);
    return 0;
}