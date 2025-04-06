#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <map>
#include <string>


#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

/**
 * @brief A single character
 * @details This struct is used to store information about a single character
 * 
 * @param TextureID ID handle of the glyph texture
 * @param Size Size of glyph
 * @param Bearing Offset from baseline to left/top of glyph
 * @param Advance Offset to advance to next glyph
 */
struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

/**
 * @brief A font
 * @details This class is used to store information about a font
 */
class Font {
    public:
        /**
         * @brief Construct a new Font object
         * 
         * @param fontPath The path to the font file
         * @param fontSize The size of the font
         */
        Font(std::string fontPath, unsigned int fontSize);

        
        /**
         * @brief Get the characters
         * 
         * @return a map of characters
         */
        std::map<char, Character> getCharacters() const;

    private:
        /**
         * @brief A set of character structs mapped to their ASCII character representations
         */
        std::map<char, Character> Characters;

};

#endif //GRAPHICS_FONT_H
