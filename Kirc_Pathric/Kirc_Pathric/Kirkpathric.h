#pragma once
#include <iostream>
#include <stack>
#include <cmath>
#include <fstream>

#include "class.h"
#include "support.h"

#define N 6 // ����������� ���������� ����� ��� ������ ������ �������

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

            // ��������� ������� ����� �����
            point top = stack.top();

            // ������� ������� ����� �����
            stack.pop();

            // ����� ��������� �����
            point second = stack.top();

            // ���� ��� ������� �����������, ����� ������������ ������� � ����
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

// ��������: 
// �������� hull, ������� ����� ������� �� 2 ����
// ����� rtan, ltan ������� ��������� hull
// ������ �� ������� ����� chain1 � chain2, �� ������� ����� ������� hull
// � �������� ������ ��������� ������� �������� chain1 � chain2 � ��������� �� ���������������� �������. 
void split_hull(ConvexHull& hull, const point& rtan, const point& ltan, point*& chain1, point*& chain2) {

    int index_rtan = hull.indexOf(rtan);
    int index_ltan = hull.indexOf(ltan);
    int pointer = index_ltan;
    int i;
    int j;
    int flag = 0;
    int size_chain1 = 0;
    int size_chain2 = 0;

    // ���� chain 1 �������� ����� ���������;
    // ���� ltan < rtan ��������� ���, ��� chain1 �������� ����� �� ltan �� rtan ������������, chain2 �������� ��� ��������� �����
    // ���� rtan < ltan ��������� ���, ��� chain1 �������� ����� �� rtan �� ltan ������������, chain2 �������� ��� ��������� �����

    // ������ ������
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

        // ��������� ����� ���� � chain 2
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

    // ����� ��������� ���������� ����� p �������������� P1.
    point p = find_internal_point(P1);

    int size1 = P1.getSize();
    int size2 = P2.getSize();

    // ����������, �������� �� ����� p ���������� ������ P2.
    if (isInside(p, P2)) {

        // �������� ������������� ������ ������ P1 � P2
        point* points = new point[size1 + size2];

        for (int i = 0; i < size1; i++) {
            points[i] = P1.getPoints()[i];
        }
        for (int i = 0; i < size2; i++) {
            points[size1 + i] = P2.getPoints()[i];
        }
        quicksort_points(points, 0, size1 + size2 - 1);



        // ��������� ����� ������� � �������������� ������ ������
        graham_alg(points, size1 + size2);

        ConvexHull merged_hull(points, size1 + size2);

        delete points;

        return merged_hull;
    }

    // ���� p �� �������� ���������� ������ P2, ��������� ��������� ���
    else {

        // ������� �������, ����������� P2 �� ��� ���������� ����
        point* rtan;
        point* ltan;
        tangent_PointPoly(p, P2.getPoints(), P2.getSize(), &rtan, &ltan);

        // ��������� ������� ����� chain1 � chain2  // !!!! ��������� ������������ ����������
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

        // ��������� P2 �� 2 ���� ������������ ����� rtan � ltan  // ��������� ������ ����� � �� ������ � ��������� ��
        split_hull(P2, *rtan, *ltan, chain1, chain2);

        point* points = new point[P1.getSize() + size_chain2];

        //�������� ������������� ������ ������ �� chain2
        for (int i = 0; i < P1.getSize(); i++) {
            points[i] = P1.getPoints()[i];
        }

        for (int i = 0; i < size_chain2; i++) {
            if (chain2[i].x == 0 && chain2[i].y == 0) { size_chain2 = size_chain2 - i; break; }
            points[size1 + i] = chain2[i];
        }

        quicksort_points(points, 0, P1.getSize() + size_chain2 - 1);

        // ��������� ����� ������� � �������������� ������ ������
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
        // ���� ������ ������� <= N, �������� graham_alg ��� ���������� �������� ��������
        return graham_alg(points, size);
    }

    int mid = size / 2;

    // ���������� ��������� ������ �� �����
    ConvexHull left_hull = create_hull(points, mid);
    ConvexHull right_hull = create_hull(points + mid, size - mid);

    // ���������� �������� � ������� merge_hulls
    return merge_hulls(left_hull, right_hull);
}