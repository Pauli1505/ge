#ifndef FREECAM_H
#define FREECAM_H

#include <SDL2/SDL.h>
#include "matrix.h" // Ensure this has the matrix definitions

class FreeCam {
public:
    FreeCam(float fov, float aspect, float near, float far);
    
    void handleInput(const SDL_Event& event);
    void update(float deltaTime);
    
    mat4f getViewMatrix() const;
    mat4f getProjectionMatrix() const;

private:
    vec3f position;
    vec3f front;
    vec3f up;
    vec3f right;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float fov;
    float aspect;
    float near;
    float far;
    bool rightMouseHeld;
};

#endif // FREECAM_H
