#include "CCamera.h"

CCamera::CCamera(void* pGLCore){
    this->SetGLCore(pGLCore);
    this->m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //this->m_Position = glm::vec3(880.0f, 880.0f, 880.0f);//(0.0f, 0.0f, 100.0f);
    //this->m_Position = glm::vec3(0.0f, 0.0f, 100.0f);//(0.0f, 0.0f, 100.0f);
    this->m_Position = glm::vec3(0.0f, 0.0f, 1300.0f);//(0.0f, 0.0f, 100.0f);
    this->m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->m_Front = glm::vec3(0.0f, 0.0f, 0.0f);
    this->m_fMovementSpeed = SPEED;
    this->m_fMouseSensitivity = SENSITIVITY;
    this->m_fZoom = ZOOM;
}

void CCamera::SetGLCore(void* pGLCore) {
    this->m_pGLCore = static_cast<COpenGLCore*>(pGLCore);
}

glm::mat4 CCamera::getViewMatrix()
{
    glm::mat4 view = glm::lookAt(this->m_Position, this->m_Front, this->m_Up);
    //mView.lookAt(this->m_Position, this->m_Front, this->m_Up);
    return view;
}

void CCamera::processKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = this->m_fMovementSpeed * deltaTime;
    ////if (direction == FORWARD)
    ////    this->m_Position += this->m_Front * velocity;
    ////if (direction == BACKWARD)
    ////    this->m_Position -= this->m_Front * velocity;
    //if (direction == FORWARD){
    //
    //    //if (this->zoom >= 1.0f && this->zoom <= 45.0f)
    //    //    this->zoom -= velocity;
    //    //if (this->zoom < 1.0f)
    //    //    this->zoom = 1.0f;
    //
    //    //W
    //    this->m_Position += this->m_Front * velocity;
    //}
    //if (direction == BACKWARD){
    //
    //    //if (this->zoom >= 1.0f && this->zoom <= 45.0f)
    //    //    this->zoom += velocity;
    //    //if (this->zoom > 45.0f)
    //    //    this->zoom = 45.0f;
    //
    //    //S
    //    this->m_Position -= this->m_Front * velocity;
    //}
    //if (direction == LEFT)
    //    this->m_Position -= this->m_Right * velocity;
    //if (direction == RIGHT)
    //    this->m_Position += this->m_Right * velocity;
    //if (direction == UP)
    //    this->m_Position += this->m_WorldUp * velocity;
    //if (direction == DOWN)
    //    this->m_Position -= this->m_WorldUp * velocity;


    float moveSpeed = 10.0f;
    glm::vec3 forwardDirection = this->m_Front - this->m_Position;
    glm::normalize(forwardDirection);
    glm::vec3 rightDirection = glm::cross(forwardDirection, this->m_WorldUp);
    glm::normalize(rightDirection);
    if (direction == LEFT) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    if (direction == RIGHT) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (direction == FORWARD) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (direction == BACKWARD) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    if (direction == UP){
        this->m_Position += this->m_WorldUp * velocity;
        this->m_Front += this->m_WorldUp * velocity;
    }
    if (direction == DOWN){
        this->m_Position -= this->m_WorldUp * velocity;
        this->m_Front -= this->m_WorldUp * velocity;
    }
    //direction = NOPRESS;
}

void CCamera::processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch)
{
    xoffset *= this->m_fMouseSensitivity;
    yoffset *= this->m_fMouseSensitivity;

    this->m_Yaw += xoffset;
    this->m_Picth += yoffset;

    if (constraintPitch) {
        if (this->m_Picth > 89.0f)
            this->m_Picth = 89.0f;
        if (this->m_Picth < -89.0f)
            this->m_Picth = -89.0f;
    }

    this->updateCameraVectors();
}

void CCamera::processMouseScroll(GLfloat yoffset)
{
    //if (this->zoom >= 1.0f && this->zoom <= 45.0f)
    //  this->zoom -= yoffset;
    //if (this->zoom > 45.0f)
    //  this->zoom = 45.0f;
    //if (this->zoom < 1.0f)
    //    this->zoom = 1.0f;

    //if(yoffset > 0){
    //    this->m_Position[0] *= 0.7f;
    //    this->m_Position[1] *= 0.7f;
    //    this->m_Position[2] *= 0.7f;
    //}
    //else{
    //    this->m_Position[0] *= 1.3f;
    //    this->m_Position[1] *= 1.3f;
    //    this->m_Position[2] *= 1.3f;
    //}

    if(yoffset < 0){
        //this->m_Position -= this->m_Front * 1.1f;
        m_fScaleFa *= 1.1f;
    }
    else {
        //this->m_Position += this->m_Front * 1.1f;
        m_fScaleFa *= 0.9f;
    }
}

//void Camera::processInput(GLfloat dt)
//{
//    if (keys[Qt::Key_W])
//      processKeyboard(FORWARD, dt);
//    if (keys[Qt::Key_S])
//      processKeyboard(BACKWARD, dt);
//    if (keys[Qt::Key_A])
//      processKeyboard(LEFT, dt);
//    if (keys[Qt::Key_D])
//      processKeyboard(RIGHT, dt);
//    if (keys[Qt::Key_E])
//      processKeyboard(UP, dt);
//    if (keys[Qt::Key_Q])
//      processKeyboard(DOWN, dt);
//}

void CCamera::updateCameraVectors()
{
    glm::vec3 front3(cos(this->m_Yaw) * cos(this->m_Picth), sin(this->m_Picth), sin(this->m_Yaw) * cos(this->m_Picth));
    glm::normalize(front3);
    this->m_Front = front3;
    this->m_Right = glm::cross(this->m_Front, this->m_WorldUp);
    glm::normalize(this->m_Right);
    this->m_Up = glm::cross(this->m_Right, this->m_Front);
    glm::normalize(this->m_Up);
}

glm::mat4 CCamera::Update(Camera_Movement direction, float deltaTime){
    float moveSpeed = 10.0f;
    glm::vec3 forwardDirection = this->m_Front - this->m_Position;
    glm::normalize(forwardDirection);
    glm::vec3 rightDirection = glm::cross(forwardDirection, this->m_WorldUp);
    glm::normalize(rightDirection);

    if (direction == LEFT) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    if (direction == RIGHT) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (direction == FORWARD) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (direction == BACKWARD) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    //gluLookAt(this->m_Position.x, this->m_Position.y, this->m_Position.z,
    //    this->m_Front.x, this->m_Front.y, this->m_Front.z,
    //    this->m_WorldUp.x, this->m_WorldUp.y, this->m_WorldUp.z);

    glm::mat4 view = glm::lookAt(this->m_Position, this->m_Front, this->m_Up);
    //mView.lookAt(this->m_Position, this->m_Front, this->m_Up);
    return view;
}


// Initial horizontal angle : toward -Z
static float horizontalAngle = PI;
// Initial vertical angle : none
static float verticalAngle = 0.0f;
void CCamera::Update(float deltaTime){
    //printf("fresh: deltaTime : %f\n", deltaTime);
    //if(deltaTime>0.016f){
    //    deltaTime = 0.016f;
    //}
    const float moveSpeed = 100.0f;
    glm::vec3 forwardDirection = this->m_Front - this->m_Position;
    glm::normalize(forwardDirection);
    glm::vec3 rightDirection = this->m_Right;//QVector3D::crossProduct(forwardDirection, this->m_Up);
    glm::normalize(rightDirection);
    if (this->m_nMoveLeft) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    if (m_nMoveRight) {
        glm::vec3 delta = rightDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (this->m_nMoveForward) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position + delta;
        this->m_Front = this->m_Front + delta;
    }
    if (this->m_nMoveBack) {
        glm::vec3 delta = forwardDirection*deltaTime*moveSpeed;
        this->m_Position = this->m_Position - delta;
        this->m_Front = this->m_Front - delta;
    }
    if (this->m_nMoveUp) {
        float fDis = deltaTime*moveSpeed;
        this->m_Position.y = (this->m_Position.y + fDis);
        this->m_Front.y = (this->m_Front.y + fDis);
    }
    if (this->m_nMoveDown) {
        float fDis = deltaTime*moveSpeed;
        this->m_Position.y = (this->m_Position.y - fDis);
        this->m_Front.y = (this->m_Front.y - fDis);
    }
    if(this->m_nViewReset){
        this->m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        //this->m_Position = glm::vec3(0.0f, 0.0f, 50.0f);//(0.0f, 0.0f, 100.0f);
        //this->m_Position = glm::vec3(480.0f, 480.0f, 480.0f);//(0.0f, 0.0f, 100.0f);
        this->m_Position = glm::vec3(300.0f, 300.0f, 300.0f);//(0.0f, 0.0f, 100.0f);
        this->m_Position = glm::vec3(0.0f, 0.0f, 1300.0f);//(0.0f, 0.0f, 100.0f);
        //this->m_Position = QVector3D(380.0f, 380.0f, 380.0f);//();
        this->m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->m_Front = glm::vec3(0.0f, 0.0f, 0.0f);
        this->m_fMovementSpeed = SPEED;
        this->m_fMouseSensitivity = SENSITIVITY;
        this->m_fZoom = ZOOM;
        horizontalAngle = PI;
        verticalAngle = 0.0f;
        this->m_pGLCore->ResetMidleDownMovingPos();
        this->m_pGLCore->WheelEventEnd();
    }

    //this->m_pGLCore->SetViewMatrix(
    this->m_matView = glm::lookAt(this->m_Position, this->m_Front, this->m_Up);

    //QMatrix4x4 view;
    //view.lookAt(this->m_Position, this->m_Front, this->m_Up);
}

void CCamera::Pitch(float angle) {
    glm::vec3 viewDirection = this->m_Front - this->m_Position;
    glm::normalize(viewDirection);
    glm::vec3 rightDirection = glm::cross(viewDirection, this->m_Up);
    glm::normalize(rightDirection);
    RotateView(angle, rightDirection.x, rightDirection.y, rightDirection.z);
}
void CCamera::Yaw(float angle) {
    RotateView(angle, this->m_WorldUp.x, this->m_WorldUp.y, this->m_WorldUp.z);
}
void CCamera::RotateView(float angle, float x, float y, float z) {
    //if(fabs(angle) <= 0.000001){
    //    return;
    //}
    glm::vec3 viewDirection = this->m_Front - this->m_Position;
    glm::vec3 newDirection(0.0f, 0.0f, 0.0f);
    float C = cosf(angle);
    float S = sinf(angle);
    glm::vec3 tempX(C + x*x*(1 - C), x*y*(1 - C) - z*S, x*z*(1 - C) + y*S);
    newDirection.x = (glm::dot(tempX, viewDirection));
    glm::vec3 tempY(x*y*(1 - C) + z*S, C + y*y*(1 - C), y*z*(1 - C) - x*S);
    newDirection.y = (glm::dot(tempY,viewDirection));
    glm::vec3 tempZ(x*z*(1 - C) - y*S, y*z*(1 - C) + x*S, C + z*z*(1 - C));
    newDirection.z = (glm::dot(tempZ,viewDirection));
    //newDirection.normalize();
    this->m_Front = this->m_Position + newDirection;

    glm::vec3 rightDirection = glm::cross(newDirection, this->m_WorldUp);
    //rightDirection.normalize();
    this->m_Up = glm::cross(rightDirection, newDirection);
    glm::normalize(this->m_Up);
}

void CCamera::UpdateDirection(int deltaX, int deltaY) {

    const float mouseSpeed = 0.005f;

// Compute new orientation
    horizontalAngle -= mouseSpeed * float(deltaX);
    verticalAngle   -= mouseSpeed * float(deltaY);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - PI /2.0f),
        0,
        cos(horizontalAngle - PI /2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross( right, direction );

    this->m_Front = this->m_Position + direction;
    this->m_Right = right;
    this->m_Up = up;

}

void CCamera::SwitchTo3D() {
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    ////gluPerspective(50.0f, (float)mViewportWidth / (float)mViewportHeight, 0.1f, 1000.0f);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}
void CCamera::SwitchTo2D() {
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    ////gluOrtho2D(-mViewportWidth / 2, mViewportWidth / 2, -mViewportHeight / 2, mViewportHeight / 2);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}
