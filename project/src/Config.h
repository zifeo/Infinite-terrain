#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TEX_WIDTH 1024
#define TEX_HEIGHT 1024

#define CHUNKS 2
#define DELTA 0.02
#define VIEW_DIST 2 // in chunk

#define CAMERA_SPEED 0.9f
#define CAMERA_ACCELERATION 3
#define CAMERA_DECELERATION 0.01
#define MOUSE_SENSIBILTY 0.002

#ifndef M_PI
#define M_PI 3.14159265
#endif

// Tree defines
#define TURNING_TREES                                                                                                  \
    0 // 0 if you want TREE_PLANE_COUNT planes trees, 1 if you want 1 plane which rotates towards the camera
#define TREE_PLANE_COUNT 2 // not used if TURNING_TREES is 1

#define TREE_HEIGHT 1 / 16.0

#define MAX_TREES_PER_CHUNK                                                                                            \
    40 // the number of trees will be from MAX_TREES_PER_CHUNK/2 to MAX_TREES_PER_CHUNK (counting the trees that will be
       // discarded later)
#define MAX_TREE_ALT 0.3 // trees that spawn outside of this range will be discarded
#define MIN_TREE_ALT 0

// Physic defines
#define G 0.01
#define JUMP_SPEED 0.015

// Biome defines
#define TEMPERATURE_OCTAVE 2
#define TEMPERATURE_LAC 2
#define TEMPERATURE_H 1.25

#define ALTITUDE_OCTAVE 2
#define ALTITUDE_LAC 2
#define ALTITUDE_H 1.25

enum TreeType { NORMAL_TREE = 0, DESERT_TREE, CACTUS, ALGAE };

#define BIOME_COUNT 4

enum BiomeType { NORMAL_BIOME = 0, DESERT, HIGH_MOUNTAIN, SEA };

#define WATER_SIZE 6
