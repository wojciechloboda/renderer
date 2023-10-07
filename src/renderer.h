#pragma once

#include <memory>
#include <vector>
#include "scene.h"
#include "glad/glad.h"
#include "scene.h"
#include "glslprogram.h"
#include "texture.h"
#include "object.h"
#include "model.h"

class Renderer 
{
public:
    void render(Scene *scene, int width, int height)
    {
        float aspect = (float)width / (float)height;
        vmath::mat4 proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

        static const GLfloat red[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        vmath::mat4 view_matrix = vmath::lookat(
           scene->mainCamera.position, 
           scene->mainCamera.position + scene->mainCamera.cameraFront,
           scene->mainCamera.upDirection);
        
        //cube with lightning
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearBufferfv(GL_COLOR, 0, red);

        //common
        
        rendering_program->useProgram();
        rendering_program->setUniform("view", view_matrix);
        rendering_program->setUniform("projection", proj_matrix);
        rendering_program->setUniform("light_position", scene->pointLightPosition);
        rendering_program->setUniform("view_position", scene->mainCamera.position);
        //

        for(auto& object : scene->objects)
        {
            rendering_program->setUniform("model", 
                model_matrix(object.position, object.scale));
            rendering_program->setUniform("material.ambient", object.ambient);
            rendering_program->setUniform("material.diffuse", object.diffuse);
            rendering_program->setUniform("material.specular", object.specular);
            rendering_program->setUniform("material.shininess", object.shininess);
            object.draw();
        }

        //lightning
        light_program->useProgram();
        light_program->setUniform("view", view_matrix);
        light_program->setUniform("projection", proj_matrix);
        light_program-> setUniform("model", model_matrix(cube->position, cube->scale));
        cube->draw();  
        //
    }

    void init()
    {
        rendering_program = std::make_unique<GLSLProgram>();
        light_program = std::make_unique<GLSLProgram>();

        sphereTexture = std::make_shared<Texture>("resources/textures/SmileyFace.jpg");

        rendering_program->attachVertexShader("src/shaders/vertex_shader.vert");
        rendering_program->attachFragmentShader("src/shaders/fragment_shader.frag");
        rendering_program->linkProgram();

        light_program->attachVertexShader("src/shaders/vertex_shader.vert");
        light_program->attachFragmentShader("src/shaders/light_fragment_shader.frag");
        light_program->linkProgram();

        auto sphereModel = std::make_shared<Model>("resources/meshes/DirtMetal.obj");
        auto cubeModel = std::make_shared<Model>("resources/meshes/cube.obj");

        cube = std::make_unique<Object>(cubeModel);
        cube->position = vmath::vec3(0.7f, 0.0f, 0.0f);
        cube->scale = vmath::vec3(0.05f, 0.05f, 0.05f);
    }

private:
    float currentFrame;
    float lastFrame;
    float deltaTime;
    std::unique_ptr<Object> cube;
    std::shared_ptr<Texture> sphereTexture;
    std::unique_ptr<GLSLProgram> rendering_program;
    std::unique_ptr<GLSLProgram> light_program;

    vmath::mat4 create_model_view_matrix(float x, float y, float z, float scale)
    {
        return vmath::translate(x, y, z) *
                vmath::scale(scale, scale, scale);
    }

    vmath::mat4 model_matrix(vmath::vec3 position, vmath::vec3 scale)
    {
        return vmath::translate(position) *
                vmath::scale(scale);
    }

    vmath::mat4 create_view_matrix(vmath::vec3 camera_position)
    {
        return vmath::lookat(camera_position, 
                             vmath::vec3(0.0, 0.0, 0.0),
                             vmath::vec3(0.0, -1.0, 0.0));
    }
};