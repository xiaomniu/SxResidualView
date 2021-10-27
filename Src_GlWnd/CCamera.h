#ifndef CAMERA_H
#define CAMERA_H

#include <QKeyEvent>
#include <QOpenGLShader>//为了glfloat 这些变量，所以引进这个头文件
#include <QDebug>
#include "COpenGLCore.h"


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NOPRESS
};

//使用这些宏，可以很方便的修改初始化数据
const GLfloat YAW = -70.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 0.5f;
const GLfloat SENSITIVITY = 0.000001f;
const GLfloat ZOOM = 45.0f;

class CCamera {
public:
    CCamera(void* pGLCore);
    void SetGLCore(void* pGLCore);

    void Yaw(float angle);
    void Pitch(float angle);
    void RotateView(float angle, float x, float y, float z);    
    void UpdateDirection (int deltaX, int deltaY);
    glm::mat4 Update(Camera_Movement direction, float deltaTime);

    void Update(float deltaTime);

    void SwitchTo3D();
    void SwitchTo2D();
    glm::mat4 getViewMatrix(); //返回lookat函数
    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = true); //相应鼠标操作函数
    void processMouseScroll(GLfloat yoffset);//鼠标滚轮事件
    void processKeyboard(Camera_Movement direction, GLfloat deltaTime);//键盘处理事件函数
    //void processInput(GLfloat dt);//键盘循环函数

    glm::vec3 m_Position;
    glm::vec3 m_WorldUp;
    glm::vec3 m_Front;

    glm::vec3 m_Up; //view坐标系 的上方向
    glm::vec3 m_Right; // view坐标系的 右方向

    //Eular Angles
    GLfloat m_Picth;
    GLfloat m_Yaw;

    int m_nMoveLeft = 0, m_nMoveRight = 0, m_nMoveForward = 0, m_nMoveBack = 0, m_nMoveUp = 0, m_nMoveDown = 0;
    int m_nViewportWidth = 0, m_nViewportHeight = 0;
    int m_nViewReset = 0;
    int m_nMoveKeyDown = 0;

    //Camera options
    GLfloat m_fMovementSpeed;
    GLfloat m_fMouseSensitivity;
    GLfloat m_fZoom;

    glm::mat4 mView;
    glm::mat4 m_matView;

    float m_fScaleFa = 1.0f;

    COpenGLCore* m_pGLCore = nullptr;//无需释放
private:
    void updateCameraVectors();

};
#endif // CAMERA_H
