
//#include "GL/glew.h"

#include "FontRenderer.h"
#include <iostream>
#include <QtCore/QFile>

//#pragma comment(lib, "freetype.lib")

void FontShader::Use() {
    this->m_pGLCore->glUseProgram(this->m_ProgramFont);
}
//void FontShader::Init(const QString& vs, const QString& fs) {
//    int nFileSize = 0;
//    const char* vsCode = (char*)LoadFileContent(vs, nFileSize);
//    const char* fsCode = (char*)LoadFileContent(fs, nFileSize);
//    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
//    if (vsShader == 0) {
//        return;
//    }
//    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
//    if (fsShader == 0) {
//        return;
//    }
//    mProgram = CreateProgram(vsShader, fsShader);
//    core->glDeleteShader(vsShader);
//    core->glDeleteShader(fsShader);
//    if (mProgram == 0) {
//        printf("Font Program create faild!\n");
//    }
//}
void FontShader::Init(const char* vs, const char* fs) {
    int nFileSize = 0;
    //const char* vsCode = (char*)LoadFileContent(vs, nFileSize);
    //const char* fsCode = (char*)LoadFileContent(fs, nFileSize);
    const char szFontShaderVS[] = {
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    TexCoords = vertex.zw; }"
    };
    const char szFontShaderFS[] = {
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "uniform sampler2D text;\n"
        "uniform vec4 textColor;\n"
        "void main() {\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    gl_FragColor = vec4(textColor) * sampled;}"
        };

    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, szFontShaderVS);
    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, szFontShaderFS);
    if (vsShader != 0 && fsShader != 0) {
        this->m_ProgramFont = CreateProgram(vsShader, fsShader);
        this->m_pGLCore->glDeleteShader(vsShader);
        this->m_pGLCore->glDeleteShader(fsShader);
        if (this->m_ProgramFont == 0) {
            printf("Font Program create faild!\n");
        }
    }
    //if(vsCode){
    //    delete[] vsCode;
    //    vsCode = nullptr;
    //}
    //if(fsCode){
    //    delete[] fsCode;
    //    fsCode = nullptr;
    //}
}

//unsigned char* FontShader::LoadFileContent(const QString& path, int& filesize) {
//    unsigned char* fileContent = nullptr;
//    QFile ff(path);
//    ff.open(QFile::OpenModeFlag::ReadWrite);
//    //filesize = ff.size();
//    QByteArray shaderCode = ff.readAll();
//    fileContent = (unsigned char*)(shaderCode.data());
//    return fileContent;
//}

unsigned char* FontShader::LoadFileContent(const char* path, int& filesize) {    
    QString sPath = QString::fromLocal8Bit(path);
    QFile ff(sPath);
    ff.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text);
    filesize = ff.size();
    if(filesize <=0){
        ff.close();
        return nullptr;
    }
    QByteArray szCode = ff.readAll();

    unsigned char* fileContent = new unsigned char[filesize + 1];
    fileContent[filesize] = 0;
    szCode.data();
    memcpy(fileContent, szCode.data(), filesize);
    //filesize = 0;
    //FILE* pFile = fopen(path, "rb");
    //if (pFile) {
    //    fseek(pFile, 0, SEEK_END);
    //    int nLen = ftell(pFile);
    //    if (nLen > 0) {
    //        rewind(pFile);
    //        fileContent = new unsigned char[nLen + 1];
    //        fread(fileContent, sizeof(unsigned char), nLen, pFile);
    //        fileContent[nLen] = '\0';
    //        filesize = nLen;
    //    }
    //    fclose(pFile);
    //}
    ff.close();
    return fileContent;
}
GLuint FontShader::CompileShader(GLenum shaderType, const char* shaderCode) {
    GLuint shader = this->m_pGLCore->glCreateShader(shaderType);
    this->m_pGLCore->glShaderSource(shader, 1, &shaderCode, nullptr);
    this->m_pGLCore->glCompileShader(shader);
    GLint compileResult = GL_TRUE;
    this->m_pGLCore->glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE) {
        char szLog[1024] = { 0 };
        GLsizei logLen = 0;
        this->m_pGLCore->glGetShaderInfoLog(shader, 1024, &logLen, szLog);
        printf("Compile Shader fail error log : %s \nshader code :\n%s\n", szLog, shaderCode);
        this->m_pGLCore->glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}
GLuint FontShader::CreateProgram(GLuint vsShader, GLuint fsShader) {
    GLuint program = this->m_pGLCore->glCreateProgram();
    this->m_pGLCore->glAttachShader(program, vsShader);
    this->m_pGLCore->glAttachShader(program, fsShader);
    this->m_pGLCore->glLinkProgram(program);
    this->m_pGLCore->glDetachShader(program, vsShader);
    this->m_pGLCore->glDetachShader(program, fsShader);
    GLint nResult;
    this->m_pGLCore->glGetProgramiv(program, GL_LINK_STATUS, &nResult);
    if (nResult == GL_FALSE) {
        char log[1024] = { 0 };
        GLsizei writed = 0;
        this->m_pGLCore->glGetProgramInfoLog(program, 1024, &writed, log);
        printf("create gpu program fail,link error : %s\n", log);
        this->m_pGLCore->glDeleteProgram(program);
        program = 0;
    }
    return program;
}
///////////////////////
CFontRenderer::CFontRenderer(){

}

CFontRenderer::CFontRenderer(COpenGLCore* pGLCore){
    this->m_pGLCore = pGLCore;
}

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
void CFontRenderer::Init(float windowWidth, float windowHeight) {
	//////////////////////////////////
	// Set OpenGL options
    //core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    this->m_pShader = new FontShader;
    this->m_pShader->m_pGLCore = this->m_pGLCore;
    // Compile and setup the shader
    this->m_pShader->Init(":/img/gl_images/Res/shaders/FontRender.vs", ":/img/gl_images/Res/shaders/FontRender.fs");
    //this->m_pShader->Init(szFontShaderVS, szFontShaderFS);
    //shader->Init(":/shaders/res/shaders/FontRender.vs", ":/shaders/res/shaders/FontRender.fs");
    //glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    this->m_pShader->Use();
    this->m_FontProjectID = this->m_pGLCore->glGetUniformLocation(this->m_pShader->m_ProgramFont, "projection");
	// FreeType
    //FT_Library ft;
    this->m_pFtInst = new FT_Library;
	// All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(this->m_pFtInst))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	// Load font as face
    //FT_Face face;
    this->m_pFaceInst = new FT_Face;

    //QString fileName = QCoreApplication::applicationDirPath();
    //D:/QT_CODE_PRJ/testCtrl/rc/fonts/PingFang Medium.ttf  D:/QT_CODE_PRJ/OpenGL/HqqWndFrame0101/res/fonts/consola.ttf
    if (FT_New_Face(*this->m_pFtInst, (this->m_pGLCore->m_sExeDir+"/PingFang Medium.ttf").c_str(), 0, this->m_pFaceInst)){
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return ;
    }
	// Set size to load glyphs as
    FT_Set_Pixel_Sizes(*this->m_pFaceInst, 0, 32);
	// Disable byte-alignment restriction
    this->m_pGLCore->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Load first 128 characters of ASCII set
    //this->m_pTextures = new GLuint[GENERAL_CHAR_CNT];
    GLuint* pTextures = new GLuint[GENERAL_CHAR_CNT];
    this->m_pGLCore->glGenTextures(GENERAL_CHAR_CNT, pTextures);
    for (FT_ULong c = 0; c < GENERAL_CHAR_CNT; c++) {
		// Load character glyph 

        if (FT_Load_Char(*this->m_pFaceInst, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
        // Generate texture
        this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, pTextures[c]);
        this->m_pGLCore->glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
            (int)(*this->m_pFaceInst)->glyph->bitmap.width,
            (int)(*this->m_pFaceInst)->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
            (*this->m_pFaceInst)->glyph->bitmap.buffer
		);
		// Set texture options
        this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character* pCharacter = new Character {
            pTextures[c],
            glm::ivec2((*this->m_pFaceInst)->glyph->bitmap.width, (*this->m_pFaceInst)->glyph->bitmap.rows),
            glm::ivec2((*this->m_pFaceInst)->glyph->bitmap_left,  (*this->m_pFaceInst)->glyph->bitmap_top),
            (*this->m_pFaceInst)->glyph->advance.x
		};
        this->m_Characters.insert(std::pair<unsigned long, Character*>(c, pCharacter));
        this->m_vecCharTextureID.push_back(pTextures[c]);
    }

    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, 0);

	// Configure VAO/VBO for texture quads
    this->m_pGLCore->glGenVertexArrays(1, &this->m_VAO);
    this->m_pGLCore->glGenBuffers(1, &this->m_VBO);
    this->m_pGLCore->glBindVertexArray(this->m_VAO);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
    this->m_pGLCore->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    this->m_pGLCore->glEnableVertexAttribArray(0);
    this->m_pGLCore->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    this->m_pGLCore->glBindVertexArray(0);
    this->m_pGLCore->glUseProgram(0);
	//////////////////////////////////
}

Character* CFontRenderer::GetOneCharacter(unsigned long nCharEle) {
    auto itt = this->m_Characters.find(nCharEle);
    if(itt != this->m_Characters.end()) {
        return itt->second;
    }
    GLuint nTextureID = -1;
    this->m_pGLCore->glGenTextures(1, &nTextureID);


    FT_Error result = FT_Load_Glyph((*(this->m_pFaceInst)), FT_Get_Char_Index((*(this->m_pFaceInst)), nCharEle), FT_LOAD_DEFAULT);
    if (result) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return nullptr;
    }
    // 第二个参数为渲染模式
    result = FT_Render_Glyph((*(this->m_pFaceInst))->glyph, FT_RENDER_MODE_NORMAL);
    //(*(this->m_pFaceInst))->glyph->bitmap.width
    //if (FT_Load_Char(*this->m_pFaceInst, nCharEle, FT_LOAD_RENDER)) {
    //    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    //    return nullptr;
    //}
    // Generate texture
    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, nTextureID);
    this->m_pGLCore->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        (int)(*this->m_pFaceInst)->glyph->bitmap.width,
        (int)(*this->m_pFaceInst)->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        (*this->m_pFaceInst)->glyph->bitmap.buffer
    );
    // Set texture options
    this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    this->m_pGLCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character* pCharacter = new Character {
        nTextureID,
        glm::ivec2((*this->m_pFaceInst)->glyph->bitmap.width, (*this->m_pFaceInst)->glyph->bitmap.rows),
        glm::ivec2((*this->m_pFaceInst)->glyph->bitmap_left,  (*this->m_pFaceInst)->glyph->bitmap_top),
        (*this->m_pFaceInst)->glyph->advance.x
    };
    this->m_Characters.insert(std::pair<unsigned long, Character*>(nCharEle, pCharacter));
    this->m_vecCharTextureID.push_back(nTextureID);
    return pCharacter;
}

CFontRenderer::~CFontRenderer() {

    // Destroy FreeType once we're finished
    if(this->m_pFaceInst){
        FT_Done_Face((*this->m_pFaceInst));
        delete this->m_pFaceInst;
        this->m_pFaceInst = nullptr;
    }
    if(this->m_pFtInst){
        FT_Done_FreeType(*this->m_pFtInst);
        delete this->m_pFtInst;
        this->m_pFtInst = nullptr;
    }
    if(this->m_pShader){
        delete this->m_pShader;
        this->m_pShader = nullptr;
    }
    int nCharTextureIDs = static_cast<int>(this->m_vecCharTextureID.size());
    if(nCharTextureIDs > 0){
        GLuint* pTextureIDs = new GLuint[nCharTextureIDs];
        for(int i=0; i<nCharTextureIDs; i++) {
            pTextureIDs[i] = this->m_vecCharTextureID[i];
        }
        this->m_pGLCore->glDeleteTextures(nCharTextureIDs, pTextureIDs);
        delete pTextureIDs;
        pTextureIDs = nullptr;
        this->m_vecCharTextureID.clear();
    }
    //if(this->m_pTextures){
    //    this->m_pGLCore->glDeleteTextures(GENERAL_CHAR_CNT, this->m_pTextures);
    //    delete[] this->m_pTextures;
    //    this->m_pTextures = nullptr;
    //}
}

#include <QtCore/QTextCodec>
void CFontRenderer::RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
    QByteArray  sQtText = text.c_str();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString sUniStr = codec->toUnicode(sQtText);
    std::wstring sWStr = sUniStr.toStdWString();
    RenderText(sWStr, x, y, scale, color);
    return;
    //this->m_pGLCore->glEnable(GL_CULL_FACE);
    //this->m_pGLCore->glEnable(GL_BLEND);
    //this->m_pGLCore->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    //this->m_pShader->Use();
    //
    //this->m_pGLCore->glUniformMatrix4fv(this->m_FontProjectID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatOrthoProject));
    //
    //this->m_pGLCore->glUniform3f(this->m_pGLCore->glGetUniformLocation(this->m_pShader->m_ProgramFont, "textColor"), color.x, color.y, color.z);
    //this->m_pGLCore->glActiveTexture(GL_TEXTURE0);
    //this->m_pGLCore->glBindVertexArray(this->m_VAO);
    //this->m_MainTexture = this->m_pGLCore->glGetUniformLocation(this->m_pShader->m_ProgramFont, "text");
    //// Iterate through all characters
    //std::string::const_iterator c;
    //for (c = text.begin(); c != text.end(); c++)
    //{
    //    //Character ch = this->m_Characters[*c];
    //    Character* pCharacter = this->GetOneCharacter(static_cast<unsigned long>(*c));
    //
    //    GLfloat xpos = x + pCharacter->Bearing.x * scale;
    //    GLfloat ypos = y - (pCharacter->Size.y - pCharacter->Bearing.y) * scale;
    //
    //    GLfloat w = pCharacter->Size.x * scale;
    //    GLfloat h = pCharacter->Size.y * scale;
    //    // Update VBO for each character
    //    GLfloat vertices[6][4] = {
    //        { xpos,     ypos + h,   0.0, 0.0 },
    //        { xpos,     ypos,       0.0, 1.0 },
    //        { xpos + w, ypos,       1.0, 1.0 },
    //
    //        { xpos,     ypos + h,   0.0, 0.0 },
    //        { xpos + w, ypos,       1.0, 1.0 },
    //        { xpos + w, ypos + h,   1.0, 0.0 }
    //    };
    //    // Render glyph texture over quad
    //    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, pCharacter->TextureID);
    //    this->m_pGLCore->glUniform1i(this->m_MainTexture, 0);
    //    // Update content of VBO memory
    //    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
    //    this->m_pGLCore->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
    //
    //    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    // Render quad
    //    this->m_pGLCore->glDrawArrays(GL_TRIANGLES, 0, 6);
    //    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    //    x += (pCharacter->Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    //}
    //this->m_pGLCore->glBindVertexArray(0);
    //this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, 0);
    //this->m_pGLCore->glUseProgram(0);
    //
    //this->m_pGLCore->glDisable(GL_BLEND);
    //this->m_pGLCore->glDisable(GL_CULL_FACE);
}


void CFontRenderer::RenderText(const std::wstring& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
	// Activate corresponding render state	
    this->m_pGLCore->glEnable(GL_CULL_FACE);
    this->m_pGLCore->glEnable(GL_BLEND);
    this->m_pGLCore->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->m_pShader->Use();

    this->m_pGLCore->glUniformMatrix4fv(this->m_FontProjectID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatOrthoProject));

    this->m_pGLCore->glUniform4fv(this->m_pGLCore->glGetUniformLocation(this->m_pShader->m_ProgramFont, "textColor"), 1, glm::value_ptr(color));
    this->m_pGLCore->glActiveTexture(GL_TEXTURE0);
    this->m_pGLCore->glBindVertexArray(this->m_VAO);
    this->m_MainTexture = this->m_pGLCore->glGetUniformLocation(this->m_pShader->m_ProgramFont, "text");
	// Iterate through all characters
    //std::wstring::const_iterator c;
    //for (c = text.begin(); c != text.end(); c++)
    int nTextLen = text.length();
    const wchar_t* szwText = text.c_str();
    int i = 0;
    for(; i< nTextLen; i++){
        unsigned long c = szwText[i];
        //Character ch = this->m_Characters[*c];
        Character* pCharacter = this->GetOneCharacter(static_cast<unsigned long>(c));

        GLfloat xpos = x + pCharacter->Bearing.x * scale;
        GLfloat ypos = y - (pCharacter->Size.y - pCharacter->Bearing.y) * scale;

        GLfloat w = pCharacter->Size.x * scale;
        GLfloat h = pCharacter->Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
        this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, pCharacter->TextureID);
        this->m_pGLCore->glUniform1i(this->m_MainTexture, 0);
		// Update content of VBO memory
        this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
        this->m_pGLCore->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
        this->m_pGLCore->glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (pCharacter->Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
    this->m_pGLCore->glBindVertexArray(0);
    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, 0);
    this->m_pGLCore->glUseProgram(0);

    this->m_pGLCore->glDisable(GL_BLEND);
    this->m_pGLCore->glDisable(GL_CULL_FACE);
}
