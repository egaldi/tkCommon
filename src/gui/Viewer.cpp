#include "tkCommon/gui/Viewer.h"

using namespace tk::gui;
MouseView3D     mouseView;

Viewer::Viewer() {
}


Viewer::~Viewer() {
}


inline void Mat4_identity(float M[16])
{
    memset(M, 0, 16*sizeof(float));
    M[0] = 1.0f;
    M[5] = 1.0f;
    M[10] = 1.0f;
    M[15] = 1.0f;
}

inline void Mat4_IsoInv(float res[16], const float A[16])
{
    //Transpose R
    res[0 + 0 * 4] = A[0 + 0 * 4];
    res[1 + 0 * 4] = A[0 + 1 * 4];
    res[2 + 0 * 4] = A[0 + 2 * 4];

    res[0 + 1 * 4] = A[1 + 0 * 4];
    res[1 + 1 * 4] = A[1 + 1 * 4];
    res[2 + 1 * 4] = A[1 + 2 * 4];

    res[0 + 2 * 4] = A[2 + 0 * 4];
    res[1 + 2 * 4] = A[2 + 1 * 4];
    res[2 + 2 * 4] = A[2 + 2 * 4];

    //ti = -Rt
    const float tx = A[0 + 3 * 4];
    const float ty = A[1 + 3 * 4];
    const float tz = A[2 + 3 * 4];
    res[0 + 3 * 4] = -(A[0 + 0 * 4] * tx + A[1 + 0 * 4] * ty + A[2 + 0 * 4] * tz);
    res[1 + 3 * 4] = -(A[0 + 1 * 4] * tx + A[1 + 1 * 4] * ty + A[2 + 1 * 4] * tz);
    res[2 + 3 * 4] = -(A[0 + 2 * 4] * tx + A[1 + 2 * 4] * ty + A[2 + 2 * 4] * tz);

    //Empty row
    res[3 + 0 * 4] = 0;
    res[3 + 1 * 4] = 0;
    res[3 + 2 * 4] = 0;
    res[3 + 3 * 4] = 1;
}

inline void Mat4_AxB(float res[16], const float A[16], const float B[16])
{
    res[0 + 0 * 4] = A[0 + 0 * 4] * B[0 + 0 * 4] + A[0 + 1 * 4] * B[1 + 0 * 4] + A[0 + 2 * 4] * B[2 + 0 * 4] + A[0 + 3 * 4] * B[3 + 0 * 4];
    res[1 + 0 * 4] = A[1 + 0 * 4] * B[0 + 0 * 4] + A[1 + 1 * 4] * B[1 + 0 * 4] + A[1 + 2 * 4] * B[2 + 0 * 4] + A[1 + 3 * 4] * B[3 + 0 * 4];
    res[2 + 0 * 4] = A[2 + 0 * 4] * B[0 + 0 * 4] + A[2 + 1 * 4] * B[1 + 0 * 4] + A[2 + 2 * 4] * B[2 + 0 * 4] + A[2 + 3 * 4] * B[3 + 0 * 4];
    res[3 + 0 * 4] = A[3 + 0 * 4] * B[0 + 0 * 4] + A[3 + 1 * 4] * B[1 + 0 * 4] + A[3 + 2 * 4] * B[2 + 0 * 4] + A[3 + 3 * 4] * B[3 + 0 * 4];

    res[0 + 1 * 4] = A[0 + 0 * 4] * B[0 + 1 * 4] + A[0 + 1 * 4] * B[1 + 1 * 4] + A[0 + 2 * 4] * B[2 + 1 * 4] + A[0 + 3 * 4] * B[3 + 1 * 4];
    res[1 + 1 * 4] = A[1 + 0 * 4] * B[0 + 1 * 4] + A[1 + 1 * 4] * B[1 + 1 * 4] + A[1 + 2 * 4] * B[2 + 1 * 4] + A[1 + 3 * 4] * B[3 + 1 * 4];
    res[2 + 1 * 4] = A[2 + 0 * 4] * B[0 + 1 * 4] + A[2 + 1 * 4] * B[1 + 1 * 4] + A[2 + 2 * 4] * B[2 + 1 * 4] + A[2 + 3 * 4] * B[3 + 1 * 4];
    res[3 + 1 * 4] = A[3 + 0 * 4] * B[0 + 1 * 4] + A[3 + 1 * 4] * B[1 + 1 * 4] + A[3 + 2 * 4] * B[2 + 1 * 4] + A[3 + 3 * 4] * B[3 + 1 * 4];

    res[0 + 2 * 4] = A[0 + 0 * 4] * B[0 + 2 * 4] + A[0 + 1 * 4] * B[1 + 2 * 4] + A[0 + 2 * 4] * B[2 + 2 * 4] + A[0 + 3 * 4] * B[3 + 2 * 4];
    res[1 + 2 * 4] = A[1 + 0 * 4] * B[0 + 2 * 4] + A[1 + 1 * 4] * B[1 + 2 * 4] + A[1 + 2 * 4] * B[2 + 2 * 4] + A[1 + 3 * 4] * B[3 + 2 * 4];
    res[2 + 2 * 4] = A[2 + 0 * 4] * B[0 + 2 * 4] + A[2 + 1 * 4] * B[1 + 2 * 4] + A[2 + 2 * 4] * B[2 + 2 * 4] + A[2 + 3 * 4] * B[3 + 2 * 4];
    res[3 + 2 * 4] = A[3 + 0 * 4] * B[0 + 2 * 4] + A[3 + 1 * 4] * B[1 + 2 * 4] + A[3 + 2 * 4] * B[2 + 2 * 4] + A[3 + 3 * 4] * B[3 + 2 * 4];

    res[0 + 3 * 4] = A[0 + 0 * 4] * B[0 + 3 * 4] + A[0 + 1 * 4] * B[1 + 3 * 4] + A[0 + 2 * 4] * B[2 + 3 * 4] + A[0 + 3 * 4] * B[3 + 3 * 4];
    res[1 + 3 * 4] = A[1 + 0 * 4] * B[0 + 3 * 4] + A[1 + 1 * 4] * B[1 + 3 * 4] + A[1 + 2 * 4] * B[2 + 3 * 4] + A[1 + 3 * 4] * B[3 + 3 * 4];
    res[2 + 3 * 4] = A[2 + 0 * 4] * B[0 + 3 * 4] + A[2 + 1 * 4] * B[1 + 3 * 4] + A[2 + 2 * 4] * B[2 + 3 * 4] + A[2 + 3 * 4] * B[3 + 3 * 4];
    res[3 + 3 * 4] = A[3 + 0 * 4] * B[0 + 3 * 4] + A[3 + 1 * 4] * B[1 + 3 * 4] + A[3 + 2 * 4] * B[2 + 3 * 4] + A[3 + 3 * 4] * B[3 + 3 * 4];
}

void 
Viewer::init() {
    glfwSetErrorCallback(errorCallback);
    glfwInit();

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, windowName.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
    }

    mouseView.window = window;

    glfwSetScrollCallback(window, Viewer::scroll_callback);
    glfwSetCursorPosCallback(window, Viewer::cursor_position_callback);
    glfwSetMouseButtonCallback(window, Viewer::mouse_button_callback);

    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    //gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    Mat4_identity(rig2world);        
}


void 
Viewer::draw() {
}

void Viewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
    mouseView.mouseWheel(xoffset, yoffset);
}

void Viewer::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseView.mouseMove(xpos, ypos);
}

void Viewer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (action == GLFW_PRESS)
        mouseView.mouseDown(button, xpos, ypos);
    if (action == GLFW_RELEASE)
        mouseView.mouseUp(button, xpos, ypos);
}

void 
Viewer::run() {

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        mouseView.setWindowAspect(float(width)/height);

        float tmp[16];
        memcpy(tmp, mouseView.getModelView(), 16 * sizeof(float));
        float pi[16];
        Mat4_IsoInv(pi, rig2world);
        Mat4_AxB(mview, tmp, pi);

        glPushMatrix();
        glMultMatrixf(mouseView.getProjection());
        glMultMatrixf(mview);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(float(background.r)/255, float(background.g)/255, float(background.b)/255, float(background.a)/255);

        draw();


        glPopMatrix();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}


void
Viewer::setWindowName(std::string name) {
    windowName = name;
}


void 
Viewer::setBackground(tk::gui::Color_t c) {
    background = c;
}


int 
Viewer::tkLoadTexture(std::string filename, GLuint &tex) {

    unsigned error;
    unsigned char* image;
    unsigned width, height;

    error = lodepng_decode32_file(&image, &width, &height, filename.c_str());
    if(error == 0) {
        if( (width % 32) != 0 || (height % 32) != 0)
            std::cout<<"please use images size multiple of 32\n";

        //upload to GPU texture
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glBindTexture(GL_TEXTURE_2D, 0);

        delete [] image;

        if(tex == 0)
            error = 1;
    } else {
        tex = 0;
    }

    std::cout<<"loading "<<filename<<"    ";
    if(error == 0)
        std::cout<<" OK!!\n";
    else
        std::cout<<" ERROR: "<<error<<"\n";

    return error;
}


int 
Viewer::tkLoadOBJ(std::string filename, object3D_t &obj) {

    int error = 0;
    std::cout<<"loading "<<filename<<".obj    ";
    
    // correct locale dependent stof
    std::setlocale(LC_ALL, "C");

    objl::Loader loader;
    if(loader.LoadFile((filename + ".obj").c_str())) {
   
        std::cout<<" OK!!\n";
        obj.triangles.resize(loader.LoadedMeshes.size());
        obj.colors.resize(loader.LoadedMeshes.size());

        for(int o=0; o<loader.LoadedMeshes.size(); o++) {
            std::cout<<"name: "<<loader.LoadedMeshes[o].MeshName<<"  verts: "<<loader.LoadedMeshes[o].Vertices.size()<<"\n";
   
            std::vector<unsigned int> indices = loader.LoadedMeshes[o].Indices;
            std::vector<objl::Vertex> verts = loader.LoadedMeshes[o].Vertices;

            obj.colors[o].x = loader.LoadedMeshes[o].MeshMaterial.Kd.X;
            obj.colors[o].y = loader.LoadedMeshes[o].MeshMaterial.Kd.Y;
            obj.colors[o].z = loader.LoadedMeshes[o].MeshMaterial.Kd.Z;
            std::cout<<"mat: "<<loader.LoadedMeshes[o].MeshMaterial.name<<" diffuse: "<<obj.colors[o]<<"\n";
            
            obj.triangles[o] = Eigen::MatrixXf(5, indices.size());
            for(int i=0; i<indices.size(); i++) {
                int idx = indices[i];
                obj.triangles[o](0,i) = verts[idx].Position.X;
                obj.triangles[o](1,i) = verts[idx].Position.Y;
                obj.triangles[o](2,i) = verts[idx].Position.Z;
                obj.triangles[o](3,i) = verts[idx].TextureCoordinate.X;
                obj.triangles[o](4,i) = 1 - verts[idx].TextureCoordinate.Y;
            }
        }


    } else {
        error = 1;
        std::cout<<" ERROR\n";
    }
    
    error = error || tkLoadTexture((filename + ".png"), obj.tex);
    return error;
}

void 
Viewer::tkSetColor(tk::gui::Color_t c) {
    glColor4ub(c.r, c.g, c.b, c.a);
}

void 
Viewer::tkApplyTf(tk::common::Tfpose tf) {
    // apply roto translation
    tk::common::Vector3<float> p = tk::common::tf2pose(tf);
    tk::common::Vector3<float> r = tk::common::tf2rot (tf);
    glTranslatef(p.x, p.y, p.z);
    glRotatef(r.x*180.0/M_PI, 1, 0, 0);
    glRotatef(r.y*180.0/M_PI, 0, 1, 0);
    glRotatef(r.z*180.0/M_PI, 0, 0, 1);
}  

void 
Viewer::tkDrawAxis(float s) {
    glLineWidth(2.5); 
    glBegin(GL_LINES);
    // x
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(s, 0, 0);
    // y
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, s, 0);
    // z
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, s);
    glEnd();
}

void 
Viewer::tkDrawCircle(float x, float y, float z, float r, int res) {
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < res; j++)   {
        float theta = 2.0f * 3.1415926f * float(j) / float(res);//get the current angle 
        float xr = r * cosf(theta);//calculate the x component 
        float yr = r * sinf(theta);//calculate the y component 
        glVertex3f(x + xr, y + yr, z);//output vertex
    }
    glEnd(); 
}

void 
Viewer::tkDrawCloud(Eigen::MatrixXf *points) {
    glBegin(GL_POINTS);
    for (int p = 0; p < points->cols(); p++) {
        Eigen::Vector4f v = points->col(p);
        glVertex3f(v(0), v(1), v(2));
    }
    glEnd();
}

void 
Viewer::tkDrawArrow(float length, float radius, int nbSubdivisions) {
    
    /*static GLUquadric *quadric = gluNewQuadric();

    if (radius < 0.0)
        radius = 0.05 * length;

    const float head = 2.5 * (radius / length) + 0.1;
    const float coneRadiusCoef = 4.0 - 5.0 * head;

    gluCylinder(quadric, radius, radius, length * (1.0 - head / coneRadiusCoef),
                nbSubdivisions, 1);
    glTranslated(0.0, 0.0, length * (1.0 - head));
    gluCylinder(quadric, coneRadiusCoef * radius, 0.0, head * length,
                nbSubdivisions, 1);
    glTranslated(0.0, 0.0, -length * (1.0 - head));*/
}

void 
Viewer::tkDrawArrow(tk::common::Vector3<float> pose, float yaw, float lenght, float radius, int nbSubdivisions) {
    glPushMatrix();
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(90.0 + yaw*180/M_PI, 0.0, 1.0, 0.0);
    glTranslatef(pose.x, pose.y, pose.z);
    tkDrawArrow(lenght, radius, nbSubdivisions);
    glPopMatrix();
}

void 
Viewer::tkDrawCube(tk::common::Vector3<float> pose, tk::common::Vector3<float> size, bool filled) {
    if (!filled)
        glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;

    glPushMatrix();
    glTranslatef(pose.x, pose.y, pose.z);
    glScalef(size.x, size.y, size.z);

    // BACK
    glBegin(GL_POLYGON);
    glVertex3f(  0.5, -0.5, 0.5 );
    glVertex3f(  0.5,  0.5, 0.5 );
    glVertex3f( -0.5,  0.5, 0.5 );
    glVertex3f( -0.5, -0.5, 0.5 );
    glEnd();
    
    // RIGHT
    glBegin(GL_POLYGON);
    glVertex3f( 0.5, -0.5, -0.5 );
    glVertex3f( 0.5,  0.5, -0.5 );
    glVertex3f( 0.5,  0.5,  0.5 );
    glVertex3f( 0.5, -0.5,  0.5 );
    glEnd();
    
    // LEFT
    glBegin(GL_POLYGON);
    glVertex3f( -0.5, -0.5,  0.5 );
    glVertex3f( -0.5,  0.5,  0.5 );
    glVertex3f( -0.5,  0.5, -0.5 );
    glVertex3f( -0.5, -0.5, -0.5 );
    glEnd();
    
    // TOP
    glBegin(GL_POLYGON);
    glVertex3f(  0.5,  0.5,  0.5 );
    glVertex3f(  0.5,  0.5, -0.5 );
    glVertex3f( -0.5,  0.5, -0.5 );
    glVertex3f( -0.5,  0.5,  0.5 );
    glEnd();
    
    // BOTTOM
    glBegin(GL_POLYGON);
    glVertex3f(  0.5, -0.5, -0.5 );
    glVertex3f(  0.5, -0.5,  0.5 );
    glVertex3f( -0.5, -0.5,  0.5 );
    glVertex3f( -0.5, -0.5, -0.5 );
    glEnd();

    glPopMatrix();

    if (!filled)
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL ) ;
}

void 
Viewer::tkDrawObject3D(object3D_t *obj, float size, bool textured) {

    glPushMatrix();
    glScalef(size, size, size);

    if(textured) {
        glBindTexture(GL_TEXTURE_2D, obj->tex);
        glEnable(GL_TEXTURE_2D);
    }

    glBegin(GL_TRIANGLES);
    for(int o=0; o<obj->triangles.size(); o++) {
        if(!textured)
            glColor3f(obj->colors[o].x, obj->colors[o].y, obj->colors[o].z);

        for(int i=0; i<obj->triangles[o].cols(); i++) {
            glTexCoord2f(obj->triangles[o](3,i), obj->triangles[o](4,i)); 
            glVertex3f(obj->triangles[o](0,i),obj->triangles[o](1,i),obj->triangles[o](2,i));
        }
    }
    glEnd();

    if(textured) {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glPopMatrix();
}

void Viewer::tkDrawTexture(GLuint tex, float s) {

    float i = -s/2;
    float j = +s/2;

    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    /*
    glTexCoord2f(0, 0); glVertex3f(i, i, 0);
    glTexCoord2f(0, 1); glVertex3f(i, j, 0);
    glTexCoord2f(1, 1); glVertex3f(j, j, 0);
    glTexCoord2f(1, 0); glVertex3f(j, i, 0);
    */

    // 2d draw
    glTexCoord2f(0, 1); glVertex3f(i, i, 0);
    glTexCoord2f(0, 0); glVertex3f(i, j, 0);
    glTexCoord2f(1, 0); glVertex3f(j, j, 0);
    glTexCoord2f(1, 1); glVertex3f(j, i, 0);


    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Viewer::tkViewport2D(int width, int height) {
    //This sets up the viewport so that the coordinates (0, 0) are at the top left of the window
    glViewport(0, 0, width, height);  
    float ratio = (float) width / (float) height;
    glOrtho(0, width, height, 0, -10, 10);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Back to the modelview so we can draw stuff 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void 
Viewer::errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void
Viewer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}









