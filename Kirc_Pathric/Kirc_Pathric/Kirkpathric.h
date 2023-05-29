#pragma once
#include <iostream>
#include <stack>
#include <cmath>
#include <fstream>

#include "class.h"
#include "support.h"

#define N 6 // минимальное количество точек для вызова метода Грэхема

using namespace std;

ConvexHull graham_alg(point* points, int size) {

    // Find the bottom-most point
    int min_idx = 0;

    for (int i = 0; i < size; i++) {
        if (points[i].y < points[min_idx].y) {
            min_idx = i;
        }
        else if (points[i].y == points[min_idx].y && points[i].x < points[min_idx].x) {
            min_idx = i;
        }
    }

    // Swap bottom-most point with the first point
    swap(points[0], points[min_idx]);

    // Sort points by polar angle
    sort_by_polar_angle(points, size, points[0]);

    // Create a stack to store the points of the convex stack
    point_stack stack;

    // Push the first three points onto the stack
    stack.push(points[0]);
    stack.push(points[1]);
    stack.push(points[2]);

    // Iterate over the remaining points
    for (int i = 3; i < size; i++) {
        // Pop points from the stack while the current point is to the right of the top two points
        while (stack.size() >= 2) {

            // Сохраняем верхнюю точку стека
            point top = stack.top();

            // Удаляем верхнюю точку стека
            stack.pop();

            // Берем следующую точку
            point second = stack.top();

            // Если это условие выполняется, точка отправляется обратно в стек
            if (crossProduct(second, top, points[i]) > 0) {
                stack.push(top);
                break;
            }
        }
        stack.push(points[i]);
    }

    // Create a new array to store the points of the convex stack
    int stack_size = stack.size();
    point* stack_points = new point[stack_size];

    // Pop the points from the stack and store them in the array
    for (int i = stack_size - 1; i >= 0; i--) {
        stack_points[i] = stack.top();
        stack.pop();
    }

    ConvexHull new_hull(stack_points, stack_size);

    return new_hull;
}

// Получает: 
// оболочку hull, которую нужно разбить на 2 цепи
// точки rtan, ltan которые разбивают hull
// ссылки на массивы точек chain1 и chain2, на которые нужно разбить hull
// В процессе работы вычисляет размеры массивов chain1 и chain2 и заполняет их соответствующими точками. 
void split_hull(ConvexHull& hull, const point& rtan, const point& ltan, point*& chain1, point*& chain2) {

    int index_rtan = hull.indexOf(rtan);
    int index_ltan = hull.indexOf(ltan);
    int pointer = index_ltan;
    int i;
    int j;
    int flag = 0;
    int size_chain1 = 0;
    int size_chain2 = 0;

    // Цепь chain 1 включает точки разбиения;
    // Если ltan < rtan разбиваем так, что chain1 включает точки от ltan до rtan ВКЛЮЧИТЕЛЬНО, chain2 содержит все остальные точки
    // Если rtan < ltan разбиваем так, что chain1 содержит точки от rtan до ltan ВКЛЮЧИТЕЛЬНО, chain2 содержит все остальные точки

    // Первый случай
    if (index_ltan < index_rtan) {
        pointer = 0;
        j = 0;
        i = 0;
        pointer = index_ltan;
        chain1[j++] = ltan;
        pointer++;
        for (i = index_ltan; i < index_rtan - 1; i++) {
            chain1[j] = hull.getPoints()[pointer];
            j++;
            pointer++;
            if (pointer == hull.getSize()) {
                pointer = 0;
            }
        }
        chain1[j] = rtan;

        // Остальные точки идут в chain 2
        j = 0;
        pointer = index_rtan + 1;
        if (pointer == hull.getSize()) {
            pointer = 0;
        }

        while (pointer != index_ltan) {
            chain2[j++] = hull.getPoints()[pointer++];
            if (pointer == hull.getSize()) {
                pointer = 0;
            }
        }

    }

    else {
        pointer = 0;
        j = 0;
        i = 0;

        pointer = index_ltan;
        chain1[j++] = ltan;
        pointer++;
        if (pointer == hull.getSize()) {
            pointer = 0;
        }

        while (pointer != index_rtan) {
            chain1[j++] = hull.getPoints()[pointer++];
            if (pointer == hull.getSize()) {
                pointer = 0;
            }
            flag = 1;
        }
        chain1[j] = rtan;
        pointer++;

        //if (flag != 1) { chain1[j] = rtan; pointer++; }
        //else { chain1[++j] = rtan; pointer++; }

        j = 0;
        for (i = index_rtan + 1; i < index_ltan; i++) {
            chain2[j] = hull.getPoints()[pointer];
            j++;
            pointer++;
            if (pointer == hull.getSize()) {
                pointer = 0;
            }
        }
    }
}

ConvexHull merge_hulls(ConvexHull P1, ConvexHull P2) {

    // Найти некоторую внутреннюю точку p многоугольника P1.
    point p = find_internal_point(P1);

    int size1 = P1.getSize();
    int size2 = P2.getSize();

    // Определить, является ли точка p внутренней точкой P2.
    if (isInside(p, P2)) {

        // Получаем упорядоченный список вершин P1 и P2
        point* points = new point[size1 + size2];

        for (int i = 0; i < size1; i++) {
            points[i] = P1.getPoints()[i];
        }
        for (int i = 0; i < size2; i++) {
            points[size1 + i] = P2.getPoints()[i];
        }
        quicksort_points(points, 0, size1 + size2 - 1);



        // Применяем обход Грехема к упорядоченному списку вершин
        graham_alg(points, size1 + size2);

        ConvexHull merged_hull(points, size1 + size2);

        delete points;

        return merged_hull;
    }

    // Если p не является внутренней точкой P2, применяем описанный шаг
    else {

        // Находим вершины, разбивающие P2 на две монотонные цепи
        point* rtan;
        point* ltan;
        tangent_PointPoly(p, P2.getPoints(), P2.getSize(), &rtan, &ltan);

        // Высчитать размеры цепей chain1 и chain2  // !!!! ПРОВЕРИТЬ ПРАВИЛЬНОСТЬ ВЫЧИСЛЕНИЯ
        int index_rtan = P2.indexOf(*rtan);
        int index_ltan = P2.indexOf(*ltan);
        int pointer = index_ltan;
        int size_chain1 = 1;
        int size_chain2 = 0;
        while (pointer != index_rtan) {
            size_chain1++;
            pointer++;
            if (pointer == P2.getSize()) {
                pointer = 0;
            }
        }
        size_chain2 = P2.getSize() - size_chain1;

        point* chain1 = new point[size_chain1];
        point* chain2 = new point[size_chain2];

        // Разбиваем P2 на 2 цепи относительно точек rtan и ltan  // ПРИНИМАЕТ АДРЕСА ЦЕПЕЙ И ИХ РАЗМЕР И ЗАПОЛНЯЕТ ИХ
        split_hull(P2, *rtan, *ltan, chain1, chain2);

        point* points = new point[P1.getSize() + size_chain2];

        //Получаем упорядоченный список вершин из chain2
        for (int i = 0; i < P1.getSize(); i++) {
            points[i] = P1.getPoints()[i];
        }

        for (int i = 0; i < size_chain2; i++) {
            if (chain2[i].x == 0 && chain2[i].y == 0) { size_chain2 = size_chain2 - i; break; }
            points[size1 + i] = chain2[i];
        }

        quicksort_points(points, 0, P1.getSize() + size_chain2 - 1);

        // Применяем обход Грехема к упорядоченному списку вершин
        graham_alg(points, P1.getSize() + size_chain2);

        ConvexHull merged_hull(points, P1.getSize() + size_chain2);

        //delete points;
        delete chain2;
        delete chain1;

        return merged_hull;

    }
}

ConvexHull create_hull(point* points, int size) {
    if (size <= N) {
        // Если размер массива <= N, вызываем graham_alg для построения выпуклой оболочки
        return graham_alg(points, size);
    }

    int mid = size / 2;

    // Рекурсивно разделяем массив по полам
    ConvexHull left_hull = create_hull(points, mid);
    ConvexHull right_hull = create_hull(points + mid, size - mid);

    // Объединяем оболочки с помощью merge_hulls
    return merge_hulls(left_hull, right_hull);
}