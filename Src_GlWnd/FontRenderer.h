#ifndef _FONTRENDERER_H_
#define _FONTRENDERER_H_
#include <stdio.h>

#include <unordered_map>
#include <string>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "COpenGLCore.h"

#include "Src_Include/ft2build.h"
#include FT_FREETYPE_H

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    long Advance;    // Horizontal offset to advance to next glyph
};

class FontShader {
public:
    GLuint m_ProgramFont;

    //unsigned char* LoadFileContent(const QString& path, int& filesize);
    unsigned char* LoadFileContent(const char* path, int& filesize);
    GLuint CompileShader(GLenum shaderType, const char* shaderCode);
    GLuint CreateProgram(GLuint vsShader, GLuint fsShader);

	void Use();
    void Init(const char* vs, const char* fs);
    //void Init(const QString& vs, const QString& fs);
    COpenGLCore* m_pGLCore = nullptr;
};
#ifndef GENERAL_CHAR_CNT
#define GENERAL_CHAR_CNT 128
#endif

//class FT_Library;
//class FT_Face;

class CFontRenderer
{
public:
    CFontRenderer();
    CFontRenderer(COpenGLCore* pGLCore);
    void Init(float windowWidth, float windowHeight);
    Character* GetOneCharacter(unsigned long nCharEle);
	~CFontRenderer();


    FT_Library* m_pFtInst = nullptr;
    FT_Face* m_pFaceInst = nullptr;

    std::unordered_map<unsigned long, GLuint> m_mapCharEle2TextureID;
    //GLuint* m_pTextures = nullptr;
    COpenGLCore* m_pGLCore = nullptr;
    FontShader* m_pShader = nullptr;
    GLuint m_VAO, m_VBO;
    GLint m_FontProjectID;
    GLint m_MainTexture;
    std::unordered_map<unsigned long, Character*> m_Characters;
    std::vector<GLuint> m_vecCharTextureID;
    void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color);
    void RenderText(const std::wstring& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color);
};


#endif // ! _FONTRENDERER_H_
