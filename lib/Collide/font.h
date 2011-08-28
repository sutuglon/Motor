/*
 *  font.h
 *  Noise
 *
 *  Created by Chema on 11/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 * from http://gpwiki.org/index.php/OpenGL:Tutorials:Font_System#oglBMfont
 *
 */
 
#ifndef Font_H
#define Font_H

class Font
{
public:
    Font(const char* file=0);
    ~Font();

    void load(const char* file);
    size_t lineHeight() const; // The line height is constant
    size_t charWidth(unsigned char c) const;
    size_t stringWidth(const std::string& str) const;
    void preDrawString() const;
    void drawString(const std::string& str, float x, float y) const;
    void posDrawString() const;

private:
    // Information about a glyph. Tex_y2 can be calculated from tex_y1
    // and _tex_line_height (see below). Advance is the width of the
    // glyph in screen space.
    struct Glyph
    {
        float tex_x1, tex_y1, tex_x2;
        size_t advance;
    };
    // An array to store the glyphs.
    Glyph* _glyphs;
    // A table to quickly get the glyph belonging to a character.
    Glyph* _table[256];
    // The line height, in screen space and in texture space, and the
    // OpenGL id of the font texture.
    unsigned _line_height, _texture;
    float _tex_line_height;
};

#endif
