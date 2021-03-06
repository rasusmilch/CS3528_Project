#include <iostream>
#include <iomanip>
//#include "Client.h"
//#include "../Package.h"
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
//#include "../Utils.h"
#include <random>
#include <algorithm>
#include <vector>

using namespace std;

// Seed nice random number generator.
std::mt19937 rng(std::random_device{}());

// Structure for holding random generation parameters
typedef struct {

    // Filename to open
    string fileName;

    // Population of names to draw from.
    // 10 gives 10 first names and 10 last names, 10*10 = 100 combinations.
    unsigned int population;

    // Number of random packages
    unsigned int num;

    // Maximum building address
    int maxAddress;

    // Maximum streets each direction (20 = 20th St/Ave is furthest out.)
    unsigned int maxStreets;

    // Max random weight
    float maxWeight;

    // Priority weights of REG, TWO, OVER.
    // One can place positive probabilities or weights.
    // [0.1, 0.4, 0.5] would be 10% regular, 40% two-day and 50% overnights.
    // [1, 4, 5] would give the same percentages
    // [10, 40, 50] should as well.
    float priority[3];

    // Clustering creation.
    // Number of clusters (min[0], max[1]), package num in cluster (min [2], max[3]),
    // Radius [4] and standard deviation [5] in building addresses
    int cluster[6];

} struct_randomPackage;

string parseStreet(unsigned int street, unsigned int addressNum) {
    // Take address and street number, and create a random string address.

    // Coin flip
    std::uniform_int_distribution<int> coinFlip(0, 1);

    // Cardinal direction
    std::uniform_int_distribution<int> cardinalUniform(0, 3);

    string address = "";

    // Toss up if road is street or avenue
    unsigned int randomNum = coinFlip(rng);


    // What appropriate street name do we have?
    if (street == 0) {
        // No 0 street or avenue, choose main or central
        if (randomNum == 0) {
            address += "Main St ";

            // North or South?
            if (coinFlip(rng) == 0) {
                address += "N";
            } else {
                address += "S";
            }
        } else {
            address += "Central Ave ";
            // East or West?
            if (coinFlip(rng) == 0) {
                address += "W";
            } else {
                address += "E";
            }
        }

        // Add appropriate suffix
    } else if (street == 1) {

        address += std::to_string(street) + "st ";
    } else if (street == 2) {

        address += std::to_string(street) + "nd ";
    } else if (street == 3) {

        address += std::to_string(street) + "rd ";
    } else {

        address += std::to_string(street) + "th ";
    }

    // If we did not have a main or central
    if (street != 0) {
        // Choose street or avenue
        if (randomNum == 0) {
            address += "Street ";
        } else {
            address += "Avenue ";
        }

        // Since we did not pick main street or central avenue,
        // We need to choose our quadrant
        randomNum = cardinalUniform(rng);

        if (randomNum == 0) {
            address += "NW";
        } else if (randomNum == 1) {
            address += "NE";
        } else if (randomNum == 2) {
            address += "SE";
        } else if (randomNum == 3) {
            address += "SW";
        }
    }

    return address;
}


string streetAddressUniform(const struct_randomPackage randomConsts) {
    // Creates a uniformly distributed address string

    // Street number
    unsigned int street;

    // Uniform distribution of integers for streets
    std::uniform_int_distribution<int> streetsUniform(0, randomConsts.maxStreets);

    // Uniform distribution of integers for address numbers
    std::uniform_int_distribution<int> addressUniform(1, randomConsts.maxAddress - 1);

    string address = "";

    // Random building number
    int addressNum = static_cast<int>(addressUniform(rng));

    address += std::to_string(addressNum) + " ";

    // Random street number
    street = streetsUniform(rng);

    address += parseStreet(street, addressNum);
    // No variation for this simulation
    address += ",Bemidji,MN,56601";

    return address;
}

vector<string > streetAddressCluster(const struct_randomPackage randomConsts) {
    // Number of clusters (min[0], max[1]), package num in cluster (min [2], max[3]),
    // Radius [4] and standard deviation [5] in building addresses

    // Holds random ints for address creation
    int addressNum;

    int clusterAddress = 0;
    int clusterStreet = 0;

    // Street number
    int street;

    // Uniform distribution of integers for streets
    std::uniform_int_distribution<int> streetsUniform(0, randomConsts.maxStreets);

    // Uniform distribution of integers for address numbers
    std::uniform_int_distribution<int> addressUniform(1, randomConsts.maxAddress);

    // Number of clusters to create
    std::uniform_int_distribution<int> numClusterUniform(randomConsts.cluster[0], randomConsts.cluster[1]);

    // Number of packages to create in cluster
    std::uniform_int_distribution<int> numClusterPackages(randomConsts.cluster[2], randomConsts.cluster[3]);

    // Uniform distribution around cluster point.
    std::normal_distribution<double> clusterNormal(randomConsts.cluster[4],randomConsts.cluster[5]);

    vector<string> returnAddresses;
    string tempAddress;

    for (int cluster = 0; cluster < numClusterUniform(rng); ++cluster) {

        // Get the clusters street center
        clusterStreet = abs(streetsUniform(rng));

        clusterAddress = addressUniform(rng);

        for (int individualPackage = 0; individualPackage < numClusterPackages(rng); ++individualPackage) {

            tempAddress = "";

            // Random building number
            addressNum = static_cast<int>(clusterNormal(rng)) + clusterAddress;

            tempAddress += std::to_string(addressNum) + " ";

            // Random street number
            street = static_cast<int>(clusterNormal(rng))/100 + clusterStreet;

            tempAddress += parseStreet(street, addressNum);
            // No variation for this simulation
            tempAddress += ",Bemidji,MN,56601";

            returnAddresses.push_back(tempAddress);
        }
    }

    return returnAddresses;
}

void randomPackages(const struct_randomPackage randomConsts) {
    // Holds our names from files
    std::vector<string> firstNames;
    std::vector<string> lastNames;
    std::vector<string> clusterAddresses;


    // Output file stream
    ofstream file;

    // Input file stream
    ifstream names;

    // Holds lines and names from files
    string line;
    string name;
    string sender, receiver;

    double weight;

    // Buffer to extract names from files.
    stringstream buffer;

    // Round any odd populations down
    unsigned int halfPop = randomConsts.population / 2;

    // Uniform distribution of integers for streets
    std::uniform_int_distribution<int> streetsUniform(0, randomConsts.maxStreets);

    // Uniform distribution of integers for address numbers
    std::uniform_int_distribution<int> addressUniform(1, randomConsts.maxAddress);

    // Coin flip
    std::uniform_int_distribution<int> coinFlip(0, 1);

    // Cardinal direction
    std::uniform_int_distribution<int> cardinalUniform(0, 3);


    // Normally distributed package weight around the center of maximum weight. Standard deviation is 1/3 of that half point.
    std::normal_distribution<double> weightNormal(randomConsts.maxWeight / 2, randomConsts.maxWeight / 6);

    // Holds random floats for probabilities and weights
    float randomFloat;

    // Accumulated P or weights for priority package generation.
    float accumulated_P = 0;


    // Sum all weights or probabilities
    for (unsigned int i = 0; i < 3; i++) {
        accumulated_P += randomConsts.priority[i];
    }

    std::uniform_real_distribution<double> priorityUniform(0, accumulated_P);

    // Open file
    names.open("dist.all.last");

    // Extract up to population last names
    for(unsigned int i = 0; i < halfPop * 2; i++) {
        getline(names, line);
        buffer.str(line);
        buffer >> name;
        lastNames.push_back(name);
    }

    names.close();

    names.open("dist.female.first");

    // Extract 50% female first names
    for(unsigned int i = 0; i < halfPop; i++) {
        getline(names, line);
        buffer.str(line);
        buffer >> name;
        firstNames.push_back(name);
    }

    names.close();

    names.open("dist.male.first");

    // Extract 50% male first names.
    for(unsigned int i = 0; i < halfPop; i++) {
        getline(names, line);
        buffer.str(line);
        buffer >> name;
        firstNames.push_back(name);
    }

    names.close();

    // Uniform integer distribution for names.
    std::uniform_int_distribution<int> nameUniform(0, lastNames.size() - 1);

    // Get cluster addresses
    clusterAddresses = streetAddressCluster(randomConsts);

    // Start on creating random packages
    // Open file
    file.open(randomConsts.fileName);

    // Need to create clusters of packages
    for (auto iter = clusterAddresses.begin(); iter != clusterAddresses.end(); ++iter) {
        // Need a sender and receiver
        sender = streetAddressUniform(randomConsts);

        // Grab receiver.
        receiver = (*iter);

        file << firstNames[nameUniform(rng)] << "," << lastNames[nameUniform(rng)] << "," << sender << "," << firstNames[nameUniform(rng)] << "," << lastNames[nameUniform(rng)] << "," << receiver << ",";
        // Weight of package. Apply tenth ounce truncation by multiplying
        //unsigned int weight = weightNormal;

        weight = weightNormal(rng);

        while (weight < 0) {
            weight = weightNormal(rng);
        }

        file << weight << ',';

        // Choose a random float up to accumulated_P
        randomFloat = priorityUniform(rng);

        // Start at index 0
        unsigned int index = 0;

        // Subtract priority weights from random until we find one that is less than our random number.
        while(index < 3) {
            if (randomFloat < randomConsts.priority[index]) {
                break;
            } else {
                randomFloat -= randomConsts.priority[index];
            }

            // Not found yet, move on.
            index++;
        }

        // Found our priority, since it is an enum, just output integer (moved enum up by 1 to avoid divide by zero).
        // Additionally, all packages start life on age = 1.
        file << index + 1 << ",1" << std::endl;

    }


    // Loop for number of desired packages
    if (randomConsts.num > clusterAddresses.size()) {
        for (unsigned int i = 0; i < randomConsts.num - clusterAddresses.size(); ++i) {
            // Need a sender and receiver
            sender = streetAddressUniform(randomConsts);
            receiver = streetAddressUniform(randomConsts);

            file << firstNames[nameUniform(rng)] << "," << lastNames[nameUniform(rng)] << "," << sender << "," << firstNames[nameUniform(rng)] << "," << lastNames[nameUniform(rng)] << "," << receiver << ",";

            // Weight of package. Apply tenth ounce truncation by multiplying
            //unsigned int weight = weightNormal;
            weight = weightNormal(rng);

            while (weight < 0) {
                weight = weightNormal(rng);
            }

            file << weight << ',';

            // Start Russian roulette selection.
            // Choose a random float up to accumulated_P
            randomFloat = priorityUniform(rng);

            // Start at index 0
            unsigned int index = 0;

            // Subtract priority weights from random until we find one that is less than our random number.
            while(index < 3) {
                if (randomFloat < randomConsts.priority[index]) {
                    break;
                } else {
                    randomFloat -= randomConsts.priority[index];
                }

                // Not found yet, move on.
                index++;
            }

            // Found our priority, since it is an enum, just output integer (moved enum up by 1 to avoid divide by zero).
            // Additionally, all packages start life on age = 1.
            file << index + 1 << ",1" << std::endl;

        }
    }
    file.close();

}

void printHelp(char *argv[]) {
    // Program help usage

    std::cout << "Usage: " << argv[0] << " filename [options]" << std::endl << std::endl;
    std::cout << "Options" << std::endl;
    std::cout << "=======" << std::endl;
    std::cout << "filename           CSV file to write packages." << std::endl;
    std::cout << "-pop #             Population size of names to use." << std::endl;
    std::cout << "-n #               Number of packages to generate" << std::endl;
    std::cout << "-st #              Max number of streets in one direction (eg, 30 gives 30 streets North and South of Main St MAX)" << std::endl;
    std::cout << "-addr #            Max building number (eg, 2000 gives max building address of 2000)" << std::endl;
    std::cout << "-w #               Max weight of package to generate (normal distribution around max/2 with SD max/6)" << std::endl;

    std::cout << std::endl << "Priority Weights" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Priority Weights take an integer value for each priority (overnight, two-day, regular)." << std::endl;
    std::cout << "The values are added together and the relative weight of each is randomly generated" << std::endl;
    std::cout << "for the output file." << std::endl;
    std::cout << std::endl << "Ex: -o 10 -t 30 -r 60" << std::endl;
    std::cout << "These would add to 100." << std::endl;
    std::cout << "Overnight priority would be 10/100 = 10% chance" << std::endl;
    std::cout << "Two-day priority would be 30/100 = 30% chance" << std::endl;
    std::cout << "Regular priority would be 60/100 = 60% chance" << std::endl;
    std::cout << std::endl << "-o #               Overnight priority weight." << std::endl;
    std::cout << "-t #               Two-day priority weight" << std::endl;
    std::cout << "-r #               Regular priority weight" << std::endl;
    std::cout << std::endl << "Clusters" << std::endl;
    std::cout << "--------" << std::endl;
    std::cout << "Creates clusters of packages based on a normal distribution curve." << std::endl;
    std::cout << "Enter min/max number of clusters, with the size of each cluster." << std::endl;
    std::cout << "The program will randomly select the number of clusters, n, and create" << std::endl;
    std::cout << "n clusters with a random amount of packages between pmin and pmax for each cluster." << std::endl;
    std::cout << "The clusters will have a radius around the mean with specified standard deviation." << std::endl;
    std::cout << std::endl << "Ex: -cmin 2 -cmax 5 -pmin 10 -pmax 20 -cr 600 -sd 200" << std::endl;
    std::cout << "Creates 2-5 clusters each containing 10-20 packages with a mean address of 600 and SD of 200," << std::endl;
    std::cout << "where 68% of addresses would lie 400-800 (+/- 200 SD), 95% are 200-1000 and 99.7% from 0-1200." << std::endl;
    std::cout << std::endl << "-cmin #            Minimum number of clusters." << std::endl;
    std::cout << "-cmax #            Maximum number of clusters." << std::endl;
    std::cout << "-pmin #            Minimum number of packages within a cluster." << std::endl;
    std::cout << "-pmax #            Maximum number of packages within a cluster." << std::endl;
    std::cout << "-cr #              Radius of cluster as a normal distribution mean in street address units." << std::endl;
    std::cout << "-sd #              Standard deviation of cluster in street address units." << std::endl;
}

int main(int argc, char *argv[]) {

    // Default indicators for random generator
    int population = 0, numberPackages = 0, maxAddress = 0, maxStreet = 0;
    float pOver = 0, pTwo = 0, pReg = 0, maxWeight = 0;
    int clusterNMin = 0, clusterNMax = 0, clusterPackMin = 0, clusterPackMax = 0, radius = 0, SD = 0;

    // Test if file exists
    ofstream file;
    string outputFile;

    // Do we have enough arguments?
    if (argc == 1) {
        // No
        printHelp(argv);
        return 1;
    } else {
        // Output filename string converter
        istringstream outputFileSS(argv[1]);

        try {
            // Test if we can extract a string
            if (!(outputFileSS>>outputFile)) {
                // Nope
                std::cout << "Error parsing filename to string" << std::endl;
                printHelp(argv);
                return 1;
            } else if (outputFile == "-help" || outputFile == "--help" || outputFile == "-h" || outputFile == "--h") {
                // Test if second argument was not the file, but a cry for help!
                printHelp(argv);
                // Help is acceptable?
                return 0;
            }

            // Try opening file
            file.open(outputFile);

            // Test if successful
            if (!file) {
                // Nope
                std::cout << "File " << outputFile << " cannot be opened!" << std::endl;
                return 1;
            }

            // Close test
            file.close();

        }

        catch (exception& e) {
            std::cout << "Invalid file parameter: " << argv[2] << std::endl;
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }

        // Parse the rest of the options.
        for (int i = 2; i < argc; ++i) {
            istringstream cmdSwitch(argv[i]);
            string strSwitch;

            if (!(cmdSwitch >> strSwitch)) {
                std::cout << "Bad switch: " << argv[i] << std::endl;
                return 1;
            }

            if (strSwitch == "-pop") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> population)) {
                        std::cout << "Invalid number for population: " << argv[i+1] << std::endl;
                        return 1;
                    }
                }
                catch (exception& e) {
                    std::cout << "Invalid population parameter: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-n") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> numberPackages)) {
                        std::cout << "Invalid number for maximum number of packages: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid max packages parameter: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-st") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> maxStreet)) {
                        std::cout << "Invalid number for max streets: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for max streets: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-addr") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> maxAddress)) {
                        std::cout << "Invalid number for max addresses: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for max addresses: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-w") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> maxWeight)) {
                        std::cout << "Invalid number for max weight: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for max weight: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-o") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> pOver)) {
                        std::cout << "Invalid number for overnight priority weight: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for overnight priority weight: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-t") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> pTwo)) {
                        std::cout << "Invalid number for two-day priority weight: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for two-day priority weight: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-r") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> pReg)) {
                        std::cout << "Invalid number for regular priority weight: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for regular priority weight: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-cmin") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> clusterNMin)) {
                        std::cout << "Invalid number for minimum number of clusters: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for minimum number of clusters: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-cmax") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> clusterNMax)) {
                        std::cout << "Invalid number for maximum number of clusters: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for maximum number of clusters: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-pmin") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> clusterPackMin)) {
                        std::cout << "Invalid number for minimum number of packages per cluster: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for minimum number of packages per cluster: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-pmax") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> clusterPackMax)) {
                        std::cout << "Invalid number for maximum number of packages per cluster: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for maximum number of packages per cluster: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-cr") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> radius)) {
                        std::cout << "Invalid number for cluster radius (mean): " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for cluster radius (mean): " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            } else if (strSwitch == "-sd") {
                try {
                    istringstream ss(argv[i+1]);
                    if (!(ss >> SD)) {
                        std::cout << "Invalid number for cluster standard deviation: " << argv[i+1] << std::endl;
                    }

                }
                catch (exception& e) {
                    std::cout << "Invalid number for cluster standard deviation: " << argv[i + 1] << std::endl;
                    std::cout << "Exception: " << e.what() << std::endl;
                    return 1;
                }
            }

        }
    }

    // Check for sane option values!
    if (population <= 0) {
        std::cout << std::endl << "*** Invalid population! ***" << std::endl;
        population = 100;
    }

    if (numberPackages <= 0) {
        std::cout << std::endl << "*** Invalid number of packages! ***" << std::endl;
        numberPackages = 50;
    }

    if (maxAddress <= 0) {
        std::cout << std::endl << "*** Invalid maximum address! ***" << std::endl;
        maxAddress = 2000;
    }

    if (maxStreet <= 0) {
        std::cout << std::endl << "*** Invalid maximum street! ***" << std::endl;
        maxStreet = 20;
    }

    if (maxWeight <= 0) {
        std::cout << std::endl << "*** Invalid maximum weight! ***" << std::endl;
        maxWeight = 16*10;
    }

    if (pOver <= 0) {
        std::cout << std::endl << "*** Invalid overnight priority! ***" << std::endl;
        pOver = 1;
    }

    if (pTwo <= 0) {
        std::cout << std::endl << "*** Invalid two-day priority! ***" << std::endl;
        pTwo = 3;
    }

    if (pReg <= 0) {
        std::cout << std::endl << "*** Invalid overnight priority! ***" << std::endl;
        pReg = 6;
    }

    if (clusterNMin <= 0 || clusterNMin > clusterNMax) {
        std::cout << std::endl << "*** Invalid minimum clusters! ***" << std::endl;
        clusterNMin = 0;
    }

    if (clusterNMax <= 0 || clusterNMin > clusterNMax) {
        std::cout << std::endl << "*** Invalid maximum clusters! ***" << std::endl;
        clusterNMax = 1;
    }

    if (clusterPackMin <= 0 || clusterPackMin > clusterPackMax) {
        std::cout << std::endl << "*** Invalid minimum package count for cluster! ***" << std::endl;
        clusterPackMin = 1;
    }

    if (clusterPackMax <= 0 || clusterPackMin > clusterPackMax) {
        std::cout << std::endl << "*** Invalid maximum package count for cluster! ***" << std::endl;
        clusterPackMax = 1;
    }

    if (radius <= 100) {
        std::cout << std::endl << "*** Invalid cluster radius/mean (100)! ***" << std::endl;
        radius = 100;
    }

    if (SD <= 0) {
        std::cout << std::endl << "*** Invalid cluster standard deviation! ***" << std::endl;
        SD = 100;
    }

    // *Filename to open
    // *Population of names to draw from.
    // 10 gives 10 first names and 10 last names, 10*10 = 100 combinations.
    // *Number of random packages
    // Maximum building address
    // Maximum streets each direction (20 = 20th St/Ave is furthest out.)
    // Max random weight
    // Priority weights of REG, TWO, OVER.
    // One can place positive probabilities or weights.
    // [0.1, 0.4, 0.5] would be 10% regular, 40% two-day and 50% overnights.
    // [1, 4, 5] would give the same percentages
    // [10, 40, 50] should as well.
    // Clustering creation.
    // Number of clusters (min[0], max[1]), package num in cluster (min [2], max[3]),
    // Radius [4] and standard deviation [5] in building addresses

    // Create structure for passing to generator
    struct_randomPackage generatePackages = {outputFile, static_cast<unsigned int>(population), static_cast<unsigned int>(numberPackages), maxAddress, static_cast<unsigned int>(maxStreet), maxWeight, {pReg, pTwo, pOver}, {clusterNMin,clusterNMax,clusterPackMin,clusterPackMax, radius, SD}};

    // Not guaranteed name unique yet (eg, may send package to same name, but with different address with small population due to small random space.)
    randomPackages(generatePackages);

    return 0;
}
