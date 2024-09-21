#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "matrix.h"
#include "vector.h"

struct Context {
    unsigned int shader_program;
    unsigned int vao;
    SDL_Window* window;
    SDL_GLContext gl_context;
    unsigned int uniform_transform;
    int window_width = 800;
    int window_height = 800;
    bool is_fullscreen = false;
    bool freeCamMode = true;  // Start in FreeCam mode
    float cubeRotationAngle = 0.0f;  // Track cube's rotation angle
};

class FreeCam {
public:
    FreeCam(float fov, float aspect, float near, float far)
        : position({0.0f, 0.0f, 5.0f}), front({0.0f, 0.0f, -1.0f}), up({0.0f, 1.0f, 0.0f}),
          yaw(-90.0f), pitch(0.0f), speed(2.5f), sensitivity(0.1f),
          fov(fov), aspect(aspect), near(near), far(far), rightMouseHeld(false) {
        right = vec3f_cross(front, up);
    }

    void handleInput(const SDL_Event& event) {
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
            rightMouseHeld = true;
            SDL_SetRelativeMouseMode(SDL_TRUE); // Capture mouse
        }
        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
            rightMouseHeld = false;
            SDL_SetRelativeMouseMode(SDL_FALSE); // Release mouse
        }
        if (event.type == SDL_MOUSEMOTION && rightMouseHeld) {
            float xOffset = event.motion.xrel * sensitivity;
            float yOffset = event.motion.yrel * sensitivity;

            yaw -= xOffset;
            pitch -= yOffset;

            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            vec3f direction;
            direction.x = cosf(yaw * pi / 180.0f) * cosf(pitch * pi / 180.0f);
            direction.y = sinf(pitch * pi / 180.0f);
            direction.z = sinf(yaw * pi / 180.0f) * cosf(pitch * pi / 180.0f);
            front = vec3f_normalize(direction);

            right = vec3f_normalize(vec3f_cross(front, up));
            up = vec3f_cross(right, front);
        }
    }

    void update(float deltaTime) {
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        float velocity = speed * deltaTime;
        if (state[SDL_SCANCODE_W]) position = vec3f_add(position, vec3f_scale(front, velocity));
        if (state[SDL_SCANCODE_S]) position = vec3f_subtract(position, vec3f_scale(front, velocity));
        if (state[SDL_SCANCODE_D]) position = vec3f_subtract(position, vec3f_scale(right, velocity));
        if (state[SDL_SCANCODE_A]) position = vec3f_add(position, vec3f_scale(right, velocity));
    }

    mat4f getViewMatrix() const {
        return mat4f_look_at(position, vec3f_add(position, front), up);
    }

    mat4f getProjectionMatrix() const {
        return mat4f_perspective(fov, aspect, near, far);
    }

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

void render(Context& context, FreeCam& freeCam);
void initialize(Context& context);
void toggle_fullscreen(Context& context);
void read_file(const char* path, std::string& content);
void compile_shader_from_file(const char* path, GLuint shader);
void link_shader_program(unsigned int program);
void update_fps(Context& context);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cube", 100, 100, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed." << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Context context;
    context.window = window;
    context.gl_context = gl_context;
    initialize(context);

    FreeCam freeCam(45.0f, 800.0f / 800.0f, 0.1f, 100.0f);

    Uint32 lastTime = SDL_GetTicks();
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    toggle_fullscreen(context);
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_F11) {
                    context.freeCamMode = !context.freeCamMode;
                    if (!context.freeCamMode) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                }
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    context.window_width = event.window.data1;
                    context.window_height = event.window.data2;
                    glViewport(0, 0, context.window_width, context.window_height);
                    freeCam = FreeCam(45.0f, (float)context.window_width / (float)context.window_height, 0.1f, 100.0f);
                }
            }

            if (context.freeCamMode) {
                freeCam.handleInput(event);
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (context.freeCamMode) {
            freeCam.update(deltaTime);
        } else {
            context.cubeRotationAngle += 50.0f * deltaTime;
            if (context.cubeRotationAngle > 360.0f) {
                context.cubeRotationAngle -= 360.0f;
            }
        }

        render(context, freeCam);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void render(Context& context, FreeCam& freeCam) {
    update_fps(context);

    glClearColor(0.1f, 0.12f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(context.shader_program);

    mat4f transform;

    if (context.freeCamMode) {
        mat4f view = freeCam.getViewMatrix();
        mat4f projection = freeCam.getProjectionMatrix();
        transform = mat4f_multiply(projection, view);
    } else {
        // Set the camera position outside the cube for rotation mode
        vec3f cameraPos = {0.0f, 0.0f, 5.0f};  // Adjust the z position as needed
        mat4f view = mat4f_look_at(cameraPos, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        mat4f projection = mat4f_perspective(45.0f, (float)context.window_width / (float)context.window_height, 0.1f, 100.0f);

        // Combine projection and view matrices
        transform = mat4f_multiply(projection, view);

        // Prepare the model matrix for the cube
        mat4f model = mat4f_identity();
        mat4f rotation = mat4f_rotate_y(context.cubeRotationAngle * pi / 180.0f);
        model = mat4f_multiply(model, rotation);

        // Combine model with the transform
        transform = mat4f_multiply(transform, model);
    }

    glUniformMatrix4fv(context.uniform_transform, 1, GL_FALSE, mat4f_gl(&transform));
    glBindVertexArray(context.vao);

    unsigned short indices[] = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        5, 4, 7,
        7, 6, 5,
        4, 0, 3,
        3, 7, 4,
        1, 5, 4,
        4, 0, 1,
        3, 2, 6,
        6, 7, 3,
    };

    int numIndices = sizeof(indices) / sizeof(indices[0]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);
}


void initialize(Context& context) {
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
    };

    float vertex_colors[] = {
        1.0f, 0.4f, 0.6f,
        1.0f, 0.9f, 0.2f,
        0.7f, 0.3f, 0.8f,
        0.5f, 0.3f, 1.0f,
        0.2f, 0.6f, 1.0f,
        0.6f, 1.0f, 0.3f,
        0.3f, 0.1f, 0.2f,
        0.1f, 0.8f, 0.5f,
    };

    unsigned short indices[] = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        5, 4, 7,
        7, 6, 5,
        4, 0, 3,
        3, 7, 4,
        1, 5, 4,
        4, 0, 1,
        3, 2, 6,
        6, 7, 3,
    };

    unsigned int vbo, ebo, color_vbo;

    glGenVertexArrays(1, &context.vao);
    glBindVertexArray(context.vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    compile_shader_from_file("/usr/local/games/shaders/vertex.glsl", vertex_shader);
    compile_shader_from_file("/usr/local/games/shaders/fragment.glsl", fragment_shader);

    context.shader_program = glCreateProgram();
    glAttachShader(context.shader_program, vertex_shader);
    glAttachShader(context.shader_program, fragment_shader);
    link_shader_program(context.shader_program);

    context.uniform_transform = glGetUniformLocation(context.shader_program, "transform");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void toggle_fullscreen(Context& context) {
    context.is_fullscreen = !context.is_fullscreen;
    if (context.is_fullscreen) {
        SDL_SetWindowFullscreen(context.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(context.window, 0);
    }
}

void read_file(const char* path, std::string& content) {
    std::ifstream file(path);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
    } else {
        std::cerr << "Failed to read file: " << path << std::endl;
    }
}

void compile_shader_from_file(const char* path, GLuint shader) {
    std::string source;
    read_file(path, source);
    const char* source_c_str = source.c_str();
    glShaderSource(shader, 1, &source_c_str, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}

void link_shader_program(unsigned int program) {
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }
}

void update_fps(Context& context) {
    static Uint32 last_time = SDL_GetTicks();
    static int frames = 0;
    Uint32 current_time = SDL_GetTicks();
    frames++;

    if (current_time - last_time >= 1000) {
        float fps = frames / ((current_time - last_time) / 1000.0f);
        std::string title = "Cube - FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(context.window, title.c_str());

        frames = 0;
        last_time = current_time;
    }
}
