#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <gl/stb_image.h>
#include <iostream>
#include <gl/glut.h>



//snake keyboard movement
#define UP 1
#define Down 2
#define LEFT 3
#define RIGHT 4

// Status Variables
GLint   lvl = 1;
GLint   points = 0;
GLint   size = 0;
GLbyte  gameOver = true;
GLbyte  EnableLight = true;

//texture 
GLuint texture;
int width, height, nrChannels;
unsigned char* data = NULL;

// Snake Variables
GLint   bodyPos[2][100] = { {} };      //intial position of snake
GLint   _x = 5;
GLint   _z = 10;
GLint   _oldX[2] = {};
GLint   _oldZ[2] = {};
GLbyte  direction = 0;

// Food Variables
GLint   _bx = 0;
GLint   _bz = 0;

// Screen variables
GLint   _w = 800;
GLint   _h = 550;
GLint   _Giw = 0;
GLint   _Gih = 0;
GLint   _Gfw = 150;    //plane where is moving on width
GLint   _Gfh = 150;    //plane where is moving on height

//Variables for the Camera Angle
static GLfloat view_rotx = 45.0F;
static GLfloat view_roty = 45.0F;
static GLfloat view_rotz = 0.0F;

static GLfloat headRotation = 90.0F;
static GLfloat zoom = -300.0f;      //zoom in & out
//Configure the lightning
void initLight()
{
    //Add ambient light
    GLfloat ambientColor[] = { 0.3f, 0.4f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    //Add positioned light
    GLfloat lightColor0[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat lightPos0[] = { 4.0f, 0.0f, 8.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    //Add directed light
    GLfloat lightColor1[] = { 0.5f, 0.2f, 0.2f, 1.0f };
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = { -1.0f, 0.5f, 0.5f, 0.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}
//initialize the first configurations
void Initialize(void)
{
    glEnable(GL_DEPTH_TEST);           //stores fragments their z-values in the depth buffer
    glClearColor(0, 100, 10, 1.0); //Change the background to sky blue
    if (EnableLight) {
        glEnable(GL_COLOR_MATERIAL); //Enable color
        glEnable(GL_LIGHTING); //Enable lighting
        glEnable(GL_LIGHT0); //Enable light #0
        glEnable(GL_LIGHT1); //Enable light #1
        glEnable(GL_NORMALIZE); //Automatically normalize normals
    }
}
//Configure window resize
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); //to display
    glLoadIdentity();               //resets matrix
    gluPerspective(45.0, (double)w / (double)h, 1, 800.0);         ////view angle y , ratio , near view ,far view
}
void Write(const char* string) {//Write string on the screen
    while (*string)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string++);
    // glutBitmapCharacter renders a bitmap character
}
//This Function will rotate the object according to the Angles
void ManipulateViewAngle() 
{ //rotx 45 roty 0 rotz 0
    glRotatef(view_rotx, 1.0, 0.0, 0.0);//Rotate Arround X axis
    glRotatef(view_roty, 0.0, 1.0, 0.0);//Rotate Arround Y axis
    glRotatef(view_rotz, 0.0, 0.0, 1.0);//Rotate Arround Z axis
}
//This Function will reset the snake size and location...
void Reset()
{
    _x = 5;
    _z = 10;
    direction = 0;
    lvl = 1;
    points = 0;
    size = 0;
    gameOver = false;
    view_rotx = 45.0F;
    view_roty = 0.0F;
    view_rotz = 0.0F;
    headRotation = 90.0F;
}
//Display a welcome screen

void WelcomeScreen() {
    char tmp_str[40];
    
    glRasterPos2f(-40, 20); // to position the text
    Write("=========Simulation of Snake Game=========");
    glRasterPos2f(-60, 0);
    Write("=========To Start Playing please press 'S' or 's' =========");
    
}
void check(unsigned char* data)
{
    data = stbi_load("images.jpeg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "[!] failed to load the texture";
        
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    
}
/*void text()
{
    // creating a variable of type open gl unsigend intgear to store texture object name
    GLuint texture;

    //genearate one texture object name and assign it to texture variable
    glGenTextures(1, &texture);

    //bind texture object with the texture name
    glBindTexture(GL_TEXTURE_2D, texture);

    //filtering image (scale down, scale up)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load the texture food.bmp
    //int width, height, channels;
    unsigned char* data = stbi_load("images.jpeg", &width, &height, &nrChannels, 0);

    // if image loaded succsesfully
    if (data)
    {
        // specify texture image for currently bounded texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //free the memory allocated for the image
        stbi_image_free(data);
    }

    // if image isnot loaded
    else
    {
        std::cout << "[!] failed to load the texture";
    }
    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, texture);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
}*/
void drawbox() {

    float tra_x = 0.0f;
    float tra_y = 0.0f;
    float tra_z = 0.0f;
    glTranslatef(tra_x, tra_y, tra_z);

    glBegin(GL_QUADS);


    glLoadIdentity();
    
    
    // FRONT
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    

    // BACK
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    //glColor3f(0.0f, 1.0f, 0.0f);
    // LEFT
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2d(1.0f, 1.0f); 
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    // RIGHT
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    // TOP
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    // BOTTOM
    glTexCoord2d(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2d(0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glEnd();
}
void DrawSnake() {
    int  i;
    //Drawing the head & the plane
    glPushMatrix();
    //rotate the snake according to the angles
    ManipulateViewAngle();
    //This will draw the plane that the snake will run on.
    glPushMatrix();
    //glColor3f(1, 0.7, 0.2);
    glColor3f(0.0, 0.6, 0.2);
    glTranslatef(75.0, -16.0, 75.0);
    glScalef(155, 5.0, 155);
    drawbox();      //creates a the plane  
    glPopMatrix();
    //Here we will draw the Head of the snake
    glColor3f(1, 0, 0);//Color it red
    glTranslatef(_x, -10.0, _z);//Give it the location according to _x & _z
    glScalef(0.5, 0.5, 0.5);
    glutSolidSphere(10, 20, 20);//Draw the head as a sphere a litle bit bigger than the body spheres
    glRotatef(headRotation, 0.0, 1.0, 0.0);
    glColor3f(1, 0, 0);
    glTranslatef(0, 0.0, 6.0);
    glScalef(0.8, 1.0, 1.0);
    glutSolidCone(10, 10, 20, 20);     //the snake's first sphere has a cone attached to it
    glColor3f(1, 1, 1);
    glTranslatef(-4.0, 10.0, 0.0);
    glScalef(0.3, 0.3, 0.3);
    glutSolidSphere(10, 20, 20);                     //eyes
    glTranslatef(26.0, 0.0, 0.0);
    glutSolidSphere(10, 20, 20);                      //eyes
    glPopMatrix();

    //Drawing the body
    for (i = 0; i < size; i++) {//Loop throw the size and draw spheres representing the body
        glPushMatrix();
        ManipulateViewAngle();
        glTranslatef(bodyPos[0][i], -10.0, bodyPos[1][i]);//this will locate the spheres
        glColor3f(1, 0, 0);//Color Red
        glScalef(0.5, 0.5, 0.5);
        glutSolidSphere(7, 20, 20);
        glPopMatrix();
    }
}
void DrawFood()
{
    //Draw the Sphere representing the Food for the snake
    glPushMatrix();
    ManipulateViewAngle();
    glTranslatef(_bx, -10.0, _bz);
    glColor3f(1, 1, 0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidSphere(7, 20, 20);
    glPopMatrix();
}
void GameStatus() {
    char tmp_str[40];
    glColor3f(0.8, 0.2, 0);
    glRasterPos2f(5, 20);
    sprintf_s(tmp_str, "Snake Game");
    Write(tmp_str);
    // Print the status of the game on the screen
    glColor3f(0, 0, 0);
    glRasterPos2f(5, 10);
    sprintf_s(tmp_str, "Level: %d Points: %d", lvl, points);
    Write(tmp_str);
}
//Generates Random Numbers for the location of the food that the snake will eat
int RandomNumber(int high, int low)
{
    return (rand() % (high - low)) + low;
}
//Generate the New Food that the snake will eat
void newFood() {
    time_t seconds;
    time(&seconds);
    srand((unsigned int)seconds);
    //gfw/h 150 gih/w 0
    _bx = RandomNumber(_Gfw - _Giw, _Giw + 10);
    _bz = RandomNumber(_Gfh - _Gih, _Gih + 10);
}
//This Function Will Check for Collision
bool collision() {
    //_x=5 and _z=10
    // bodyPos[2][100] = { {} }=> starting snake position 
    int i;
    for (i = 0; i < size; i++) {
        if ((bodyPos[0][i] == _x && bodyPos[1][i] == _z) || ((bodyPos[0][i] >= _x) && (bodyPos[0][i] <= _x + 5) && (bodyPos[1][i] <= _z) && (bodyPos[1][i] >= _z - 5)))
            return true;
    }
    return false;
}
//((bodyPos[0][i] >= _x) && (bodyPos[0][i] <= _x + 5) && (bodyPos[1][i] >= _z) && (bodyPos[1][i] <= _z + 5)) ||
         //((bodyPos[0][i] <= _x) && (bodyPos[0][i] >= _x - 5) && (bodyPos[1][i] <= _z) && (bodyPos[1][i] >= _z - 5)) ||
         //((bodyPos[0][i] <= _x) && (bodyPos[0][i] >= _x - 5) && (bodyPos[1][i] >= _z) && (bodyPos[1][i] <= _z + 5)) ||
/*------------------------------------------------------------------------------------*/
//This Function will move the snake according to the directions
//Taken from the Keyboard keys
void Run(int value) {
    int i;
    _oldX[1] = _x;
    _oldZ[1] = _z;
    switch (direction) {
    case RIGHT:
        headRotation = 90;
        _x += 6;
        if (_x > _Gfw - 2) _x = _Giw - 1;//This will check if the snake is going into the border so it will appear on the other side
        break;
    case LEFT:
        headRotation = -90;
        _x -= 6;
        if (_x < 0) _x = _Gfw - 2;//This will check if the snake is going into the border so it will appear on the other side
        break;
    case UP:
        headRotation = 0;
        _z += 6;
        if (_z > _Gfh - 2) _z = _Gih - 1;//This will check if the snake is going into the border so it will appear on the other side
        break;
    case Down:
        headRotation = 180;
        _z -= 6;
        if (_z < 2) _z = _Gfh - 2;//This will check if the snake is going into the border so it will appear on the other side
        break;
    }
    //Checks for Collisoin if yes Game Over
    if (collision())
    {
        gameOver = true;
        
    }
    //Checks if the snake ate the food (check the X and Y)
    // If yes it will increase the points & the size of the snake & create a new food
    if ((_x == _bx && _z == _bz) ||
        ((_x >= _bx) && (_x <= _bx + 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
        ((_x <= _bx) && (_x >= _bx - 4) && (_z <= _bz) && (_z >= _bz - 4)) ||
        ((_x <= _bx) && (_x >= _bx - 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
        ((_x >= _bx) && (_x <= _bx + 4) && (_z <= _bz) && (_z >= _bz - 4))) {
        points++;
        if (points < 100) size++;
        if (points % 5 == 0 && lvl < 15) lvl++;
        newFood();
    }

    for (i = 0; i < size; i++) {
        // Save the positions of the body parts
        _oldX[0] = _oldX[1];
        _oldZ[0] = _oldZ[1];
        _oldX[1] = bodyPos[0][i];
        _oldZ[1] = bodyPos[1][i];
        bodyPos[0][i] = _oldX[0];
        bodyPos[1][i] = _oldZ[0];
    }

    //Set the Timer
    glutTimerFunc(130, Run, 0);
}

void Display(void) {//Draw Function
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0.0f, 0.0f, 3.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);
   
   
    if (EnableLight) initLight();           

    glTranslatef(-60.0, 40.0, zoom);
    //Check if the value of the Flag "Game Over is not True it will continue the game"
    if (!gameOver) {
        GameStatus();
        DrawFood();
        DrawSnake();
    }
    else
    {
        
        //char tmp_str[40];
        //glRasterPos2f(-40, 20); // to position the text
                //Write("GAME OVER!!");
       // glrasterpos2f(-60, -20); // to position the text
       // write("press r to restart or s to go for main menu");

 
        WelcomeScreen();
    }
    // Updates the screen
    glutPostRedisplay();
    glutSwapBuffers();
}
void Special(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_RIGHT:
        if (direction != LEFT)
            direction = RIGHT;
        break;
    case GLUT_KEY_LEFT:
        if (direction != RIGHT)
            direction = LEFT;
        break;
    case GLUT_KEY_UP:
        if (direction != UP)
            direction = Down;
        break;
    case GLUT_KEY_DOWN:
        if (direction != Down)
            direction = UP;
        break;
    }
}
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'S': Reset();
        glutPostRedisplay();
        break;
    case 's': Reset();
        glutPostRedisplay();
        break;
        //ESC to Exit
    case 27:
        exit(0);
        break;
    case 'R':Reset();
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(_w, _h);
    glutInitWindowPosition(80, 80);
    glutCreateWindow("Snake Game");
    Initialize();
    glutSpecialFunc(Special);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(Display);
    glutReshapeFunc(resize);
    newFood();
    Run(0);
    glutMainLoop();
}