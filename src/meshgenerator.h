#ifndef __MESHGENERATOR_H
#define __MESHGENERATOR_H

#include <stdint.h>
#include "rendering.h"

#define MESH_STATIC 1 << 1
#define MESH_VERTEXDISPLACE 1 << 2

void new_mesh();
void finalize_mesh(const int num_triangles);
void load_model(const int16_t *model_data, int16_t num_triangles, uint8_t flags, uint8_t scale, uint8_t texture_index);
void generate_normals(model_t *model);
void subdivide_icosahedron(uint16_t offset, uint8_t mode, uint8_t texture_index);

#endif /* __MESHGENERATOR_H */