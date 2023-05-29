#include <iostream>
#include <stack>
#include <cmath>
#include <fstream>
#include <ctime>
#include <chrono>

#include "class.h"
#include "support.h"
#include "Kirkpathric.h"

using namespace std;

int main() {
    // »нициализируем генератор случайных чисел
    srand(time(NULL));

    int size_points;

    std::cout << "Enter '1' to generate random points or '2' to read points from a file: ";
    int choice;
    std::cin >> choice;
    std::chrono::high_resolution_clock::time_point startTime;

    if (choice == 1) {
        std::cout << "Enter the size of the points array: ";
        std::cin >> size_points;
        point* points = new point[size_points];
        generateRandomPoints(size_points, points);
        startTime = std::chrono::high_resolution_clock::now();
        ConvexHull hull = create_hull(points, size_points);
        hull.saveToFile("hull.csv");
        delete[] points;
    }
    else if (choice == 2) {
        std::string filename;
        std::cout << "Enter the filename: ";
        std::cin >> filename;
        int fileLines = countFileLines(filename);
        if (fileLines > 0) {
            point* points = new point[fileLines];
            readPointsFromFile(filename, fileLines, points);
            startTime = std::chrono::high_resolution_clock::now();
            ConvexHull hull = create_hull(points, fileLines);
            hull.saveToFile("hull.csv");
            delete[] points;
        }
        else {
            std::cout << "Invalid lines count" << std::endl;
            return 0;
        }
    }
    else {
        std::cout << "Invalid choice" << std::endl;
        return 0;
    }

    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    std::cout << "Algorithm execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}