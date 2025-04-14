#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <float.h>
#include <time.h>

/* Window dimensions */
const GLuint WIDTH = 800, HEIGHT = 600;

/* Shader sources */
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    TexCoords = vertex.zw;\n"
    "}\0";

const char* fragmentShaderSource = 
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D text;\n"
    "uniform vec3 textColor;\n"
    "void main() {\n"
    "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    "    color = vec4(textColor, 1.0) * sampled;\n"
    "}\0";

/* Character structure */
typedef struct {
    GLuint TextureID;   /* ID handle of the glyph texture */
    int Width;          /* Width of glyph */
    int Height;         /* Height of glyph */
    int Advance;        /* Advance of glyph (horizontal offset) */
    int Left;           /* Left offset of glyph */
    int Top;            /* Top offset of glyph */
} Character;

/* 扩展字符映射，用于存储中文字符 */
#define MAX_CHARS 65536  /* 支持基本的 Unicode 范围 */
Character* Characters;

GLuint VAO, VBO;
GLuint shaderProgram;

/* Function prototypes */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint compileShaders(void);
void initFreeType(void);
void renderText(const char* text, float x, float y, float scale, float r, float g, float b);
unsigned int codepoint_from_utf8(const char** text);

int main(void) {
    /* Initialize GLFW */
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    /* Set OpenGL version and profile */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "你好，世界！", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Set callback functions */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    /* Initialize GLEW */
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    /* Compile and link shaders */
    shaderProgram = compileShaders();
    
    /* 分配字符映射内存 */
    Characters = (Character*)calloc(MAX_CHARS, sizeof(Character));
    if (!Characters) {
        fprintf(stderr, "Failed to allocate memory for characters\n");
        return -1;
    }
    
    /* Initialize FreeType and load font */
    initFreeType();
    
    /* Configure VAO/VBO for text rendering */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    /* Enable blending for text rendering */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* Set up projection matrix */
    glUseProgram(shaderProgram);
    GLfloat projection[16] = {
        2.0f / WIDTH, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / HEIGHT, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    
    /* Main loop */
    while (!glfwWindowShouldClose(window)) {
        /* Clear the screen */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* 渲染中文文本 */
        const char* text = "你好，世界！";
        float scale = 1.5f;
        
        /* 计算文本总宽度 */
        float textWidth = 0.0f;
        const char* p = text;
        while (*p) {
            unsigned int codepoint = codepoint_from_utf8(&p);
            textWidth += (Characters[codepoint].Advance >> 6) * scale;
        }
        
        /* 计算水平居中位置 */
        float x = (WIDTH - textWidth) / 2.0f;
        
        /* 计算垂直居中位置（基线位置） */
        float y = HEIGHT / 2.0f;
        
        /* 渲染不同颜色的文本 */
        renderText(text, x, y-100, scale, 0, 0, 0);       // 黑色
        renderText(text, x, y, scale, -1, -1, -1);        // 彩虹模式
        renderText(text, x, y+100, scale, 255, 215, 0);   // 金色
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    /* Clean up */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    /* 释放字符映射内存 */
    free(Characters);
    
    /* Terminate GLFW */
    glfwTerminate();
    
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint compileShaders(void) {
    /* Vertex shader */
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    /* Check for shader compile errors */
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
    /* Fragment shader */
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    /* Check for shader compile errors */
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
    /* Link shaders */
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    /* Check for linking errors */
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    
    /* Delete the shaders as they're linked into our program now and no longer necessary */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

void initFreeType(void) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library\n");
        exit(1);
    }
    
    FT_Face face;
    /* 尝试加载支持中文的字体 */
    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",  // Linux
        "/usr/share/fonts/noto/NotoSansCJK-Regular.ttc",              // Linux
        "/usr/share/fonts/wenquanyi/wqy-microhei/wqy-microhei.ttc",   // Linux
        "C:/Windows/Fonts/msyh.ttc",                                  // Windows
        "C:/Windows/Fonts/simsun.ttc",                                // Windows
        "/System/Library/Fonts/PingFang.ttc",                         // macOS
        "fonts/NotoSansSC-Regular.otf"                                // 自定义路径
    };
    
    int numFonts = sizeof(fontPaths) / sizeof(fontPaths[0]);
    int fontLoaded = 0;
    
    for (int i = 0; i < numFonts; i++) {
        if (FT_New_Face(ft, fontPaths[i], 0, &face) == 0) {
            printf("Successfully loaded font: %s\n", fontPaths[i]);
            fontLoaded = 1;
            break;
        }
    }
    
    if (!fontLoaded) {
        fprintf(stderr, "ERROR::FREETYPE: Failed to load any font\n");
        exit(1);
    }
    
    /* 设置字体大小 */
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    /* 禁用字节对齐限制 */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    
    /* 预加载中文字符 */
    const char* chineseText = "你好，世界！";
    const char* p = chineseText;
    
    while (*p) {
        unsigned int codepoint = codepoint_from_utf8(&p);
        
        /* 如果字符尚未加载，则加载它 */
        if (Characters[codepoint].TextureID == 0) {
            if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER)) {
                fprintf(stderr, "ERROR::FREETYTPE: Failed to load Glyph (Unicode %u)\n", codepoint);
                continue;
            }
            
            /* 生成纹理 */
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            
            /* 设置纹理选项 */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            /* 存储字符供以后使用 */
            Character character = {
                texture,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                face->glyph->advance.x,
                face->glyph->bitmap_left,
                face->glyph->bitmap_top
            };
            Characters[codepoint] = character;
        }
    }
    
    /* 清理 FreeType 资源 */
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

/* 从 UTF-8 编码的字符串中提取 Unicode 码点 */
unsigned int codepoint_from_utf8(const char** text) {
    unsigned int codepoint = 0;
    const unsigned char* str = (const unsigned char*)(*text);
    
    if (str[0] < 0x80) {  // 单字节 ASCII
        codepoint = str[0];
        *text += 1;
    } else if ((str[0] & 0xE0) == 0xC0) {  // 双字节
        codepoint = ((str[0] & 0x1F) << 6) | (str[1] & 0x3F);
        *text += 2;
    } else if ((str[0] & 0xF0) == 0xE0) {  // 三字节 (中文常见)
        codepoint = ((str[0] & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
        *text += 3;
    } else if ((str[0] & 0xF8) == 0xF0) {  // 四字节
        codepoint = ((str[0] & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
                    ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
        *text += 4;
    }
    
    return codepoint;
}

void renderText(const char* text, float x, float y, float scale, float r, float g, float b) {
    glUseProgram(shaderProgram);
    GLint colorLoc = glGetUniformLocation(shaderProgram, "textColor");
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    
    // 是否使用彩虹模式（每个字符不同颜色）
    int rainbowMode = (r < 0 || g < 0 || b < 0);
    
    // 为彩虹模式生成鲜艳颜色的种子
    srand(time(NULL));
    
    // 遍历 UTF-8 编码的文本
    const char* p = text;
    while (*p) {
        // 解码 UTF-8 获取 Unicode 码点
        unsigned int codepoint = codepoint_from_utf8(&p);
        Character ch = Characters[codepoint];
        
        // 如果字符未加载，跳过
        if (ch.TextureID == 0) continue;
        
        // 如果是彩虹模式，为每个字符生成鲜艳的随机颜色
        if (rainbowMode) {
            // 生成鲜艳的颜色 - 确保至少一个通道接近最大值
            float bright_r, bright_g, bright_b;
            
            // 确保颜色鲜艳的策略：
            // 1. 随机选择一个通道接近最大值 (0.8-1.0)
            // 2. 其他通道随机但较低，确保对比度
            int primary = rand() % 3; // 随机选择一个主通道
            
            if (primary == 0) {
                bright_r = 0.8f + (rand() % 20) / 100.0f; // 0.8-1.0
                bright_g = (rand() % 60) / 100.0f;        // 0.0-0.6
                bright_b = (rand() % 60) / 100.0f;        // 0.0-0.6
            } else if (primary == 1) {
                bright_r = (rand() % 60) / 100.0f;        // 0.0-0.6
                bright_g = 0.8f + (rand() % 20) / 100.0f; // 0.8-1.0
                bright_b = (rand() % 60) / 100.0f;        // 0.0-0.6
            } else {
                bright_r = (rand() % 60) / 100.0f;        // 0.0-0.6
                bright_g = (rand() % 60) / 100.0f;        // 0.0-0.6
                bright_b = 0.8f + (rand() % 20) / 100.0f; // 0.8-1.0
            }
            
            glUniform3f(colorLoc, bright_r, bright_g, bright_b);
        } else {
            // 使用传入的统一颜色
            glUniform3f(colorLoc, r/255.0f, g/255.0f, b/255.0f);
        }
        
        // 计算位置 - 基线对齐
        float xpos = x + ch.Left * scale;
        
        // 注意：在我们的坐标系统中，y 轴是从上到下的
        // ch.Top 是从基线到字形顶部的距离
        float ypos = y - (ch.Height - ch.Top) * scale;
        
        float w = ch.Width * scale;
        float h = ch.Height * scale;
        
        // 更新顶点数据 - 注意 Y 坐标的顺序
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },  // 左上 (纹理坐标 0,0)
            { xpos,     ypos,       0.0f, 1.0f },  // 左下 (纹理坐标 0,1)
            { xpos + w, ypos,       1.0f, 1.0f },  // 右下 (纹理坐标 1,1)
            
            { xpos,     ypos + h,   0.0f, 0.0f },  // 左上 (纹理坐标 0,0)
            { xpos + w, ypos,       1.0f, 1.0f },  // 右下 (纹理坐标 1,1)
            { xpos + w, ypos + h,   1.0f, 0.0f }   // 右上 (纹理坐标 1,0)
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        x += (ch.Advance >> 6) * scale;
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
} 