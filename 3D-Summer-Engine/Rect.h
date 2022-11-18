#pragma once
#include "Object.h"
#include <glad/glad.h>

class Rect :
    public Object
{
private:
    float planeVertices[4 * 5] = {
        //Positions              //TexCoords
        -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,     //Top left corner
        -1.0f, -1.0f, 0.0f,      0.0f, 0.0f,     //Lower left corner
        1.0f, 1.0f, 0.0f,        1.0f, 1.0f,     //Top right corner
        1.0f, -1.0f, 0.0f,       1.0f, 0.0f      //Lower right corner
    };

    unsigned int indices[6] = {
        0, 1, 2,
        1, 2, 3
    };

public:

    Rect() {
        transform.pos = glm::vec3(0.0f);
        transform.dim = glm::vec3(1.0f);
        transform.rot = glm::vec3(0.0f);
        SetupMesh(planeVertices, sizeof(planeVertices), indices, sizeof(indices));
    }

    Rect(glm::vec3 pos, glm::vec3 dim, glm::vec3 rot) {
        transform.pos = pos;
        transform.dim = dim;
        transform.rot = rot;
        SetupMesh(planeVertices, sizeof(planeVertices), indices, sizeof(indices));
    }

    Rect(glm::vec3 pos, glm::vec3 dim, glm::vec3 rot, GLuint* texture2D) {
        transform.pos = pos;
        transform.dim = dim;
        transform.rot = rot;
        texture = texture2D;
        SetupMesh(planeVertices, sizeof(planeVertices), indices, sizeof(indices));
    }

    void setTexture(GLuint* texture2D) {
        texture = texture2D;
    }

    void updateMesh() {
        SetupMesh(planeVertices, sizeof(planeVertices), indices, sizeof(indices));
    }
};

