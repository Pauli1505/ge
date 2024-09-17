#include "freecam.h"
#include <SDL2/SDL.h> // Include SDL header

// Implement the FreeCam methods
void FreeCam::handleInput(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        float xOffset = event.motion.xrel * sensitivity;
        float yOffset = event.motion.yrel * sensitivity;

        yaw += xOffset;
        pitch -= yOffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        vec3f direction;
        direction.x = cosf(yaw * pi / 180.0f) * cosf(pitch * pi / 180.0f);
        direction.y = sinf(pitch * pi / 180.0f);
        direction.z = sinf(yaw * pi / 180.0f) * cosf(pitch * pi / 180.0f);
        front = vec3f_normalize(direction);

        right = vec3f_normalize(vec3f_cross(front, {0.0f, 1.0f, 0.0f}));
        up = vec3f_cross(right, front);
    }
}

void FreeCam::update(float deltaTime) {
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    float velocity = speed * deltaTime;
    if (state[SDL_SCANCODE_W]) position = vec3f_add(position, vec3f_scale(front, velocity));
    if (state[SDL_SCANCODE_S]) position = vec3f_subtract(position, vec3f_scale(front, velocity));
    if (state[SDL_SCANCODE_A]) position = vec3f_subtract(position, vec3f_scale(right, velocity));
    if (state[SDL_SCANCODE_D]) position = vec3f_add(position, vec3f_scale(right, velocity));
}
