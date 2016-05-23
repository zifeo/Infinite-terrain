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
        void addPoint(glm::vec3 &point);
    private:
        int per_knot = 4;
        vector<glm::vec3> list_points;
        int nbr_knot = 0;
        int nbr_elem = 0;

        glm::vec3 calculateBezier(double t, int part_begin, int part_end);

        static float bernsteinPolynom(double t, int n, int i);
        static float binomial(int n, int k);
        static float fact(int n);
};


#endif //ICG16_BEZIER_H
