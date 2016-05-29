#pragma once

#include "glm/vec3.hpp"
#include "icg_helper.h"

#include <vector>

class Bezier {

  public:
    glm::vec3 bezierPoint(double t) {
        if (nbr_knot < 1) {
            double newT = t / nbr_elem;
            return calculateBezier(newT, 0, nbr_elem - 1);
        } else {
            int part_begin = floor(t / (per_knot - 1)) * (per_knot - 1);
            int part_end = part_begin + (per_knot - 1);

            if (part_end > nbr_knot * (per_knot - 1)) {
                part_begin = nbr_knot * (per_knot - 1);
                part_end = nbr_elem - 1;
            }

            int diff = part_end - part_begin;
            double newT = (t - part_begin) / diff;

            return calculateBezier(newT, part_begin, part_end);
        }
    }

    void addPoint(glm::vec3 &point) {
        if (nbr_elem > (per_knot - 1) && nbr_elem % (per_knot - 1) == 0) {
            int knot_point = nbr_elem - (per_knot - 1);
            list_points.at(knot_point) = (list_points.at(knot_point - 1) + list_points.at(knot_point + 1)) * 0.5f;
            cout << "before ";
            print_vec3(list_points.at(knot_point - 1));
            cout << endl;
            cout << "change point " << knot_point << " to ";
            print_vec3(list_points.at(knot_point));
            cout << endl;
            cout << "after ";
            print_vec3(list_points.at(knot_point - 1));
            cout << endl;
            nbr_knot++;
        }

        list_points.push_back(point);
        nbr_elem++;
    }

    void purge() {
        list_points.clear();
        nbr_knot = 0;
        nbr_elem = 0;
    };

    void print_list() {
        for (auto &vec : list_points) // access by reference to avoid copying
        {
            cout << "vec ";
            print_vec3(vec);
            cout << endl;
        }
    }

    static void print_vec3(glm::vec3 vec) { cout << ": " << vec.x << ", " << vec.y << ", " << vec.z; }

    int get_nbr_elem() const { return nbr_elem; }

  private:
    vector<glm::vec3> list_points;
    int nbr_elem = 0;
    int per_knot = 4;
    int nbr_knot = 0;

    glm::vec3 calculateBezier(double t, int part_begin, int part_end) {
        glm::vec3 bn = glm::vec3(0.0f);
        for (int i = 0; i < part_end - part_begin + 1; i++) {
            glm::vec3 bi = list_points.at(part_begin + i) * bernsteinPolynom(t, part_end - part_begin, i);
            bn = bn + bi;
        }
        return bn;
    }

    static float bernsteinPolynom(double t, int n, int i) {
        return (float)(Bezier::binomial(n, i) * pow(t, i) * pow(1 - t, n - i));
    }

    static float binomial(int n, int k) { return fact(n) / (fact(n - k) * fact(k)); }

    static float fact(int n) { return (n == 1 || n == 0) ? 1 : fact(n - 1) * n; }
};
