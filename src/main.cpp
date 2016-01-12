
/*===============================================================*\

    Vendredi 25 septembre 2013

	Arash Habibi

	main.c

	Un programme equivalent à 02_glut.c et qui ne prend en compte
	que trois événements pour quitter le programme.

\*===============================================================*/
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>

#include <dirent.h>

extern "C" {
#include "Image.h"
#include "Ppm.h"
}

#define IMAGE_PATH "resource/"

#define INT_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#include "include/imgui/imgui.h"
#include "imgui_impl_glut.h"

#include "vertex.h"
#include "polygon.h"
#include "drawing.h"

using namespace std;

int w, h;

float red = 0.5, green = 0.5, blue = 0.5;
Color c = C_new(red, green, blue);
Color oldColor = C_new(0.0, 0.0, 0.0);

bool guiDrawing = true;
bool drawingMode = false;

bool line = false;
bool circle = false;
bool b_polygon = false;
bool fillColor = false;
int listbox_item_current = 0;
int pattern_item_current = 0;


bool firstRun = true;
bool show_analysis_window = true;

void DoGUI();
void Shutdown();

void Shutdown() {
    ImGui_ImplGLUT_Shutdown();
}

void computeCentroid(Polygon &p);

void Binarization();

void Grayscale();

void Erosion();

Image *img;

Image *pattern;

Drawing draw;

vector<Polygon> polygons;
Polygon p;
int poly_index = -1;
bool polygon_end = false;

DIR *dpdf;
struct dirent *epdf;

string files;
int file_item_current = 0;

int threshold = 180;

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

                c._red = red;
                c._green = green;
                c._blue = blue;

                oldColor = img->_buffer[x][img->_height - y];

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
                else if (b_polygon == true)
                {
                    if(polygon_end != true)
                    {
                        Vertex v(x, img->_height - y);
                        p.vertices.push_back(v);

                        if (p.vertices.size() != 1)
                        {
                            Vertex v1, v2;
                            int i = p.vertices.size();

                            v1 = p.vertices.at(i - 2);
                            v2 = p.vertices.at(i - 1);
                            draw.bresenham(img, v1.x, v1.y, v2.x, v2.y, c);
                        }
                    }
                }
                else if (fillColor == true)
                {

                    if (listbox_item_current == 0)
                    {
                        draw.floodFillRec(img, x, img->_height - y, c, oldColor, w, h);
                        cout << "4 way Flood fill Rec" << endl;
                    }
                    else if (listbox_item_current == 1)
                        draw.floodFillScanline(img, x, img->_height - y, c, oldColor, w, h);
                    else if (listbox_item_current == 2)
                        draw.floodFillNonRec(img, x, img->_height - y, c,oldColor, w, h);
                    else if (listbox_item_current == 3)
                        draw.floodFillPatternRec(img, pattern, x, img->_height - y, oldColor, w, h, x, img->_height - y);
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
    case 'z'    :
    {
        if (drawingMode)
        I_zoom(img,2.0);
        else
            io.AddInputCharacter(key);
        break;
    }
    case 'Z'    : I_zoom(img,0.5); break;
    case 'i'    :
    {
        if (drawingMode)
            I_zoomInit(img);
        else
            io.AddInputCharacter(key);
        break;
    }
    case 'p'    :
    {
        if (drawingMode)
        {
            int i = p.vertices.size();
            Vertex v1, v2;
            v1 = p.vertices.at(i - 1);
            v2 = p.vertices.at(0);
            draw.bresenham(img, v1.x, v1.y, v2.x, v2.y, c);

            // compute orientation
            int edgeSum = 0;
            for (size_t j = 0; j < p.vertices.size(); j++)
            {
                // (x2 - x1)(y2 + y1)
                if (j != p.vertices.size() - 1)
                    edgeSum += (p.vertices.at(j+1).x - p.vertices.at(j).x) * (p.vertices.at(j+1).y + p.vertices.at(j).y);
                else
                    edgeSum += (p.vertices.at(0).x - p.vertices.at(j).x) * (p.vertices.at(0).y + p.vertices.at(j).y);
            }

            if (edgeSum > 0)
                p.o = clockWise;
            else
                p.o = antiClockWise;
            p.getOrientation();

            computeCentroid(p);
            Color r;
            r._red = 1.0; r._green = 0.0; r._blue = 0.0;
            I_plotColor(img, p.centroid.x, img->_height - p.centroid.y, r);
            cout << "Polygon's centroid(screen coordiante):" << p.centroid << endl;

            if(p.isConvex(h))
                p.c = convex;
            else
                p.c = concave;

            cout << "Convexity(1:convex, 0:concave): " << p.c << endl;

            p.count++;
            polygons.push_back(p);

            poly_index ++;
            while (p.vertices.size() != 0)
                p.vertices.pop_back();
        }
        else
            io.AddInputCharacter(key);
        break;
    }
    case 'b'    :
    {
        if (drawingMode)
            Binarization();
        else
            io.AddInputCharacter(key);
        break;
    }
    case 'e'    :
    {
        if (drawingMode)
            Erosion();
        else
            io.AddInputCharacter(key);
        break;
    }
    case 'g'    :
        if (drawingMode)
            Grayscale();
        else
            io.AddInputCharacter(key);
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
    string imageName = "image2.jpg";
    string imagePath = IMAGE_PATH + imageName;
    char * imagePathC = new char[imagePath.length() + 1];
    strcpy(imagePathC, imagePath.c_str());


    dpdf = opendir(IMAGE_PATH);
    if (dpdf != NULL){
       while (epdf = readdir(dpdf)){
          files = files + epdf->d_name + ", ";
       }
    }
    cout << files << endl;

    pattern = I_read("resource/image2.jpg");

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

void computeCentroid(Polygon& p)
{
    int x0 =0, y0= 0;
    for (size_t i=0; i < p.vertices.size(); ++i)
    {
        x0 += p.vertices.at(i).x;
        y0 += img->_height - p.vertices.at(i).y;
    }

    p.centroid.change(x0/p.vertices.size(), y0/p.vertices.size());
}

void Grayscale()
{
    Color gray;
    for (int i = 0; i < img->_width; i++)
    {
        for (int j = 0; j < img->_height; j++)
        {
            gray._red = 0.299 * (img->_buffer[i][j]._red) + 0.587 * (img->_buffer[i][j]._green) + 0.114* (img->_buffer[i][j]._blue);
            gray._green = 0.299 * (img->_buffer[i][j]._red) + 0.587 * (img->_buffer[i][j]._green) + 0.114* (img->_buffer[i][j]._blue);
            gray._blue = 0.299 * (img->_buffer[i][j]._red) + 0.587 * (img->_buffer[i][j]._green) + 0.114* (img->_buffer[i][j]._blue);
            I_plotColor(img, i, j, gray);
        }
    }
}

void Binarization()
{
    float gray;
    int g;
    Color b,w;
    b._red = b._green = b._blue = 0.0;
    w._red = w._green = w._blue = 1.0;
    for (int i = 0; i < img->_width; i++)
    {
        for (int j = 0; j < img->_height; j++)
        {
            gray = 0.299 * (img->_buffer[i][j]._red) + 0.587 * (img->_buffer[i][j]._green) + 0.114* (img->_buffer[i][j]._blue);
            g = gray * 255;
            //cout << gray << endl;
            if (g > threshold)
                I_plotColor(img, i, j, w);
            else
                I_plotColor(img, i, j, b);
        }
    }
}

void Dilation()
{
    short mask[3][3] = {{0, 1, 0},
                        {1, 1, 1},
                        {0, 1, 0}};
    float max;
    for(int i=1; i< img->_width -1; i++)
    {
       //if( (i%10) == 0) printf("%3d", i);
       for(int j=1; j< img->_height-1; j++)
       {
          max = 0;
          for(int a=-1; a<=1; a++)
          {
              for(int b=-1; b<=1; b++){
                 if(mask[a+1][b+1] == 1){
                    if(img->_buffer[i+a][j+b]._red > max)
                       max = img->_buffer[i+a][j+b]._red;
                 }  /* ends if mask == 1 */
              }  /*  ends loop over b */
          }  /* ends loop over a */
          img->_buffer[j][i] = C_new(max, max, max);
       }  /* ends loop over j */
    }  /* ends loop over i */
}

void Erosion()
{
    short mask[3][3] = {{0, 1, 0},
                        {1, 1, 1},
                        {0, 1, 0}};
    float min;

//    Image * out_image;
//    out_image = I_read("resource/image2.jpg");

    for(int i=1; i< img->_width - 1; i++)
    {
       //if( (i%10) == 0) printf("%3d", i);
       for(int j=1; j< img->_height - 1; j++){
          min = 1.0;
          for(int a=-1; a<=1; a++){
              for(int b=-1; b<=1; b++){
                 if(mask[a+1][b+1] == 1){
                    if(img->_buffer[i+a][j+b]._red < min)
                       min = img->_buffer[i+a][j+b]._red;
                 }  /* ends if mask == 1 */
              }  /*  ends loop over b */
          }  /* ends loop over a */
          img->_buffer[j][i] = C_new(min, min, min);
       }  /* ends loop over j */
    }  /* ends loop over i */
}

void LabelComponent(int* output, int labelNo, unsigned short x, unsigned short y)
{
  int index = x + w*y;
  if (img->_buffer[x][y]._red == 0) return;   /* This pixel is not part of a component */
  if (output[index] != 0) return;   /* This pixel has already been labelled  */
  output[index] = labelNo;


  /* Now label the 4 neighbours: */
  if (x > 0)        LabelComponent(output, labelNo, x-1, y);   /* left  pixel */
  if (x < -1)  LabelComponent(output, labelNo, x+1, y);   /* right pixel */
  if (y > 0)        LabelComponent(output, labelNo, x, y-1);   /* upper pixel */
  if (y < -1) LabelComponent(output, labelNo, x, y+1);   /* lower pixel */
}

void LabelImage()
{
  int labelNo = 0;
  int index   = -1;
  int* output = (int*)malloc(w*h*sizeof(int));
  memset(output, 0, w*h*sizeof(int));
  for (unsigned short y = 0; y < img->_height; y++)
  {
    for (unsigned short x = 0; x < img->_width; x++)
    {
      index++;
      if (img->_buffer[x][y]._red == 0) continue;   /* This pixel is not part of a component */
      if (output[index] != 0) continue;   /* This pixel has already been labelled  */
      /* New component found */
      img->_buffer[x][y] = C_new(0.1 + labelNo * 0.2, 0.1 + labelNo * 0.2, 0.1 + labelNo * 0.2);
      labelNo++;
      LabelComponent(output, labelNo, x, y);
    }
  }
}



void DoGUI()
{
    ImGui_ImplGLUT_NewFrame(w, h, 1.0f / 30.0f);

    ImVec4 fill_color = ImColor(red, green, blue);
    const char* listbox_items[] = { "Recursif seed-fill", "Scanline seed-fill", "Non recursif seed-fill", "seed-fill with texture" };

    static char buf[64] = "";


    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(300,400));

    if (guiDrawing)
    {
        ImGui::Begin("Drawing",&show_analysis_window);
        line = ImGui::Button("Line");
        //cout << "Draw line mode :" << line << endl;
        ImGui::InputInt("Radius", &draw.radius);
        circle = ImGui::Button("Circle");
        ImGui::SliderFloat("Red", &red, 0.0, 1.0);
        ImGui::SliderFloat("Green", &green, 0.0, 1.0);
        ImGui::SliderFloat("Blue", &blue, 0.0, 1.0);
        ImGui::ColorEdit3("Color", (float*)&fill_color);
        ImGui::Combo("Method", &listbox_item_current, listbox_items, INT_ARRAYSIZE(listbox_items));

        fillColor = ImGui::Button("Color fill");

        b_polygon = ImGui::Button("Polygon");


        if (ImGui::Button("Rotate"))
        {
            if (polygons.empty() != true && polygons.size() - 1 <= poly_index)
            {
                Color b = C_new(0, 0, 0);
                draw.drawPoly(img, polygons.at(poly_index), b);
                polygons.at(poly_index).quarter(h);
                draw.drawPoly(img, polygons.at(poly_index), c);
            }
        }
        if (ImGui::Button("Increase size"))
        {
            if (polygons.empty() != true && polygons.size() - 1 <= poly_index)
            {
                Color b = C_new(0, 0, 0);
                draw.drawPoly(img, polygons.at(poly_index), b);
                polygons.at(poly_index).increase(h);
                draw.drawPoly(img, polygons.at(poly_index), c);
            }
        }
        if (ImGui::Button("Decrease size"))
        {
            if (polygons.empty() != true && polygons.size() - 1 <= poly_index)
            {
                Color b = C_new(0, 0, 0);
                draw.drawPoly(img, polygons.at(poly_index), b);
                polygons.at(poly_index).reduce(h);
                draw.drawPoly(img, polygons.at(poly_index), c);
            }
        }
        ImGui::SliderInt("Polygon", &poly_index, -1, 9);

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(320, 10));
        ImGui::SetNextWindowSize(ImVec2(300,400));
        ImGui::Begin("Analysis", &show_analysis_window);
        ImGui::InputText("File name", buf, 64);
        if (ImGui::Button("Open"))
        {
                string s(buf);
                if (s != "")
                {
                s = IMAGE_PATH + s;
                char * sPtr = new char[s.length() + 1];
                strcpy(sPtr, s.c_str());
                img= I_read(sPtr);
                w = img->_width;
                h = img->_height;
                free(sPtr);

                glViewport(0, 0, w, h);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glOrtho(0,w,0,h,-1,1);

                glutReshapeWindow(w, h);
                }
                else
                {
                    w = 800;
                    h = 600;
                    img = I_new(w, h);

                    glViewport(0, 0, w, h);

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();

                    glOrtho(0,w,0,h,-1,1);

                    glutReshapeWindow(w, h);
                }
        }
        if (ImGui::Button("Grayscale"))
        {
            Grayscale();
        }
        ImGui::SliderInt("Threshold", &threshold, 0, 255);
        if (ImGui::Button("Binarization"))
        {
            Binarization();
        }
        if (ImGui::Button("Erosion"))
        {
            cout << "erosion" << endl;
            Erosion();
        }
        if (ImGui::Button("Dilation"))
        {
            Dilation();
        }
        if (ImGui::Button("Label"))
        {
            LabelImage();
        }

        ImGui::End();

        ImGui::Render();
    }
}

