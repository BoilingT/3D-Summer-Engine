#pragma once
#include "Object.h"
class Plane :
    public Object
{
private:
    float planeVertices[4*3] = {
        //Positions
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };

    unsigned int indices[6] = {
        0, 1, 2,
        1, 2, 3
    };

public:

    Plane(glm::vec3 pos, glm::vec3 dim, glm::vec3 rot) {
        transform.pos = pos;
        transform.dim = dim;
        transform.rot = rot;
        SetupMesh(planeVertices, sizeof(planeVertices), indices, sizeof(indices));
    }
};

