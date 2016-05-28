//
// Created by Nicolas Casademont on 22/05/16.
//

#ifndef ICG16_BEZIER_H
#define ICG16_BEZIER_H
#pragma once

#include "glm/vec3.hpp"

#include <vector>

using namespace std;

class Bezier {
    public:
        glm::vec3 bezierPoint(double t);
        static void print_vec3(glm::vec3 vec);
        void addPoint(glm::vec3 &point);
        void print_list();

        int get_nbr_elem() const {
            return nbr_elem;
        }

        void purge(){
            list_points.clear();
            nbr_knot = 0;
            nbr_elem = 0;
        };

    private:
        vector<glm::vec3> list_points;
        int nbr_elem = 0;
        int per_knot = 4;
        int nbr_knot = 0;

        glm::vec3 calculateBezier(double t, int part_begin, int part_end);

        static float bernsteinPolynom(double t, int n, int i);
        static float binomial(int n, int k);
        static float fact(int n);
};

#endif // ICG16_BEZIER_H
