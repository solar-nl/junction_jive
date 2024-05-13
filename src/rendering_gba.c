#include <gba_video.h>
#include "rendering.h"
#include "fixedmath.h"
#include "assets.h"

//#define USER_CAM 

const vector2_t clipping_planes[] = {{0, 0}, {234, 0}, {234, 160}, {0, 160}};

const uint8_t *texture_list[] = { envmapBitmap2 , 0, 0, envmap_zwierBitmap };

extern uint16_t *framebuffer;
extern uint16_t *backbuffer;
extern uint32_t effect_frame;

vector3_t camera_position;
vector3_t euler_angles; 

EWRAM_DATA triangle_t triangle_list[MAX_VERTICES];
EWRAM_DATA triangle_t transformed_list[MAX_VERTICES]; 
EWRAM_DATA uint16_t draw_order[MAX_FACES];

vertex_t temp_vertices[12];
vertex_t clip_buffer[12];

model_t current_model;
uint16_t triangle_count = 0;


void set_palette(const uint16_t *palette)
{
  fast_copy(((unsigned short*)0x5000000), palette, 256);
}

void clear_screen(uint8_t color)
{
  fast_set(framebuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
}

void clear_buffers(uint8_t color)
{
  fast_set(framebuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
  fast_set(backbuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
}

void set_camera_position(int16_t x, int16_t y, int16_t z)
{
  camera_position.x = x;
  camera_position.y = y;
  camera_position.z = z;
}

void set_euler_angles(int16_t x, int16_t y, int16_t z)
{
  euler_angles.x = x;
  euler_angles.y = y;
  euler_angles.z = z; 
}

void init_renderer()
{
  set_camera_position(0, 0, 0);
  set_euler_angles(0, 0, 0);
}

void IWRAM_CODE put_pixel(uint8_t x, uint8_t y, uint8_t color)
{

  if ((x < 0) | (x > 240) | (y < 0) | (y > 160))
    return;

  uint16_t *target = (uint16_t*)framebuffer + ((y * 240 + x) >> 1);

  if (x & 1) {
    *target = (*target & 0xFF) | (color << 8);
  }
  else {
    *target = (*target & ~0xFF) | color;
  }
}

void IWRAM_CODE line(int x0, int y0, int x1, int y1, uint8_t color)
{
  if (y0 > y1) 
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int dx = x1 - x0;
  int dy = y1 - y0;

  int gradient = dx < 0 ? -1 : 1;
  dx *= gradient;

  put_pixel(x0, y0, color);

  if (dy == 0) 
  {
    while (dx--) put_pixel(x0 += gradient, y0, color);
    return;
  }

  if (dx == 0) 
  { 
    while (dy--) put_pixel(x0, y0++, color);
    return;
  }

  if (dx == dy) 
  {
    while (dy--) put_pixel(x0 += gradient, y0++, color);
    return;
  }
  
  uint16_t currentError;
  uint16_t error = 0;
  uint16_t intensity = 0;
   
  if (dy > dx) 
  {
    uint16_t fract = div(dx << 16, dy);

    while (--dy) 
    {
      currentError = error;   
      x0 += ((error += fract) <= currentError) ? gradient : 0;

      intensity = error >> 8;

      put_pixel(x0, ++y0, color/* + intensity*/);
      put_pixel(x0 + gradient, y0, color/* + (intensity ^ 255)*/);
    }
  }
  else
  {
    uint16_t fract = div(dy << 16, dx);

    while (--dx) 
    {
      currentError = error; 
      y0 += ((error += fract) <= currentError) ? 1 : 0;

      intensity = error >> 8;

      put_pixel(x0 += gradient, y0, color/* + intensity*/);
      put_pixel(x0, y0 + 1, color/* + (intensity ^ 255)*/);
    }
  }
  put_pixel(x1, y1, color);
}

void IWRAM_CODE transform(model_t *model, vertex_t *v, matrix3x3_t m)
{    
  v->position.x -= camera_position.x;
  v->position.y -= camera_position.y;
  v->position.z -= camera_position.z;

  int nx = (m[0][0] * v->position.x + m[0][1] * v->position.y + m[0][2] * v->position.z) >> 8;
  int ny = (m[1][0] * v->position.x + m[1][1] * v->position.y + m[1][2] * v->position.z) >> 8;
  int nz = (m[2][0] * v->position.x + m[2][1] * v->position.y + m[2][2] * v->position.z) >> 8;

  v->position.x = nx;
  v->position.y = ny;
  v->position.z = nz;

  v->position.x += model->position.x;
  v->position.y += model->position.y;
  v->position.z += model->position.z;
}

void IWRAM_CODE generate_envmap_coords(vertex_t *v, matrix3x3_t m)
{
  int nx = (m[0][0] * v->coords.x + m[0][1] * v->coords.y + m[0][2] * -v->coords.z) >> 10;
  int ny = (m[1][0] * -v->coords.x + m[1][1] * v->coords.y + m[1][2] * v->coords.z) >> 10;

  v->coords.x = 128 - nx;
  v->coords.y = 64 - ny; 
  }

void IWRAM_CODE project(vertex_t *v)
{ 
#if 1
  int z = rcp(v->position.z >> 1);

  v->position.x = (v->position.x * z >> 10) + (VIEWPORT_WIDTH >> 1);
  v->position.y = (v->position.y * z >> 10) + (VIEWPORT_HEIGHT >> 1);

#else
  int z = prevent_zero(v->position.z);

  v->position.x = _div(v->position.x << 7, v->position.z) + (VIEWPORT_WIDTH >> 1);
  v->position.y = _div(v->position.y << 7, v->position.z) + (VIEWPORT_HEIGHT >> 1);
#endif
}

bool IWRAM_CODE is_inside_viewport(triangle_t *t)
{
  return !((t->vertices[0].position.x < 0 && t->vertices[1].position.x < 0 && t->vertices[2].position.x < 0) ||
        (t->vertices[0].position.y < 0 && t->vertices[1].position.y < 0 && t->vertices[2].position.y < 0) ||
        (t->vertices[0].position.x >= (VIEWPORT_RIGHT-1) && t->vertices[1].position.x >= (VIEWPORT_RIGHT-1) && t->vertices[2].position.x >= (VIEWPORT_RIGHT-1)) || 
        (t->vertices[0].position.y >= (VIEWPORT_HEIGHT-1) && t->vertices[1].position.y >= (VIEWPORT_HEIGHT-1) && t->vertices[2].position.y >= (VIEWPORT_HEIGHT-1))); 
}

bool IWRAM_CODE is_clockwise(triangle_t *t) 
{
  return ((t->vertices[1].position.x - t->vertices[0].position.x) * 
          (t->vertices[2].position.y - t->vertices[1].position.y) - 
          (t->vertices[1].position.y - t->vertices[0].position.y) * 
          (t->vertices[2].position.x - t->vertices[1].position.x)) > 0;
}

bool IWRAM_CODE is_behind_camera(triangle_t *t)
{
  int z = camera_position.z - 200;
  return (t->vertices[0].position.z > z && t->vertices[1].position.z > z && t->vertices[2].position.z > z);
}

void IWRAM_CODE swap_indices(uint16_t *a, uint16_t *b, uint16_t l, uint16_t r)
{
  uint16_t t = draw_order[l];
  draw_order[l] = draw_order[r];
  draw_order[r] = t;
}

void IWRAM_CODE quick_sort(uint16_t *list, uint16_t start, uint16_t end)
{
  uint16_t z, l, r;

  if (end > start + 1)
  {
    z = transformed_list[list[start]].depth;
    l = start + 1;
    r = end;

    while (l < r)
    {
      if (transformed_list[list[l]].depth < z)
      { 
        l++;
      }
      else
      {
        r--;
        swap_indices(&list[l], &list[r], l, r);
      }
    }
    l--;
    swap_indices(&list[l], &list[start], l, start);
    quick_sort(list, start, l);
    quick_sort(list, r, end);
  }
} 

void IWRAM_CODE texture_polygon(vertex_t *vtx, int vertices, uint16_t *texture)
{
  int c = 0;
  int i, top_index, bottom_index, left_index, right_index;
  int min_y = 99999, max_y = 0;
  for (i = 0; i < vertices; i++) {
      if ((vtx[i].position.y) < min_y) {min_y = (vtx[i].position.y); top_index = i;}
      if ((vtx[i].position.y) > max_y) {max_y = (vtx[i].position.y); bottom_index = i;}
  }

  left_index = right_index = top_index;
  int y = min_y * 120;
  int length = max_y - min_y;
  int right_x, right_dx, left_x, left_dx, left_u, left_du, left_v, left_dv, right_u, right_v, right_du, right_dv;
  int n, stride, oud_x, oud_u, oud_v, tu, tv, len, du, dv;
  
  int left_height = 0, right_height = 0;
  uint16_t *start, *line = (uint16_t*)(framebuffer + y);
  uint8_t *bitmap = (uint8_t*)texture;
  do {
      if (--left_height <= 0) {
          if (left_index == bottom_index) {
            return;
          }   

          left_x = oud_x = vtx[left_index].position.x;
          left_u = oud_u = vtx[left_index].coords.x;
          left_v = oud_v = vtx[left_index].coords.y;
          left_height = vtx[left_index].position.y;
          
          if (--left_index < 0) {
            left_index = vertices - 1;
          }
          
          left_height = (vtx[left_index].position.y) - (left_height);
          
          left_dx = divide((vtx[left_index].position.x - oud_x) << 16, left_height);
          left_du = divide((vtx[left_index].coords.x - oud_u) << 16, left_height);
          left_dv = divide((vtx[left_index].coords.y - oud_v) << 16, left_height);
          left_x <<= 16;
          left_u <<= 16;
          left_v <<= 16;
      }
      {
          left_x += left_dx;
          left_u += left_du;
          left_v += left_dv;
      }
      
      if (--right_height <= 0) {
          if (right_index == bottom_index) {
            return;
          }

          right_x = oud_x = vtx[right_index].position.x;
          right_u = oud_u = vtx[right_index].coords.x;
          right_v = oud_v = vtx[right_index].coords.y;
          right_height = vtx[right_index].position.y;
          
          if (++right_index >= vertices) {
            right_index = 0;
          }
          
          right_height = (vtx[right_index].position.y) - (right_height);
          
          right_dx = divide((vtx[right_index].position.x - oud_x) << 16, right_height);
          right_du = divide((vtx[right_index].coords.x - oud_u) << 16, right_height);
          right_dv = divide((vtx[right_index].coords.y - oud_v) << 16, right_height);
          right_x <<= 16;
          right_u <<= 16;
          right_v <<= 16;
      }
      {
          right_x += right_dx;
          right_u += right_du;
          right_v += right_dv;
      }
    
      tu = left_u;
      tv = left_v;

      start = (uint16_t*)(line + (left_x >> 17));

      #define BLIT \
      { \
        c = (bitmap[(((tu >> 16)) + (((tv >> 16)) << 9)) & 0xFFFF]); \
        tu += du; tv += dv; \
        c |= (bitmap[(((tu >> 16)) + (((tv >> 16)) << 9)) & 0xFFFF]) << 8; \
        tu += du; tv += dv; \
        *start++ = c; \
      }      

      len = ((right_x) - (left_x)) >> 17;

      du = divide((right_u - left_u), len << 1);
      dv = divide((right_v - left_v), len << 1);

      if (len >= 0) {
        stride = len + 2;
        n = (stride + 7) >> 3;

        switch (stride & 7) {
          case 0: do { BLIT;   
          case 7: BLIT;
          case 6: BLIT;
          case 5: BLIT; 
          case 4: BLIT;   
          case 3: BLIT;
          case 2: BLIT;
          case 1: BLIT;  
          } while (--n); 
        }
      }
      line += 120;
  } while (--length);
}

void IWRAM_CODE wire_polygon(vertex_t *vertices, int num_vertices, uint16_t *texture)
{
  uint16_t i, j;
  for (i = 0; i < num_vertices; ++i) {
    j = (i + 1) % num_vertices;

    line(vertices[i].position.x, vertices[i].position.y, vertices[j].position.x, vertices[j].position.y, 196);
  }  
}

#define x_intersect(x1, y1, x2, y2, x3, y3, x4, y4) ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)); 
#define y_intersect(x1, y1, x2, y2, x3, y3, x4, y4) ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)); 

int IWRAM_CODE clip(vertex_t *vertices, uint16_t num_vertices, int x1, int y1, int x2, int y2) {
    uint16_t i, j, new_count = 0;
    int ix, iy, jx, jy, c1, c2, tx, ty, dx, dy;

    for (i = 0; i < num_vertices; ++i) {
        j = (i + 1) % num_vertices;

        ix = vertices[i].position.x; 
        iy = vertices[i].position.y;
        jx = vertices[j].position.x; 
        jy = vertices[j].position.y;

        dx = jx - ix;
        dy = jy - iy;        

        c1 = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);  
        c2 = (x2 - x1) * (jy - y1) - (y2 - y1) * (jx - x1);

        if (c1 >= 0 && c2 >= 0) {
            temp_vertices[new_count] = vertices[j]; 
            ++new_count;
        } else if (c1 < 0 && c2 >= 0) {
            temp_vertices[new_count].position.x = x_intersect(x1, y1, x2, y2, ix, iy, jx, jy);
            temp_vertices[new_count].position.y = y_intersect(x1, y1, x2, y2, ix, iy, jx, jy);

            tx = ((temp_vertices[new_count].position.x - vertices[i].position.x) << 8) / dx;
            ty = ((temp_vertices[new_count].position.y - vertices[i].position.y) << 8) / dy;
            
            temp_vertices[new_count].coords.x = lerp(vertices[i].coords.x, vertices[j].coords.x, tx);
            temp_vertices[new_count].coords.y = lerp(vertices[i].coords.y, vertices[j].coords.y, ty);
            ++new_count;

            temp_vertices[new_count] = vertices[j];   
            ++new_count;
        } else if (c1 >= 0 && c2 < 0) {
            temp_vertices[new_count].position.x = x_intersect(x1, y1, x2, y2, ix, iy, jx, jy);
            temp_vertices[new_count].position.y = y_intersect(x1, y1, x2, y2, ix, iy, jx, jy);

            tx = ((temp_vertices[new_count].position.x - vertices[i].position.x) << 8) / dx;
            ty = ((temp_vertices[new_count].position.y - vertices[i].position.y) << 8) / dy;

            temp_vertices[new_count].coords.x = lerp(vertices[i].coords.x, vertices[j].coords.x, tx);
            temp_vertices[new_count].coords.y = lerp(vertices[i].coords.y, vertices[j].coords.y, ty);
            ++new_count;
        }
    }

    for (i = 0; i < new_count; i++) { 
        clip_buffer[i] = temp_vertices[i]; 
    }

    return new_count;
}
int IWRAM_CODE sutherland_hodgman_clipping(vertex_t *vertices, uint16_t num_vertices)
{
  uint16_t i, j, n = num_vertices;

  for (i = 0; i < 4; ++i) {
    j = (i + 1) & 3;

    n = clip(vertices, n, clipping_planes[i].x, clipping_planes[i].y, clipping_planes[j].x, clipping_planes[j].y);
  }

  return n;
}

void IWRAM_CODE render(model_t *model)
{
  int i;
  int num_triangles = 0;
  matrix3x3_t m;

#ifdef USER_CAM
  rotate(m, euler_angles.x, euler_angles.y, euler_angles.z);
#else
  rotate(m, model->euler_angles.x, model->euler_angles.y, model->euler_angles.z);
#endif

  triangle_t *idx = model->triangles;
  if (model->flags & MODEL_SOFT) { /* for now this is just a hack for one effect */
    while (idx != 0) {
      triangle_t face;

      for (i = 0; i < 3; i++) {
        face.vertices[i] = idx->vertices[i];

        int sqr_dist = sqrt(face.vertices[i].position.x * face.vertices[i].position.x + face.vertices[i].position.y * face.vertices[i].position.y + face.vertices[i].position.z * face.vertices[i].position.z) >> 2;

        rotate(m, sin(effect_frame * 8 - sqr_dist) >> 3, -cos(effect_frame * 3 - sqr_dist) >> 3, 0);  

        int nx = ((m[0][0] * idx->vertices[i].position.x + m[0][1] * idx->vertices[i].position.y + m[0][2] * idx->vertices[i].position.z)) >> 8;
        int ny = ((m[1][0] * idx->vertices[i].position.x + m[1][1] * idx->vertices[i].position.y + m[1][2] * idx->vertices[i].position.z)) >> 8;
        int nz = ((m[2][0] * idx->vertices[i].position.x + m[2][1] * idx->vertices[i].position.y + m[2][2] * idx->vertices[i].position.z)) >> 8;

        face.vertices[i].position.x = nx + model->position.x;
        face.vertices[i].position.y = ny + model->position.y;
        face.vertices[i].position.z = nz + model->position.z;
        

        face.texture_index = idx->texture_index;
      }

      if (!is_behind_camera(&face)) {
          {
          for (i = 0; i < 3; i++) {
            project(&face.vertices[i]);
          }

          if (is_clockwise(&face) && is_inside_viewport(&face)/* && !is_behind_camera(&face)*/) {
            face.depth = min(min(face.vertices[0].position.z, face.vertices[1].position.z), face.vertices[2].position.z);
            transformed_list[num_triangles++] = face;
          }
        }
      }
      idx = idx->next;
    }    
  }
  else {
    while (idx != 0) {
      triangle_t face;

      for (i = 0; i < 3; i++) {
        face.vertices[i] = idx->vertices[i];

        transform(model, &face.vertices[i], m);

        face.texture_index = idx->texture_index;
      }

      if (!is_behind_camera(&face)) {
          {
          for (i = 0; i < 3; i++) {
            project(&face.vertices[i]);
          }

          if (is_clockwise(&face) && is_inside_viewport(&face)/* && !is_behind_camera(&face)*/) {
            face.depth = min(min(face.vertices[0].position.z, face.vertices[1].position.z), face.vertices[2].position.z);
            transformed_list[num_triangles++] = face;
          }
        }
      }
      idx = idx->next;
    }
  }


  if (model->flags & MODEL_ENVMAPPED) {
    for (i = 0; i < num_triangles; i++) {
      generate_envmap_coords(&transformed_list[i].vertices[0], m);
      generate_envmap_coords(&transformed_list[i].vertices[1], m);
      generate_envmap_coords(&transformed_list[i].vertices[2], m);
    }
  }
  
  for (i = 0; i < num_triangles; i++)
    draw_order[i] = i;

  quick_sort(draw_order, 0, num_triangles);

  uint16_t triangle_index, triangle_size;
  if (model->flags & MODEL_NOCLIP) {
   for (i = 0; i < num_triangles; i++) {
      triangle_index = draw_order[i];

      clip_buffer[0] = transformed_list[triangle_index].vertices[0];
      clip_buffer[1] = transformed_list[triangle_index].vertices[1];
      clip_buffer[2] = transformed_list[triangle_index].vertices[2];

      uint16_t *texture = (uint16_t*)texture_list[transformed_list[triangle_index].texture_index];
      texture_polygon(((vertex_t*)&transformed_list[triangle_index]), 3, texture);
    }    
  }
  else {
    for (i = 0; i < num_triangles; i++) {
      triangle_index = draw_order[i];

      clip_buffer[0] = transformed_list[triangle_index].vertices[0];
      clip_buffer[1] = transformed_list[triangle_index].vertices[1];
      clip_buffer[2] = transformed_list[triangle_index].vertices[2];

      uint8_t num_vertices = sutherland_hodgman_clipping((vertex_t *)&clip_buffer, 3);

      /* the clipper sometimes generates large polygons that cut through the near-plane, so this hack prevents those from being drawn */
      triangle_size = abs(clip_buffer[0].position.x * (clip_buffer[1].position.y - clip_buffer[2].position.y) + 
                          clip_buffer[1].position.x * (clip_buffer[2].position.y - clip_buffer[0].position.y) + 
                          clip_buffer[2].position.x * (clip_buffer[0].position.y - clip_buffer[1].position.y));

      if (num_vertices >= 3 && triangle_size < 4000) {
        uint16_t *texture = (uint16_t*)texture_list[transformed_list[triangle_index].texture_index];
        
        texture_polygon(clip_buffer, num_vertices, texture);
        //wire_polygon(clip_buffer, num_vertices, texture);
      }
    }
  }
}
