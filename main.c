#include "raylib.h"
#define CAMERA_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_COLUMNS 20
#define G 6.67e-11
#define CENTER_OF_MASS_AT_ORIGIN

#define VECTOR_3_ZERO (Vector3){ 0 }


static inline double cube(double n) {
  return n*n*n;
}


static inline double square(double n) {
  return n*n;
}


Vector3 Vector3dScale(Vector3 v, double scale) {
  return (Vector3){v.x*scale, v.y*scale, v.z*scale};
}


double Vector3dDistance(Vector3 v1, Vector3 v2) {
  return sqrtf(square(v1.x - v2.x) + square(v1.y - v2.y) + square(v1.z - v2.z));
}


Vector3 Vector3dAdd(Vector3 v1, Vector3 v2) {
  return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}


Vector3 Vector3dSubtract(Vector3 v1, Vector3 v2) {
  return (Vector3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}


typedef struct {
  bool active;
  double mass;
  double radius;
  Vector3 position;
  Vector3 velocity;
  Color color;
} Celestial_Body;


size_t get_objects(Celestial_Body bodies[]) {
  size_t last_index = 0;

  FILE *csv_file = fopen("index.csv", "r");
  char* line = NULL;
  size_t n = 1024;

  if (!csv_file) {
    perror("Error openning file");
    return -1;
  }

  getline(&line, &n, csv_file);   // Gets rid of the first line
  // I decided to use this function because I didn't want to use many lines to use the strtok and the error verification
  #define continue_and_verify() {token = strtok(NULL, ",");if (!token) {perror("\033[31mSomething's wrong with your config file\033[0m"); break;}}

  while (getline(&line, &n, csv_file) != -1) {
    bodies[last_index].color.a = 255;
    bodies[last_index].active = true;

    char *token;
    token = strtok(line, ",");
    bodies[last_index].position.x = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].position.y = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].position.z = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].velocity.x = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].velocity.y = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].velocity.z = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].mass = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].radius = strtod(token, NULL);
    continue_and_verify();
    bodies[last_index].color.r = strtof(token, NULL);
    continue_and_verify();
    bodies[last_index].color.g = strtof(token, NULL);
    continue_and_verify();
    bodies[last_index].color.b = strtof(token, NULL);

    last_index++;
  }
  free(line);
  fclose(csv_file);
  return last_index-1;
}


bool destroy(Celestial_Body* body1, Celestial_Body* body2) {
  if (body1->radius + body2->radius > Vector3dDistance(body1->position, body2->position) && body1->active && body2->active) {
    body2->active = false;
    body1->radius = cbrtf(cube(body1->radius) + cube(body2->radius)); // Formula cbrt(R³+r³)
    body1->velocity = Vector3dScale(
                        Vector3dAdd(
                          Vector3dScale(
                            body1->velocity, body1->mass), 
                          Vector3dScale(
                            body2->velocity, body2->mass)
                        ),
                        1/(body1->mass + body2->mass)
                      );
    body1->mass += body2->mass;
    body1->position = Vector3dScale(
                        Vector3dAdd(
                          Vector3dScale(
                            body1->position, body1->mass), 
                          Vector3dScale(
                            body2->position, body2->mass)
                        ),
                        1/(body1->mass + body2->mass)
                      );
    body1->color = (Color){(body1->color.r + body2->color.r)/2, (body1->color.g + body2->color.g)/2, (body1->color.b + body2->color.b)/2, 255};
    return true;
  }
  return false;
}

// It'll run once, so I decided to use "static inline"
static inline Vector3 create_center_of_mass(Celestial_Body bodies[], size_t size_of_bodies) {
  Vector3 center_of_mass = VECTOR_3_ZERO;
  double sum_of_masses = 0;
  for (int i = 0; i < size_of_bodies; i++) {
    if (!bodies[i].radius) {
      bodies[i].active = false;
    }
    if (bodies[i].active){
      center_of_mass = Vector3dAdd(center_of_mass, Vector3dScale(bodies[i].position, bodies[i].mass));
      sum_of_masses += bodies[i].mass;
    }
  }
  center_of_mass = Vector3dScale(center_of_mass, 1/sum_of_masses);
}


void do_gravity(Celestial_Body bodies[], size_t array_size, unsigned short time_speed) {
  Celestial_Body old_bodies[array_size];
  for (int k=0; k < time_speed; k++) {
    // Passes the value of bodies to a new variable, because, for exemple, if object A was moved in the x axis by 1, when calculating it's distance to object B, the distance will be affected by this movement, which will make the simulation imprecise.
    memcpy(old_bodies, bodies, array_size * sizeof(Celestial_Body));

    for (int i = 0; i < array_size; i++) {
      if (bodies[i].active) {
        for (int j = 0; j < array_size; j++) {
          if ((i != j) && !destroy(&bodies[i], &bodies[j]) && bodies[j].active) {
            double distance = Vector3dDistance(bodies[i].position, old_bodies[j].position);
            double general_speed = 0;
            if (distance) {
              general_speed = old_bodies[j].mass/cube(distance);
            }

            bodies[i].velocity = Vector3dSubtract(bodies[i].velocity, Vector3dScale(Vector3dSubtract(bodies[i].position, old_bodies[j].position), general_speed*GetFrameTime()*G));
          }
        }
        bodies[i].position = Vector3dAdd(bodies[i].position, bodies[i].velocity);
      }
    }
  }
}


int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Gravity");
  
  Celestial_Body bodies[255];
  size_t size_of_bodies = get_objects(bodies)*sizeof(Celestial_Body);
  if (size_of_bodies < 0) {
    return 1;
  }

  Camera camera = { 0 };
  camera.position = (Vector3){ 50.0f, 50.0f, 0.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
  camera.up = (Vector3){ 0.0f, 2.0f, 0.0f };
  camera.fovy = 80.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Vector3 center_of_mass = create_center_of_mass(bodies, size_of_bodies);

  unsigned short time_speed = 1;

  DisableCursor();

  SetTargetFPS(180);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_J)) {time_speed += 1;}
    if (IsKeyPressed(KEY_K) && time_speed != 0) {time_speed -= 1;}

    do_gravity(bodies, size_of_bodies, time_speed);
    UpdateCamera(&camera, CAMERA_FREE);
    
    BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode3D(camera);

        Vector3 new_center_of_mass = VECTOR_3_ZERO;
        double sum_of_masses = 0;
        for (int i = 0; i < size_of_bodies; i++) {
          if (bodies[i].active){
            #ifdef CENTER_OF_MASS_AT_ORIGIN
              bodies[i].position = Vector3dSubtract(bodies[i].position, center_of_mass);
            #endif
            DrawSphere(bodies[i].position, bodies[i].radius, bodies[i].color);

            new_center_of_mass = Vector3dAdd(new_center_of_mass, Vector3dScale(bodies[i].position, bodies[i].mass));
            sum_of_masses += bodies[i].mass;
          }
        }
        center_of_mass = Vector3dScale(new_center_of_mass, 1/sum_of_masses);

      EndMode3D();
      DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
