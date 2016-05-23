//
// Created by Nicolas Casademont on 22/05/16.
//
#include <tgmath.h>
#include <iostream>

#include "Bezier.h"

glm::vec3 Bezier::bezierPoint(double t) {
    if (nbr_knot < 1) {
        double newT = t / nbr_elem;
        return calculateBezier(newT, 0, nbr_elem);
    } else {
        int part_begin = t;
        int part_end = part_begin + (per_knot - 1);
        double newT = fmod(t, per_knot - 1)/(per_knot - 1);

        if (part_end > nbr_knot) {
            newT *= fmod(t, per_knot - 1)/(per_knot - 1 + nbr_elem % per_knot);
            part_end = nbr_elem - 1;
        }

        return calculateBezier(newT, part_begin, part_end);
    }
}

void Bezier::addPoint(glm::vec3 &point) {
    if (nbr_elem > (per_knot - 1) && nbr_elem % (per_knot - 1) == 0) {
        list_points.at(nbr_elem - 1) = (list_points.at(nbr_elem - 2) + point) * 0.5f;
        nbr_knot++;
    }
    list_points.push_back(point);
    nbr_elem++;
}

glm::vec3 Bezier::calculateBezier(double t, int part_begin, int part_end) {
    glm::vec3 bn = glm::vec3(0.0f);

    for (int i = 0; i < part_end - part_begin; i++) {
        glm::vec3 bi = list_points.at(part_begin + i) * bernsteinPolynom(t, part_end - part_begin - 1, i);

        bn = bn + bi;
    }

    return bn;
}


float Bezier::bernsteinPolynom(double t, int n, int i) {
    return Bezier::binomial(n, i) * pow(t, i) * pow(1-t, n - i);
}

float Bezier::binomial(int n, int k) {
    return fact(n)/(fact(n - k)*fact(k));
}

float Bezier::fact(int n) {
    return (n == 1 || n == 0) ? 1 : fact(n - 1) * n;
}