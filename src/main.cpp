
/*===============================================================*\

    Vendredi 25 septembre 2013

	Arash Habibi

	main.c

	Un programme equivalent à 02_glut.c et qui ne prend en compte
	que trois événements pour quitter le programme.

\*===============================================================*/
#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>
extern "C" {
#include "Image.h"
#include "Ppm.h"
}

#define IMAGE_PATH "resource/"

#define INT_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#include "include/imgui/imgui.h"
#include "imgui_impl_glut.h"

#include "drawing.h"

using namespace std;

int w, h;

float red = 0.0, green = 0.9, blue = 0.0;

bool guiDrawing = true;
bool drawingMode = false;

bool line = false;
bool circle = false;
bool polygon = false;
bool fillColor = false;
int listbox_item_current = 0;

bool firstRun = true;
bool show_analysis_window = true;

void DoGUI();
void Shutdown();

void Shutdown() {
    ImGui_ImplGLUT_Shutdown();
}

Image *img;

Drawing draw;

//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------

void display_CB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    I_draw(img);

    ImGui_ImplGLUT_Init();

    DoGUI();


    //glPopMatrix();
    glutSwapBuffers();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir en fonction de la position de
// la souris (x,y), en fonction du bouton de la souris qui a été
// pressé ou relaché.
//------------------------------------------------------------------

void mouse_CB(int button, int state, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    if (drawingMode == true)
    {
        if((button == GLUT_LEFT_BUTTON))
        {
            if (state == GLUT_DOWN)
            {
                io.MouseDown[0] = true;
                I_focusPoint(img,x,img->_height-y);

                Color c = C_new(red, green, blue);
                //I_plotColor(img, x,img->_height-y,c);

                if (line == true)
                {
                    if (draw.bresenham_phase_1 == true)
                    {
                        draw.bresenham_x1 = x;
                        draw.bresenham_y1 = img->_height - y;
                        draw.bresenham_phase_2 = true;
                        draw.bresenham_phase_1 = false;
                    }
                    else
                    {
                        draw.bresenham(img, draw.bresenham_x1, draw.bresenham_y1, x, img->_height - y, c);
                        draw.bresenham_phase_1 = true;
                        draw.bresenham_phase_2 = false;
                    }
                }
                else if (circle == true)
                {
                    draw.circle(img, x, img->_height - y, draw.radius, c);
                }
                else if (polygon == true)
                {

                }
                else if (fillColor == true)
                {

                    if (listbox_item_current == 0)
                    {
                        draw.floodFillRec(img, x, img->_height - y, c, w, h);
                        cout << "Flood fill" << img->_current_color._red << endl;
                    }
                    else if (listbox_item_current == 1)
                        ;
                    else if (listbox_item_current == 2)
                        draw.floodFillNonRec(img, x, img->_height - y, c, w, h);
                    else if (listbox_item_current == 3)
                        ;
                }
            }
            else
            {
                io.MouseDown[0] = false;
            }
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            if (state == GLUT_DOWN) {
                io.MouseDown[1] = true;
            } else
            {
                io.MouseDown[1] = false;
            }
        }
    }
    else if (guiDrawing == true)
    {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                io.MouseDown[0] = true;
            } else /* GLUT_UP */ {
                io.MouseDown[0] = false;
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_DOWN) {
                io.MouseDown[1] = true;
            } else /* GLUT_UP */ {
                io.MouseDown[1] = false;
            }
        }
    }
    glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------

void keyboard_CB(unsigned char key, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();

    switch(key)
    {
    case 9      : {
        if (guiDrawing)
        {
            guiDrawing = false;
            drawingMode = true;
            printf("GUI closed. Drawing mode is actived.\n");
        }
        else
        {
            guiDrawing = true;
            drawingMode = false;
            printf("GUI opened. Drawing mode is not actived.\n");
        }
    };
        break;
    case 27     : {
        Shutdown();
        exit(1);
    }
        break;
    case 'z'    : I_zoom(img,2.0); break;
    case 'Z'    : I_zoom(img,0.5); break;
    case 'i'    : I_zoomInit(img); break;
    case 8      : {
        io.KeysDown[key] = true; break; // Backspace down
    }
    case 13     : {
        io.KeysDown[key] = true; break; // Enter down
    }
    default     : io.AddInputCharacter(key);
        //fprintf(stderr,"keyboard_CB : %d : unknown key.\n",key);
    }
	glutPostRedisplay();
}

void keyUp_CB(unsigned char key, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;
    io.MousePos = ImVec2((float)x, (float)y);
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche spéciale (F1, F2 ... F12, home, end, insert,
// haut, bas, droite, gauche etc).
//------------------------------------------------------------------

void special_CB(int key, int x, int y)
{
	// int mod = glutGetModifiers();

	int d = 10;

	switch(key)
	{
	case GLUT_KEY_UP    : I_move(img,0,d); break;
	case GLUT_KEY_DOWN  : I_move(img,0,-d); break;
	case GLUT_KEY_LEFT  : I_move(img,d,0); break;
	case GLUT_KEY_RIGHT : I_move(img,-d,0); break;
	default : fprintf(stderr,"special_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

void mouseDrag_CB(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

void mouseMove_CB(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

//------------------------------------------------------------------------

int main(int argc, char* argv[])
{
//    int largeur, hauteur;
//    if(argc==2)
//    {
//        img = I_read(argv[1]);
//        largeur = img->_width;
//        hauteur = img->_height;
//    }
//    else
//    {
//        largeur = atoi(argv[1]);
//        hauteur = atoi(argv[2]);
//        img = I_new(largeur,hauteur);
//        Color rouge = C_new(100,0,0);
//        Color blanc = C_new(200,200,255);
//        I_checker(img,rouge,blanc,50);
//    }
    // image path as string then convert to char * need free it at the end.
    string imageName = "";
    string imagePath = IMAGE_PATH + imageName;
    char * imagePathC = new char[imagePath.length() + 1];
    strcpy(imagePathC, imagePath.c_str());

    if (imageName == "")
    {
        w = 800;
        h = 600;
        img = I_new(w, h);
    }
    else
    {
        img= I_read(imagePathC);
        w = img->_width;
        h = img->_height;
    }

    int windowPosX = 600, windowPosY = 300;

    glutInitWindowSize(w, h);
    glutInitWindowPosition(windowPosX,windowPosY);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
    glutInit(&argc, argv);
    glutCreateWindow("FAIN");

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glOrtho(0,w,0,h,-1,1);

    glutDisplayFunc(display_CB);
    glutKeyboardFunc(keyboard_CB);
    glutKeyboardUpFunc(keyUp_CB);
    glutSpecialFunc(special_CB);
    glutMouseFunc(mouse_CB);
    glutMotionFunc(mouseDrag_CB);
    glutPassiveMotionFunc(mouseMove_CB);
    atexit(Shutdown);
    glutMainLoop();

    free(imagePathC);

    return 0;
}

void DoGUI()
{
    ImGui_ImplGLUT_NewFrame(w, h, 1.0f / 60.0f);

    ImVec4 clear_color = ImColor(red, green, blue);
    const char* listbox_items[] = { "Recursif seed-fill", "Scanline seed-fill", "Non recursif seed-fill", "seed-fill with texture" };


    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(300,400));

    if (guiDrawing)
    {
        ImGui::Begin("Drawing",&show_analysis_window);
        line = ImGui::Button("Line");
        //cout << "Draw line mode :" << line << endl;
        ImGui::InputInt("Radius", &draw.radius);
        circle = ImGui::Button("Circle");
        ImGui::ColorEdit3("Color", (float*)&clear_color);
        ImGui::Combo("Method", &listbox_item_current, listbox_items, INT_ARRAYSIZE(listbox_items), 4);
        fillColor = ImGui::Button("Color fill");


        polygon = ImGui::Button("Polygon");

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(320, 10));
        ImGui::SetNextWindowSize(ImVec2(300,400));
        ImGui::Begin("Analysis", &show_analysis_window);
        ImGui::End();

        ImGui::Render();
    }
}
