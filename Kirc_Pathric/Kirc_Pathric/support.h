#pragma once

#include <iostream>
#include <stack>
#include <cmath>
#include <fstream>
#include <string>

// ‘ункци€ дл€ генерации случайных точек
void generateRandomPoints(int n, point* points_array) {

    // «аполн€ем массив случайными координатами
    for (int i = 0; i < n; i++) {
        points_array[i].x = (rand());
        points_array[i].y = (rand());
    }
}

// функци€ crossProduct возвращает знак псевдоскал€рного произведени€ двух векторов, 
//  которое €вл€етс€ положительным, если точка c находитс€ слева от вектора ab, и отрицательным,
//   если точка c находитс€ справа от вектора ab. 
double crossProduct(point a, point b, point c) {
    double ab_x = b.x - a.x;
    double ab_y = b.y - a.y;
    double ac_x = c.x - a.x;
    double ac_y = c.y - a.y;
    return (ab_x * ac_y) - (ab_y * ac_x);
}

// ‘ункци€ дл€ определени€ пол€рного угла между точками
double polar_angle(point p1, point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return atan2(dy, dx);
}

// ‘ункци€ дл€ сравнени€ точек по пол€рному углу
bool cmp(point p1, point p2, point first_point) {
    double angle1 = polar_angle(first_point, p1);
    double angle2 = polar_angle(first_point, p2);
    if (angle1 == angle2) {
        // ≈сли точки имеют одинаковый угол, то сортируем их по рассто€нию до первой точки
        double dist1 = sqrt(pow(p1.x - first_point.x, 2) + pow(p1.y - first_point.y, 2));
        double dist2 = sqrt(pow(p2.x - first_point.x, 2) + pow(p2.y - first_point.y, 2));
        return dist1 < dist2;
    }
    else {
        return angle1 < angle2;
    }
}

// ‘ункци€ дл€ сортировки массива точек по пол€рному углу относительно первой точки (“очки сортируютс€ в пор€дке уменьшени€ угла -> они будут упор€доченны в пор€дке обхода по часовой стрелке)
void sort_by_polar_angle(point* points, int size, point first_point) {
    std::sort(points, points + size, [first_point](point p1, point p2) { return cmp(p1, p2, first_point); });
}

point find_internal_point(ConvexHull hull) {

    int x_sum = 0;
    int y_sum = 0;

    // ¬ычисл€ем сумму координат точек многоугольника
    for (int i = 0; i < hull.getSize(); i++) {
        x_sum += hull.getPoints()[i].x;
        y_sum += hull.getPoints()[i].y;
    }

    // Ќаходим центроид
    point centroid(x_sum / hull.getSize(), y_sum / hull.getSize());

    //cout <<""<< centroid.x<< endl<< "" << centroid.y << endl;

    return centroid;
}

bool isInside(const point p, const ConvexHull P2) {

    point* points = P2.getPoints();
    int size = P2.getSize();

    for (int i = 0; i < size; i++) {

        point a = points[i];

        // оператор % вычисл€ет остаток от делени€, чтобы обеспечить зацикленность
        //  - когда мы дошли до последней вершины, следующа€ вершина будет первой вершиной, т.е. индекс 0.
        point b = points[(i + 1) % size];

        double cp = crossProduct(a, b, p);

        if (cp < 0) {
            return false;
        }
    }

    return true;
}

bool compare_points(const point& a, const point& b) {
    if (a.y == b.y) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

int partition(point* points, int low, int high) {
    point pivot = points[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (compare_points(points[j], pivot)) {
            i++;
            std::swap(points[i], points[j]);
        }
    }
    std::swap(points[i + 1], points[high]);
    return i + 1;
}

void quicksort_points(point* points, int low, int high) {
    if (low < high) {
        int pivot_index = partition(points, low, high);
        quicksort_points(points, low, pivot_index - 1);
        quicksort_points(points, pivot_index + 1, high);
    }
}

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
inline float isLeft(const point& p0, const point& p1, const point& p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

// tangent_PointPoly(): find any polygon's exterior tangents
//    Input:  p = a 2D point (exterior to the polygon)
//            P = vertices for any 2D polygon
//    Output: rtan = pointer to the rightmost tangent point
//            ltan = pointer of leftmost tangent point
void tangent_PointPoly(const point& p, point* P, int n, point** rtan, point** ltan) {

    double  eprev, enext;
    int    i;

    *rtan = &P[0];  // initially assume P[0] = both tangents
    *ltan = &P[0];

    eprev = isLeft(P[0], P[1], p);

    for (i = 1; i <= n; ++i) {
        enext = isLeft(P[i % n], P[(i + 1) % n], p);
        if ((eprev <= 0) && (enext > 0)) {
            if (isLeft(p, P[i % n], **rtan) >= 0)
                *rtan = &P[i % n];
        }
        else if ((eprev > 0) && (enext <= 0)) {
            if (isLeft(p, P[i % n], **ltan) <= 0)
                *ltan = &P[i % n];
        }
        eprev = enext;
    }
}

void readPointsFromFile(const std::string& filename, int size, point* points) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return;
    }

    for (int i = 0; i < size; i++) {
        file >> points[i].x >> points[i].y;
    }

    file.close();
}


int countFileLines(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return 0;
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line)) {
        count++;
    }

    file.close();
    return count;
}