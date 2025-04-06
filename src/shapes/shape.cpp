#include "shape.h"

Shape::Shape(Shader &shader, glm::vec2 pos, glm::vec2 size, struct color color) :
        shader(shader), pos(pos), size(size), color(color) {}

Shape::Shape(Shape const& other) :
        shader(other.shader), pos(other.pos), size(other.size), color(other.color) {}

// Initialize VAO
unsigned int Shape::initVAO() {
    glGenVertexArrays(1, &VAO); // Generate VAO
    glBindVertexArray(VAO); // Bind VAO
    return VAO;
}

// Initialize VBO
void Shape::initVBO() {
    // Generate VBO, bind it to VAO, and copy vertices data into it
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers (2 floats per vertex (x, y))
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
}

// Initialize EBO
void Shape::initEBO() {
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
    // Don't unbind EBO because it's bound to VAO
}

void Shape::setUniforms() const {
    // If you want to use a custom shader, you have to set it and call it's Use() function here.
    // Since we are using the same shader for all shapes, we can just set it once in the constructor.
    //this->shader.use();

    // Define the model matrix for the shape as a 4x4 identity matrix
    mat4 model = mat4(1.0f);
    // The model matrix is used to transform the vertices of the shape in relation to the world space.
    model = translate(model, vec3(pos, 1.0f));
    // The size of the shape is scaled by the model matrix to make the shape larger or smaller.
    model = scale(model, vec3(size, 1.0f));

    // Set the model matrix and color uniform variables in the shader
    this->shader.setMatrix4("model", model);
    this->shader.setVector4f("shapeColor", color.vec);
}

bool Shape::isOverlapping(const vec2 &point) const {
    // A shape is overlapping a point if the point is within the shape's bounding box.
    // in this class, you can still call them from here.
    return this->getTop() > point.y && this->getBottom() < point.y && this->getLeft() < point.x && this->getRight() > point.x;
}

// Setters
void Shape::move(vec2 offset)         { pos += offset; }
void Shape::moveX(float x)            { pos.x += x; }
void Shape::moveY(float y)            { pos.y += y; }
void Shape::setPos(vec2 pos)          { this->pos = pos; }
void Shape::setPosX(float x)          { pos.x = x; }
void Shape::setPosY(float y)          { pos.y = y; }

void Shape::setColor(struct color c)    { color = c; }
void Shape::setColor(vec4 c)     { color.vec = c; }
void Shape::setColor(vec3 c)     { color.vec = vec4(c, 1.0); }
void Shape::setRed(float r)      { color.red = r; }
void Shape::setGreen(float g)    { color.green = g; }
void Shape::setBlue(float b)     { color.blue = b; }
void Shape::setOpacity(float a)  { color.alpha = a; }

void Shape::setSize(vec2 size) { this->size = size; }
void Shape::setSizeX(float x)  { size.x = x; }
void Shape::setSizeY(float y)  { size.y = y; }

void move(vec2 deltaPos);
void moveX(float deltaWidth);
void moveY(float deltaHeight);

// Getters
vec2 Shape::getPos() const      { return pos; }
float Shape::getPosX() const    { return pos.x; }
float Shape::getPosY() const    { return pos.y; }
vec2 Shape::getSize() const     { return size; }
vec3 Shape::getColor3() const   { return {color.red, color.green, color.blue}; }
vec4 Shape::getColor4() const   { return color.vec; }
float Shape::getRed() const     { return color.red; }
float Shape::getGreen() const   { return color.green; }
float Shape::getBlue() const    { return color.blue; }
float Shape::getOpacity() const { return color.alpha; }

void Shape::update(float deltaTime) {}