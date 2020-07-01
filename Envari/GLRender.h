#if !defined(GLRENDER_H)
#define GLRENDER_H

#include "Defines.h"
#ifdef GL_PROFILE_GLES3
#include <GLES3/gl3.h>

const char *vertexColored = "#version 300 es\n"
    "layout (location = 0) in vec3 Position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);\n"
    "}\0";

const char *fragmentColored = "#version 300 es\n"
    "precision mediump float;\n"
    "uniform vec4 color;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(color.r, color.g, color.b, color.a);\n"
    "}\0";

#else
#include <gl/gl.h>

const char *vertexColored = "#version 300 es\n"
    "layout (location = 0) in vec3 Position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);\n"
    "}\0";

const char *fragmentColored = "#version 300 es\n"
    "precision mediump float;\n"
    "uniform vec4 color;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(color.r, color.g, color.b, color.a);\n"
    "}\0";

const char *vertexTextured = "#version 300 es\n"
    "layout (location = 0) in vec3 Position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);\n"
    "}\0";

const char *fragmentTextured = "#version 300 es\n"
    "precision mediump float;\n"
    "uniform vec4 color;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(color.r, color.g, color.b, color.a);\n"
    "}\0";

#endif


u32 glVertexBuffer;
u32 glIndexBuffer;
u32 glVertexArray;
unsigned int coloredProgram;
unsigned int texturedProgram;

m44 model;
m44 view;
m44 projection;

static void GLInit()
{
    glGenBuffers(1, &glVertexBuffer);
    glGenBuffers(1, &glIndexBuffer);
    
    glGenVertexArrays(1, &glVertexArray);
}

static int GLCompileProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{

    // NOTE(Juan): Shaders
    u32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    i32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        // cout << "ERROR::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    u32 fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        // cout << "ERROR::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        // cout << "ERROR::PROGRAM::COMPILATION_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}

static void GLRender()
{
    glViewport(0, 0, gameState->screen.width, gameState->screen.height);

    RenderHeader *renderHeader = (RenderHeader *)renderTemporaryMemory.arena->base;

    model = lua["model"];
    view = lua["view"];
    view._03 = 0.2f;
    view._13 = 0.2f;
    view._23 = 0.2f;
    projection = lua["projection"];

    while(renderHeader->id > 0) {        
        int size = 0;
        // #TODO #PERFORMANCE (Juan): This could be improved by generating some geometries on init time (rectangles and circles?)
        // #TODO (Juan): More render types can be added line, spline, etc
        switch(renderHeader->type) {
            case type_RenderClear: {
                RenderClear *clear = (RenderClear *)renderHeader;
                glViewport(0, 0, gameState->screen.width, gameState->screen.height);
                glClearColor(clear->color.r, clear->color.g, clear->color.b, clear->color.a);
                glClear(GL_COLOR_BUFFER_BIT);
                size = sizeof(RenderClear);
                break;
            }
            case type_RenderColor: {
                RenderColor *color = (RenderColor *)renderHeader;
                renderState.renderColor = color->color;
                size = sizeof(RenderColor);
                break;
            }
            case type_RenderTriangle: {
                RenderTriangle *triangle = (RenderTriangle *)renderHeader;
                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.E);
                glUniformMatrix4fv(viewLocation, 1, false, view.E);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.E);

                f32 vertices[] = {
                    triangle->position.x + triangle->point1.x, triangle->position.y + triangle->point1.y, 0.0f,
                    triangle->position.x + triangle->point2.x, triangle->position.y + triangle->point2.y, 0.0f,
                    triangle->position.x + triangle->point3.x, triangle->position.y + triangle->point3.y, 0.0f
                };

                u32 indices[] = {
                    0, 1, 2
                };

                glBindVertexArray(glVertexArray);
                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glUseProgram(coloredProgram);
                glDrawArrays(GL_TRIANGLES, 0, 3);

                glBindVertexArray(0);

                size = sizeof(RenderTriangle);
                break;
            }
            case type_RenderRectangle: {
                RenderRectangle *rectangle = (RenderRectangle *)renderHeader;
                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.E);
                glUniformMatrix4fv(viewLocation, 1, false, view.E);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.E);

                f32 vertices[] = {
                    rectangle->position.x, rectangle->position.y, 0.0f,
                    rectangle->position.x + rectangle->size.x, rectangle->position.y, 0.0f,
                    rectangle->position.x, rectangle->position.y + rectangle->size.y, 0.0f,
                    rectangle->position.x + rectangle->size.x, rectangle->position.y + rectangle->size.y, 0.0f
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glUseProgram(coloredProgram);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                glBindVertexArray(0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderCircle: {
                RenderCircle *circle = (RenderCircle *)renderHeader;
                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.E);
                glUniformMatrix4fv(viewLocation, 1, false, view.E);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.E);

                f32 vertices[300] = {
                    circle->position.x, circle->position.y, 0.0f,
                };

                u32 indices[300] = {

                };

                for(int i = 0; i < circle->segments; ++i) {
                    float angle = ((float)i / (float)circle->segments) * PI32 * 2;
                    vertices[i * 3 + 3] = circle->position.x + Sin(angle) * circle->radius;
                    vertices[i * 3 + 4] = circle->position.y + Cos(angle) * circle->radius;
                    vertices[i * 3 + 5] = 0.0f;

                    indices[i * 3 + 0] = 0;
                    indices[i * 3 + 1] = i;
                    indices[i * 3 + 2] = i + 1;
                }

                int lastSegment = circle->segments - 1;
                indices[circle->segments * 3 + 0] = 0;
                indices[circle->segments * 3 + 1] = circle->segments;
                indices[circle->segments * 3 + 2] = 1;

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glUseProgram(coloredProgram);
                glDrawElements(GL_TRIANGLES, (circle->segments + 1) * 3, GL_UNSIGNED_INT, 0);
                
                glBindVertexArray(0);

                size = sizeof(RenderRectangle);
                break;
            }
            case type_RenderImage: {
                RenderImage *image = (RenderImage *)renderHeader;
                i32 colorLocation = glGetUniformLocation(coloredProgram, "color");
                i32 modelLocation = glGetUniformLocation(coloredProgram, "model");
                i32 viewLocation = glGetUniformLocation(coloredProgram, "view");
                i32 projectionLocation = glGetUniformLocation(coloredProgram, "projection");
                glUniformMatrix4fv(modelLocation, 1, false, model.E);
                glUniformMatrix4fv(viewLocation, 1, false, view.E);
                glUniformMatrix4fv(projectionLocation, 1, false, projection.E);

                f32 vertices[] = {
                    image->position.x, image->position.y, 0.0f, 0.0f, 0.0f,
                    image->position.x + image->size.x, image->position.y, 1.0f, 0.0f, 0.0f,
                    image->position.x, image->position.y + image->size.y, 0.0f, 1.0f, 0.0f,
                    image->position.x + image->size.x, image->position.y + image->size.y, 0.0f, 1.0f, 1.0f
                };

                u32 indices[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                glBindVertexArray(glVertexArray);

                glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

                glVertexAttribPointer(0, 5, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform4f(colorLocation, renderState.renderColor.r, renderState.renderColor.g, renderState.renderColor.b, renderState.renderColor.a);

                glUseProgram(coloredProgram);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
                glBindVertexArray(0);
                
                size = sizeof(RenderImage);
                break;
            }
            default: {
                InvalidCodePath;
                break;
            }
        }

        Assert(size > 0)
        renderHeader = (RenderHeader *)((u8 *)renderHeader + size);
    }
}

static void GLEnd()
{
    glDeleteBuffers(1, &glVertexBuffer);
    glDeleteBuffers(1, &glIndexBuffer);
}

#endif