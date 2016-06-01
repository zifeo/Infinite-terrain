#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TEX_WIDTH 512
#define TEX_HEIGHT 512

#define CHUNKS 2
#define DELTA 0.02
#define VIEW_DIST 3 // in chunk

#define CAMERA_SPEED (1.0 / 40 * 1)
#define CAMERA_ACCELERATION (CAMERA_SPEED / 15)
#define CAMERA_DECELERATION (0.8)
#define MOUSE_SENSIBILTY 0.002

#define CAM_PHI_MAX ((float)(9 * M_PI / 10))
#define CAM_PHI_MIN ((float)(M_PI / 10))
#define CAM_PHI_MIN_PAD 0.5
#define CAM_PHI_DT 0.5
#define CAM_THETA_DT 1

#ifndef M_PI
#define M_PI 3.14159265
#endif

// Tree defines
#define TREE_PLANE_COUNT 2

#define TREE_HEIGHT 1 / 16.0

#define MAX_TREES_PER_CHUNK                                                                                            \
    40 // the number of trees will be from MAX_TREES_PER_CHUNK/2 to MAX_TREES_PER_CHUNK (counting the trees that will be
       // discarded later)

// Physic defines
#define G 0.001
#define JUMP_SPEED 0.015

// Biome defines
#define TEMPERATURE_OCTAVE 2
#define TEMPERATURE_LAC 2
#define TEMPERATURE_H 1.25

#define ALTITUDE_OCTAVE 2
#define ALTITUDE_LAC 2
#define ALTITUDE_H 1.25

enum TreeType { NORMAL_TREE = 0, DESERT_TREE, CACTUS, ALGAE, SNOWY_TREE, REED };

#define BIOME_COUNT 5

enum BiomeType { NORMAL_BIOME = 0, DESERT, HIGH_MOUNTAIN, SEA, OCEAN };

#define WATER_SIZE 8
#define WATER_HEIGHT -0.3f
#define WATER_AMPL 0.01f
