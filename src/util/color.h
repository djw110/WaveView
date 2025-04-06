#ifndef GRAPHICS_COLOR_H
#define GRAPHICS_COLOR_H

#include <glm/glm.hpp>
using std::ostream, glm::vec4;

// Union treats all members as if they were at the same address, so changing one changes the others.
// This allows us to access the color as a vec4 or as individual floats.
typedef struct color {
    union {
        vec4 vec;
        struct {
            float red, green, blue, alpha = 1.0f;
        };
    };

    /* Constructors */
    color() : vec(0.0f, 0.0f, 0.0f, 1.0f) {}
    color(float r, float g, float b) : vec(r, g, b, 1.0f) {}
    color(float r, float g, float b, float a) : vec(r, g, b, a) {}

    /* Overloaded Operator */
    friend ostream &operator<<(ostream &outs, const color &c) {
        outs << "Red: " << c.red << ", Green: " << c.green << ", Blue: " << c.blue << ", Alpha: " << c.alpha;
        return outs;
    }
} color;

// Colors
const color RED = color(1.0f, 0.0f, 0.0f);
const color GREEN = color(0.0f, 1.0f, 0.0f);
const color BLUE = color(0.0f, 0.0f, 1.0f);
const color YELLOW = color(1.0f, 1.0f, 0.0f);
const color CYAN = color(0.0f, 1.0f, 1.0f);
const color MAGENTA = color(1.0f, 0.0f, 1.0f);
const color WHITE = color(1.0f, 1.0f, 1.0f);
const color BLACK = color(0.0f, 0.0f, 0.0f);
const color GRAY = color(0.5f, 0.5f, 0.5f);
const color LIGHT_GRAY = color(0.75f, 0.75f, 0.75f);
const color DARK_GRAY = color(0.2f, 0.2f, 0.2f);


//// How to do this with a class:
//class Color {
//private:
//    union {
//        vec4 vec;
//        struct {
//            float red, green, blue, alpha;
//        };
//    };
//
//public:
//    /* Constructors */
//    Color() : vec(0.0f, 0.0f, 0.0f, 1.0f) {}
//    Color(float r, float g, float b) : vec(r, g, b, 1.0f) {}
//    Color(float r, float g, float b, float a) : vec(r, g, b, a) {}
//
//    /* Overloaded Operator */
//    friend ostream &operator<<(ostream &outs, const Color &c) {
//        outs << "Red: " << c.red << ", Green: " << c.green << ", Blue: " << c.blue << ", Alpha: " << c.alpha;
//        return outs;
//    }
//
//    /* Getters */
//    float getRed() const { return red; }
//    float getGreen() const { return green; }
//    float getBlue() const { return blue; }
//    float getAlpha() const { return alpha; }
//
//    /* Setters */
//    void setRed(float r) { red = r; }
//    void setGreen(float g) { green = g; }
//    void setBlue(float b) { blue = b; }
//    void setAlpha(float a) { alpha = a; }
//};

#endif //GRAPHICS_COLOR_H