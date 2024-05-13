#include "meshgenerator.h"
#include "rendering.h"
#include "fixedmath.h"

void (*displacement_func)(vertex_t *, uint16_t);

const int16_t X = 34;
const int16_t Z = 64;
const int16_t N = 0;

const vector3_t ico_verts[] = {
  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z}, {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X}, {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

const uint8_t ico_indices[] = {
  0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
};

void finalize_mesh(const int num_triangles)
{
  current_model.triangles = &triangle_list[0];
  for (int i = 0; i < num_triangles - 1; i++) {
    triangle_list[i].next = &triangle_list[i+1];
  }
    
  current_model.triangles[num_triangles - 1].next = 0;
}

void new_mesh()
{
  triangle_count = 0;
  current_model.triangles[0].next = 0;
}

void displace_vertex(vertex_t *in, uint16_t value)
{
  int x = in->position.x;
  int y = in->position.y;
  int z = in->position.z;

  int rep = 2;
	int scale = sin(((x * rep) + (value >> 0))) + 
              sin(((y * rep) + (value << 1))) + 
              sin(((z * rep) + (value >> 1)));

  in->position.x += (in->position.x) * (scale >> 4) >> 5; 
  in->position.y += (in->position.y) * (scale >> 4) >> 5; 
  in->position.z += (in->position.z) * (scale >> 4) >> 5; 
}

void IWRAM_CODE load_model(const int16_t *model_data, int16_t num_triangles, uint8_t flags, uint8_t scale, uint8_t texture_index)
{
  const uint8_t shift = 6;

  current_model.flags = flags;

  if (flags & MODEL_TEXTURED) {
    for (uint16_t i = 0; i < num_triangles; i++) {
        triangle_list[i].vertices[2].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[2].coords.y = -((*model_data++) >> 7);
        triangle_list[i].vertices[2].coords.z = ((*model_data++) >> 7);
        triangle_list[i].vertices[1].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[1].coords.y = -((*model_data++) >> 7);
        triangle_list[i].vertices[1].coords.z = ((*model_data++) >> 6);        
        triangle_list[i].vertices[0].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[0].coords.y = -((*model_data++) >> 7);
        triangle_list[i].vertices[0].coords.z = ((*model_data++) >> 6);
        triangle_list[i].texture_index = texture_index;
    }
  }
  else 
  if (flags & MODEL_ENVMAPPED) {
    for (uint16_t i = 0; i < num_triangles; i++) {
        triangle_list[i].vertices[2].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[2].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[2].coords.y = ((*model_data++) >> 6);
        triangle_list[i].vertices[2].coords.z = ((*model_data++) >> 6); 
        triangle_list[i].vertices[1].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[1].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[1].coords.y = ((*model_data++) >> 6);
        triangle_list[i].vertices[1].coords.z = ((*model_data++) >> 6);
        triangle_list[i].vertices[0].position.x = -(((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].position.y = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].position.z = (((*model_data++) * scale) >> shift);
        triangle_list[i].vertices[0].coords.x = ((*model_data++) >> 6);
        triangle_list[i].vertices[0].coords.y = ((*model_data++) >> 6);
        triangle_list[i].vertices[0].coords.z = ((*model_data++) >> 6);
        triangle_list[i].texture_index = texture_index;
    }
  }
}


// Define the fixed-point data type and scaling factor
#define FIXED_POINT_BITS 16 // Number of fractional bits


void _normalize_vector(vector3_t *vector) {
    int max_component = 0;
    if (abs(vector->x) > max_component) {
        max_component = abs(vector->x);
    }
    if (abs(vector->y) > max_component) {
        max_component = abs(vector->y);
    }
    if (abs(vector->z) > max_component) {
        max_component = abs(vector->z);
    }

    // Normalize the vector to have a length of 255
    if (max_component != 0) {
        int alpha = (1 << FIXED_POINT_BITS) / max_component;
        vector->x = (vector->x * 255) / alpha;
        vector->y = (vector->y * 255) / alpha;
        vector->z = (vector->z * 255) / alpha;
    }
}



// Cross product function
vector3_t _cross_product(vector3_t v1, vector3_t v2) {
    vector3_t result;
    result.x = (v1.y * v2.z - v1.z * v2.y);
    result.y = (v1.z * v2.x - v1.x * v2.z);
    result.z = (v1.x * v2.y - v1.y * v2.x);
    return result;
}

// Function to generate normals for each face of the model
void generate_normals(model_t *model) {
    triangle_t *face = model->triangles;
    while (face != NULL) {
        for (int i = 0; i < 3; ++i) {
            face->vertices[i].coords.x = 0;
            face->vertices[i].coords.y = 0;
            face->vertices[i].coords.z = 0;
        }
        face = face->next;        
    }

    face = model->triangles;
    while (face != NULL) {
        vector3_t v1, v2;
        v1.x = face->vertices[1].position.x - face->vertices[0].position.x;
        v1.y = face->vertices[1].position.y - face->vertices[0].position.y;
        v1.z = face->vertices[1].position.z - face->vertices[0].position.z;
        v2.x = face->vertices[2].position.x - face->vertices[0].position.x;
        v2.y = face->vertices[2].position.y - face->vertices[0].position.y;
        v2.z = face->vertices[2].position.z - face->vertices[0].position.z;
        face->normal = _cross_product(v1, v2);
        _normalize_vector(&face->normal);

        for (int i = 0; i < 3; ++i) {
            face->vertices[i].coords.x += (face->normal.x >> 8);
            face->vertices[i].coords.y += (face->normal.y >> 8);
            face->vertices[i].coords.z += (face->normal.z >> 8);
        }

        face = face->next;
    }

    // Normalize vertex normals
    face = model->triangles;
    vector3_t temp_normal;
    while (face != NULL) {
        for (int i = 0; i < 3; ++i) {
            temp_normal.x = face->vertices[i].coords.x;
            temp_normal.y = face->vertices[i].coords.y;
            temp_normal.z = face->vertices[i].coords.z;   

            _normalize_vector(&temp_normal);
            face->vertices[i].coords.x = temp_normal.x;
            face->vertices[i].coords.y = temp_normal.y;
            face->vertices[i].coords.z = temp_normal.z;                    
        }
        face = face->next;
    }
}



void displace_none(vertex_t *in, uint16_t value)
{
}

void subdivide_icosahedron(uint16_t offset, uint8_t mode, uint8_t texture_index)
{
  int i = 0, j = 0;
  vector3_t edge1, edge2, edge3;
  int16_t index = 0;

  switch (mode) {
    case MESH_VERTEXDISPLACE:
      displacement_func = displace_vertex;
      break;
    case MESH_STATIC:
      displacement_func = displace_none;
    default:
    break;
  }

  for (i = 0; i < 20; i++) {
    int16_t t0x0 = ico_verts[ico_indices[index + 0]].x;
    int16_t t0y0 = ico_verts[ico_indices[index + 0]].y;
    int16_t t0z0 = ico_verts[ico_indices[index + 0]].z;

    int16_t t0x1 = ico_verts[ico_indices[index + 1]].x;
    int16_t t0y1 = ico_verts[ico_indices[index + 1]].y;
    int16_t t0z1 = ico_verts[ico_indices[index + 1]].z;

    int16_t t0x2 = ico_verts[ico_indices[index + 2]].x;
    int16_t t0y2 = ico_verts[ico_indices[index + 2]].y;
    int16_t t0z2 = ico_verts[ico_indices[index + 2]].z;
  
    edge1.x = t0x0 + ((t0x1 - t0x0) >> 1);
    edge1.y = t0y0 + ((t0y1 - t0y0) >> 1);
    edge1.z = t0z0 + ((t0z1 - t0z0) >> 1);

    edge2.x = t0x1 + ((t0x2 - t0x1) >> 1);
    edge2.y = t0y1 + ((t0y2 - t0y1) >> 1);
    edge2.z = t0z1 + ((t0z2 - t0z1) >> 1);

    edge3.x = t0x2 + ((t0x0 - t0x2) >> 1);
    edge3.y = t0y2 + ((t0y0 - t0y2) >> 1);
    edge3.z = t0z2 + ((t0z0 - t0z2) >> 1);

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge3.x;
    triangle_list[j].vertices[0].position.y = edge3.y;
    triangle_list[j].vertices[0].position.z = edge3.z;
    triangle_list[j].vertices[1].position.x = t0x0;
    triangle_list[j].vertices[1].position.y = t0y0;
    triangle_list[j].vertices[1].position.z = t0z0;
    triangle_list[j].vertices[2].position.x = edge1.x;
    triangle_list[j].vertices[2].position.y = edge1.y;
    triangle_list[j].vertices[2].position.z = edge1.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge1.x;
    triangle_list[j].vertices[0].position.y = edge1.y;
    triangle_list[j].vertices[0].position.z = edge1.z;
    triangle_list[j].vertices[1].position.x = t0x1;
    triangle_list[j].vertices[1].position.y = t0y1;
    triangle_list[j].vertices[1].position.z = t0z1;
    triangle_list[j].vertices[2].position.x = edge2.x;
    triangle_list[j].vertices[2].position.y = edge2.y;
    triangle_list[j].vertices[2].position.z = edge2.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge2.x;
    triangle_list[j].vertices[0].position.y = edge2.y;
    triangle_list[j].vertices[0].position.z = edge2.z;
    triangle_list[j].vertices[1].position.x = t0x2;
    triangle_list[j].vertices[1].position.y = t0y2;
    triangle_list[j].vertices[1].position.z = t0z2;
    triangle_list[j].vertices[2].position.x = edge3.x;
    triangle_list[j].vertices[2].position.y = edge3.y;
    triangle_list[j].vertices[2].position.z = edge3.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge1.x;
    triangle_list[j].vertices[0].position.y = edge1.y;
    triangle_list[j].vertices[0].position.z = edge1.z;
    triangle_list[j].vertices[1].position.x = edge2.x;
    triangle_list[j].vertices[1].position.y = edge2.y;
    triangle_list[j].vertices[1].position.z = edge2.z;
    triangle_list[j].vertices[2].position.x = edge3.x;
    triangle_list[j].vertices[2].position.y = edge3.y;
    triangle_list[j].vertices[2].position.z = edge3.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;
    
    index += 3;
  }

  finalize_mesh(79+1);
}
