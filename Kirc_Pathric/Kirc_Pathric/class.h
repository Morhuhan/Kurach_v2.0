#pragma once
#pragma once
#include <iostream>
#include <stack>
#include <algorithm>
#include <cmath>
#include <fstream>
using namespace std;


class point {

public:
    double x, y;

    point() {
        x = 0;
        y = 0;
    };

    point(double _x, double _y) {
        x = _x;
        y = _y;
    }

    point operator-(const point& other) const {
        return point(x - other.x, y - other.y);
    }

    point operator+(const point& other) const {
        return point(x + other.x, y + other.y);
    }

    point operator*(const double k) const {
        return point(x * k, y * k);
    }

    double operator^(const point& other) const {
        return x * other.y - y * other.x;
    }

    bool operator==(const point& other) const {
        if (x == other.x && y == other.y) return true;
        else { return false; }
    }

};

class ConvexHull {

private:

    int _size; // Размер массива точек
    point* _points; // Указатель на массив точек

public:

    // Конструктор, принимающий массив точек и его размер
    ConvexHull(point* points, int size) {
        _size = size;
        _points = new point[size];
        for (int i = 0; i < size; i++) {
            _points[i] = point(points[i].x, points[i].y);
        }
    }

    // Конструктор, принимающий размер оболочки
    ConvexHull(int size) {
        _size = size;
        _points = new point[size];
    }

    // Конструктор по умолчанию
    ConvexHull() {
        _size = 0;
        _points = nullptr;
    }

    // Конструктор копии
    ConvexHull(const ConvexHull& other) {
        _size = other._size;
        _points = new point[_size];
        for (int i = 0; i < _size; i++) {
            _points[i] = other._points[i];
        }
    }

    // Деструктор
    ~ConvexHull() {
        delete _points;
    }

    // Метод для получения массива точек
    point* getPoints() const {
        return _points;
    }

    // Метод для получения размера массива точек
    int getSize() const {
        return _size;
    }

    int indexOf(point p) {
        for (int i = 0; i < _size; i++) {
            if (_points[i] == p) return i;
        }
    }

    void print_hull() {
        for (int i = 0; i < _size; i++) {
            printf("%f %f\n", _points[i].x, _points[i].y);
        }
    }

    // Метод для сохранения координат точек в файл
    void saveToFile(const char* filename) {
        ofstream file(filename);
        if (file.is_open()) {
            for (int i = 0; i < _size; i++) {
                file << _points[i].x << ";" << _points[i].y << "\n";
            }
            file.close();
            cout << "Points was written in the file " << filename << std::endl;
        }
        else {
            cout << "File wasn't find " << filename << std::endl;
        }
    }

    // Перегрузка оператора присваивания
    ConvexHull& operator=(const ConvexHull& other) {
        if (this == &other) {
            return *this;  // Проверка на самоприсваивание
        }

        delete[] _points;  // Освобождение ресурсов текущего объекта

        _size = other._size;
        _points = new point[_size];
        for (int i = 0; i < _size; i++) {
            _points[i] = other._points[i];
        }

        return *this;
    }

};


class point_stack {
private:
    static const int MAX_SIZE = 100;  // Максимальный размер стека
    point* data;                      // Динамический массив
    int topIndex;                     // Индекс верхнего элемента

public:
    point_stack() {
        data = new point[MAX_SIZE];
        topIndex = -1;
    }

    ~point_stack() {
        delete[] data;
    }

    void push(const point& point) {
        if (topIndex == MAX_SIZE - 1) {
            std::cerr << "Error: Stack is full." << std::endl;
            exit(1);
        }

        data[++topIndex] = point;
    }

    size_t size() const {
        return topIndex + 1;
    }

    point pop() {
        if (topIndex == -1) {
            std::cerr << "Error: Stack is empty." << std::endl;
            exit(1);
        }

        return data[topIndex--];
    }

    point top() const {
        if (topIndex == -1) {
            std::cerr << "Error: Stack is empty." << std::endl;
            exit(1);
        }

        return data[topIndex];
    }
};