#include "rect.h"
#include "../util/color.h"

Rect::Rect(Shader & shader, vec2 pos, vec2 size, struct color color)
        : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::Rect(Rect const& other) : Shape(other) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::~Rect() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Rect::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Rect::initVectors() {
    this->vertices.insert(vertices.end(), {
            // Add other three corners here
            // Note: the order of the corners matters because the two triangles
            //    formed below from the indices of these corners must fill the 
            //    rectangle
            0.5f, -0.5f,  // x, y of bottom right corner
            -0.5f, -0.5f, // x, y of bottom left corner
            0.5f, 0.5f, // x, y of top right corner
            -0.5f, 0.5f //x, y of top left corner
    });

    this->indices.insert(indices.end(), {
            0, 1, 2, // First triangle
            1, 3, 2  // Second triangle
    });
}

// Overridden Getters from Shape
float Rect::getLeft() const        { return pos.x - (size.x / 2); }
float Rect::getRight() const       { return pos.x + (size.x / 2); }
float Rect::getTop() const         { return pos.y + (size.y / 2); }
float Rect::getBottom() const      { return pos.y - (size.y / 2); }
