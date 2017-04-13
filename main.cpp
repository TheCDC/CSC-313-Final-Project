// note you need a bitmap file named data/EarthTexture.bmp to run this
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> //for malloc/free
#include <string>
#include <vector>
double G = 6.673 * pow(10, -11);
const int NUMDIMENSIONS = 2;
GLsizei winWidth = 400, winHeight = 300; // Initial display-window size.

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

class Particle {
  // a class to represent a point mass that experiences gravity wrt other point
  // masses
public:
  double pos[NUMDIMENSIONS];
  double velocity[NUMDIMENSIONS];
  double accel[NUMDIMENSIONS];

  double mass;

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
    }
  }
  void simulate(double dt) {
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      velocity[i] += accel[i] * dt;
      this->pos[i] += this->velocity[i] * dt;
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
public:
  std::vector<Particle *> list;

  ParticleField() {}

  void add(Particle *p) { list.push_back(p); }

  void simulate(double dt) {
    for (int i = 0; i < list.size(); i++) {
      (*list[i]).resetPull();
      for (int j = 0; i < list.size(); i++) {
        if (j != i) {
          (*list[i]).addPull((*list[j]));
        }
      }
      list[i]->simulate(dt);
    }
  }
};
void init(void) {
  glClearColor(0.0, 0.0, 1.0, 1.0); // Set display-window color to blue.

  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void displayFcn(void) { glClear(GL_COLOR_BUFFER_BIT); }

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
  double pos1[2] = {0, 0};
  double pos2[2] = {10, 10};

  Particle p1 = Particle(pos1, pos1, 100000000000);
  Particle p2 = Particle(pos2, pos1, 100000000);

  std::cout << "p1=" << p1.toString() << "\n";
  std::cout << "p2=" << p2.toString() << "\n";
  std::cout << "dist=" << distance(p1.pos, p2.pos) << "\n";

  std::cout << "---------- Testing acceleration ----------\n";
  for (int i = 0; i < 10; i++) {
    p1.addPull(p2);
    p2.addPull(p1);

    p1.simulate(0.1);
    p2.simulate(0.1);
    std::cout << "p1=" << p1.toString() << "\n";
    std::cout << "p2=" << p2.toString() << "\n";
    std::cout << "dist=" << distance(p1.pos, p2.pos) << "\n";
  }
  std::cout << "---------- Test ParticleField ----------\n";
  ParticleField pf = ParticleField();
  // p1.setxy(0, 0);
  // p2.setxy(10, 10);
  Particle *pp = &p1;
  pf.add(pp);
  pp = &p2;
  pf.add(pp);
  std::cout << "readout\n";
  for (int j = 0; j < pf.list.size(); j++) {
    std::cout << "p" << j << "=" << pf.list[j]->toString() << "\n";
  }
  for (int i = 0; i < 10; i++) {
    pf.simulate(0.1);
    for (int j = 0; j < pf.list.size(); j++) {
      std::cout << "p" << j << "=" << pf.list[j]->toString() << "\n";
    }
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(winWidth, winHeight);
  glutCreateWindow("Chris Chen CSC313 Final Project");

  init();
  glutDisplayFunc(displayFcn);
  glutReshapeFunc(winReshapeFcn);
  glutMouseFunc(mouseHandler);

  glutMainLoop();
}
