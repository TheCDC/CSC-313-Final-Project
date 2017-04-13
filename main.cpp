#include <GL/glut.h>
#include <functional>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> //for malloc/free
#include <string>
#include <vector>
int WIDTH;
int HEIGHT;
double G = 6.673 * pow(10, -11);
const int NUMDIMENSIONS = 2;
GLsizei winWidth = 400, winHeight = 300; // Initial display-window size.
// ==================== Helper functions ====================
std::string dtos(double v) {
  // return string representation of a double
  std::ostringstream strs;
  strs << v;
  return strs.str();
}

double distance(double a[], double b[]) {
  // angle between to 2tuples of coordinates
  double sqsum = 0;
  for (int i = 0; i < NUMDIMENSIONS; i++) {
    sqsum += pow(a[i] - b[i], 2);
  }
  return pow(sqsum, 0.5);
}

double angle(double a[], double b[]) {
  // angle between to 2tuples of coordinates
  return atan2(b[1] - a[1], b[0] - a[0]);
}
// ==================== Classes ====================
class Particle {
  // a class to represent a point mass that experiences gravity wrt other point
  // masses
public:
  double pos[NUMDIMENSIONS];
  double velocity[NUMDIMENSIONS];
  double accel[NUMDIMENSIONS];

  double mass;
  Particle() {
    this->mass = 0;
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      this->pos[i] = 0;
      this->velocity[i] = 0;
    }
  }
  Particle(double pos[], double vel[], double mass) {
    this->mass = mass;
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      this->pos[i] = pos[i];
      this->velocity[i] = vel[i];
    }
  }
  void setxy(double x, double y) {
    this->pos[0] = x;
    this->pos[1] = y;
  }
  void addPull(Particle &other) {
    // add the acceleration due to gravity by other on this
    double dist = distance(this->pos, other.pos);
    double F = (this->mass) * (other.mass) * G / dist;
    double a = F / (this->mass);
    double theta = angle(this->pos, other.pos);
    this->accel[0] = a * cos(theta);
    this->accel[1] = a * sin(theta);
  }

  void resetPull() {
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      this->accel[i] = 0;
      this->velocity[i] = 0;
    }
  }
  void advance(double dt) {
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      velocity[i] = velocity[i] + accel[i] * dt;
      pos[i] += velocity[i] * dt;
    }
  }
  std::string toString() {
    std::string out = "Particle({";
    std::string temp;
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      out += dtos(this->pos[i]);
      if (i < NUMDIMENSIONS - 1) {
        out += ",";
      }
    }
    out += "}, {";
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      out += dtos(this->velocity[i]);
      if (i < NUMDIMENSIONS - 1) {
        out += ",";
      }
    }
    out += "}," + dtos(this->mass) + ")";
    return out;
  }
};

class ParticleField {
  // A class to represent a field of interacting particles
public:
  std::vector<Particle> list;

  ParticleField() {}

  void add(Particle p) { list.push_back(p); }

  void simulate(double dt) {
    for (int i = 0; i < list.size(); i++) {
      (list[i]).resetPull();
      for (int j = 0; j < list.size(); j++) {
        if (j != i) {
          (list[i]).addPull((list[j]));
        }
      }
      (list[i]).advance(dt);
    }
  }
};

class Simulator2D {
public:
  ParticleField sim = ParticleField();
  Simulator2D(int N) {

    ParticleField sim;
    double pos[] = {10, 10};
    double vel[] = {0, 0};
    Particle pa;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < 2; j++) {
        vel[j] = ((double)(rand() % 10) - 5.0 / 5.0);
        pos[j] = (double)(rand() % 200);
      }

      pa = Particle(pos, vel, 10000000000);
      (this->sim).add(pa);
    }
  }
  void advance(double dt) { this->sim.simulate(dt); }
  void render() {
    // OpenGL display stuff goes here
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < this->sim.list.size(); i++) {
      Particle p = ((this->sim.list[i]));
      std::cout << "draw " << i << " " << p.toString() << "\n";
      int x, y;
      x = (int)p.pos[0];
      y = (int)p.pos[1];
      glColor3f(1, 1, 1);
      glPointSize(1);
      glBegin(GL_POINTS);
      glVertex2f(x, y);
      glEnd();
    }
    glFlush();
    glutPostRedisplay();
  };
};

Simulator2D SIMULATION = Simulator2D(10);
// ==================== OpenGL stuff ====================
void init(void) {
  glClearColor(0.0, 0.0, 1.0, 1.0); // Set display-window color to blue.

  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void displayFcn(void) {
  SIMULATION.advance(0.5);
  SIMULATION.render();
}

void winReshapeFcn(GLint newWidth, GLint newHeight) {
  /*  Reset viewport and projection parameters  */
  glViewport(0, 0, newWidth, newHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, GLdouble(newWidth), 0.0, GLdouble(newHeight));

  /*  Reset display-window size parameters.  */
  winWidth = newWidth;
  winHeight = newHeight;
}

void mouseHandler(GLint button, GLint action, GLint xMouse, GLint yMouse) {}

int main(int argc, char **argv) {
  std::cout << "---------- Tests ----------\n";
  double pos1[2] = {0, 0};
  double pos2[2] = {10, 10};

  Particle p1 = Particle(pos1, pos1, 1000000000);
  Particle p2 = Particle(pos2, pos1, 1000000);

  std::cout << "p1=" << p1.toString() << "\n";
  std::cout << "p2=" << p2.toString() << "\n";
  std::cout << "dist=" << distance(p1.pos, p2.pos) << "\n";

  std::cout << "---------- Testing acceleration ----------\n";
  for (int i = 0; i < 10; i++) {
    p1.addPull(p2);
    p2.addPull(p1);

    p1.advance(0.1);
    p2.advance(0.1);
    std::cout << "p1=" << p1.toString() << "\n";
    std::cout << "p2=" << p2.toString() << "\n";
    std::cout << "dist=" << distance(p1.pos, p2.pos) << "\n";
  }
  std::cout << "---------- Test ParticleField ----------\n";
  ParticleField pf = ParticleField();
  p1 = Particle(pos1, pos1, 10000000000);
  p2 = Particle(pos2, pos1, 10000000);
  pf.add(p1);
  pf.add(p2);
  for (int j = 0; j < pf.list.size(); j++) {
    std::cout << "p" << j << "=" << (pf.list[j]).toString() << "\n";
  }
  for (int i = 0; i < 10; i++) {
    pf.simulate(0.1);
    for (int j = 0; j < pf.list.size(); j++) {
      std::cout << "p" << j << "=" << pf.list[j].toString() << "\n";
    }
  }
  std::cout << "---------- Test Simulator2D ----------\n";

  Simulator2D testsim = Simulator2D(10);
  for (int i = 0; i < testsim.sim.list.size(); i++) {
    std::cout << "contains: " << (testsim.sim.list[i]).toString() << "\n";
  }
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(winWidth, winHeight);
  glutCreateWindow("Chris Chen CSC313 Final Project");
  std::cout << "---------- Begin ----------\n";
  init();
  glutDisplayFunc(displayFcn);
  glutReshapeFunc(winReshapeFcn);
  glutMouseFunc(mouseHandler);

  glutMainLoop();
}
