#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include "mouse.h"
#include "button.h"

int winw = 1280;
int winh = 800;

const float Y {26.};
const float X {4.1};
const size_t N {1000}; //Итератор
const double M {4.08};

const double alpha {.341}; // vat na s m ^2 *K Коэффициент теплопередачи контейнера(измеренный)
const double s_out {16.25}; // Общая площадь стенок снаружи, кв.м 
const double s_in {12.58}; //Общая площадь стенок внутри, кв.м
const double s_avrg {14.30}; //Общая площадь стенок средняя, кв.м
const double c_density {35.}; // ПСБС-50 (35 кг/м³ - 50 кг/м³),
const double y_out {22.+273}; //температура окружающей среды
const double y_in {4.+273};  //начальную температуру тела
const double h {.9}; //Толщина стенки


template<typename Func, typename T>
T d_solve(const Func& f, T x0, T y0, const T& x)
{   
    T P1, P2, P3, P4;
    T h {(x - x0)/N};
    T y {y0};
    for (size_t i = 0; i < N; i++)
    {
        P1 = f(x0 + i*h, y);
        P2 = f(x0 + i*h + h/2, y + h*P1/2);
        P3 = f(x0 + i*h + h/2, y + h*P2/2);
        P4 = f(x0 + i*h + h, y + h*P3);
        y += (h/6)*(P1 + 2*P2 + 2*P3 + P4);
    }
    return y;
}
////////////! Lambda
static auto funk ([](auto x, auto y){
        return alpha*s_out*(y_out - y)*x/(h*c_density);
    });
static auto funk_solv ([](auto x){
        return y_out - (y_out - y_in)*exp(-(alpha*s_out/(c_density))*x);
    });
////////////////!
void TheButtonCallback()
{
    std::vector<double> x0;
    for (double i = 0.; i < M; i+=.25) x0.push_back(i);
        for(const auto& t : x0)
            {
                std::cout <<"t = " << t*60 << " hour, T(t) = " 
                << d_solve(funk, x0[0], y_in, t) - 273 <<  " vs " << funk_solv(t) - 273 << "\n" ;
            }
            std::cout << std::endl;
    std::cout  << "\n--------------------------------------" << std::endl;
}
Button MyButton = {5,5, 100,25, 0,0, (char*)"Button", TheButtonCallback };

void MouseMotion(int x, int y)
{
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;
    TheMouse.x = dx;
	TheMouse.y = dy;
    ButtonPassive(&MyButton,x,y);
    glutPostRedisplay();
}
void MousePassiveMotion(int x, int y)
{
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;
    TheMouse.x = dx;
	TheMouse.y = dy;
    ButtonPassive(&MyButton,x,y);
}
void MouseButton(int button,int state,int x, int y)
{
	TheMouse.x = x;
	TheMouse.y = y;
    if (state == GLUT_DOWN) 
	{
		if ( !(TheMouse.lmb || TheMouse.mmb || TheMouse.rmb) ) {
			TheMouse.xpress = x;
			TheMouse.ypress = y;
		}
        switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 1;
			ButtonPress(&MyButton,x,y);
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 1;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 1;
			break;
		}
	}
	else 
	{
		switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 0;
			ButtonRelease(&MyButton,x,y);
			break;
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 0;
			break;
		}
	}
    glutPostRedisplay();
}
///----------------------------------------------------------------------------------------
void init() {
    glClearColor(0.25, 0.0, 0.2, 1.0);
    glEnable(GL_LIGHT0);
}
///----------------------------------------------------------------------------------------
void glWrite(float x, float y, int *font, const char* text, size_t kls) {
    glRasterPos2f(x, y);
    for (size_t i  =0; i < kls; i++)
    glutBitmapCharacter(font, text[i]);
}

void Draw_Graph()
{
	std::vector<double> x0;
    for (double i = 0.; i < M; i+=.25)
    {
        x0.push_back(i);
    }
    {
        glColor3f(1,1,0);
        glWrite(-.003, -.001, (int*)GLUT_BITMAP_HELVETICA_18, (char*)"(0, 0)", 6);
        glWrite(-.003, Y-.8, (int*)GLUT_BITMAP_HELVETICA_18, (char*)"T(t)", 5);
        glWrite(X-.05, -.01, (int*)GLUT_BITMAP_HELVETICA_18, (char*)"t", 2);
        for(const auto& t : x0)
            {
                char buffer[80];
                sprintf(buffer, "(%f, %f)", t*60, d_solve(funk, x0[0], y_in, t)-273);
                glWrite(t, d_solve(funk, x0[0], y_in, t) - 273, (int*)GLUT_BITMAP_HELVETICA_10, buffer, 22);
            }
    }
        glBegin(GL_LINES);
            glColor3f(1, 0, 0); glVertex2f(-20, 0); glVertex2f(400, 0);
            glColor3f(0, 0, 1); glVertex2f(0, -100); glVertex2f(0, 100);
        glEnd();
        glBegin(GL_LINE_STRIP);
            for(const auto& t : x0)
            {
                glColor3f(0, 1, 0); glVertex2f(t, d_solve(funk, x0[0], y_in, t)-273);
            }
        glEnd();
        glBegin(GL_LINE_STRIP);
            for(const auto& t : x0)
            {
                glColor3f(0, 0, 1); glVertex2f(t, funk_solv(t)-273);
            }
        glEnd();
    glFlush();
}
//----------------------------------------------------------------------------------------
void Draw2D()
{
	ButtonDraw(&MyButton);
}
//----------------------------------------------------------------------------------------
void Draw()
{
	glClear( GL_COLOR_BUFFER_BIT |
			 GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.01,X,-2,Y,.0,1.);
	Draw_Graph();
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho(0,winw,winh,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    Draw2D();
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    const char* win {"GRAPH 2D"};
    glutInit(&argc, argv);
    glutInitWindowSize(winw, winh);
    glutInitWindowPosition(120, 86);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow(win);
    glutDisplayFunc(Draw);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
    init();
    glutMainLoop();
    
    return 0;
}

