#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "matrix.h"

struct Context {
    unsigned int shader_program;
    unsigned int vao;
    SDL_Window* window;
    SDL_GLContext gl_context;
    unsigned int uniform_transform;
};

void render(Context&);
void initialize(Context&);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cube", 100, 100, 800, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); // make window 800x800 res
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

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        render(context);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void read_file(const char* path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader: (make sure you have all shaders. .GLSL)" << path << std::endl; // what the fuk
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
}

void compile_shader_from_file(const char* path, GLuint shader) {
    std::string source;
    read_file(path, source);
    const char* source_c_str = source.c_str();

    glShaderSource(shader, 1, &source_c_str, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char info_buffer[1024];
        glGetShaderInfoLog(shader, sizeof(info_buffer), nullptr, info_buffer);
        std::cerr << "Error compiling shader: " << info_buffer << "\nThe shader was:\n" << source << std::endl; // log shader errors if any
    }
}

void link_shader_program(unsigned int program) {
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char info_buffer[1024];
        glGetProgramInfoLog(program, sizeof(info_buffer), nullptr, info_buffer);
        std::cerr << "Error linking shader program: " << info_buffer << std::endl; // error linking shader to app?
    }
}

const unsigned int triangles = 6 * 2;   // Number of triangles rendered

const unsigned int vertices_index = 0;
const unsigned int colors_index = 1;

void initialize(Context& context) {
    glEnable(GL_DEPTH_TEST);

    float vertices[] = { // cube
        // Front face
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

        // Back face
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
        0.6f, 1.0f, 0.4f,
        0.6f, 0.8f, 0.8f,
        0.4f, 0.8f, 0.8f,
    };

    unsigned short triangle_indices[] = {
        // Front
        0, 1, 2,
        2, 3, 0,

        // Right
        0, 3, 7,
        7, 4, 0,

        // Bottom
        2, 6, 7,
        7, 3, 2,

        // Left
        1, 5, 6,
        6, 2, 1,

        // Back
        4, 7, 6,
        6, 5, 4,

        // Top
        5, 1, 0,
        0, 4, 5,
    };

    glGenVertexArrays(1, &context.vao);
    glBindVertexArray(context.vao);

    unsigned int triangles_ebo;
    glGenBuffers(1, &triangles_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    unsigned int vertices_vbo;
    glGenBuffers(1, &vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(vertices_index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(vertices_index);

    unsigned int colors_vbo;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(colors_index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(colors_index);

    // Unbind to prevent accidental modification
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    compile_shader_from_file("vertex.glsl", vertex_shader);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader_from_file("fragment.glsl", fragment_shader); // call shaders

    context.shader_program = glCreateProgram();
    glAttachShader(context.shader_program, vertex_shader);
    glAttachShader(context.shader_program, fragment_shader);
    link_shader_program(context.shader_program);

    context.uniform_transform = glGetUniformLocation(context.shader_program, "transform");
}

void update_fps(Context& context) {
    static double last_update_time = 0;
    static int frames_since_last_update = 0;

    double now = SDL_GetTicks() / 1000.0;
    frames_since_last_update++;

    if (now - last_update_time > 0.25) {
        double fps = frames_since_last_update / (now - last_update_time);

        std::string title = "Cube (" + std::to_string(fps) + " FPS)"; // show fps precisely
        SDL_SetWindowTitle(context.window, title.c_str());

        last_update_time = now;
        frames_since_last_update = 0;
    }
}

float animation(float duration) {
    Uint32 ms_time = SDL_GetTicks();
    Uint32 ms_duration = duration * 1000;
    float ms_position = ms_time % ms_duration;

    return ms_position / ms_duration;
}

void render(Context& context) {
    update_fps(context);

    // Clear
    glClearColor(0.1f, 0.12f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(context.shader_program);

    mat4f transform = mat4f_identity();
    transform = mat4f_multiply(transform, mat4f_perspective());
    transform = mat4f_multiply(transform, mat4f_translation(0, 0, -3));
    transform = mat4f_multiply(transform, mat4f_rotate_x(0.15f * pi));
    transform = mat4f_multiply(transform, mat4f_rotate_y(2 * pi * animation(4)));
    glUniformMatrix4fv(context.uniform_transform, 1, GL_FALSE, mat4f_gl(&transform));

    glBindVertexArray(context.vao);
    glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_SHORT, nullptr);
}

// goodbye, world!
