#include <GL/glut.h>
#include <functional>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <stdio.h>
#include <stdlib.h> //for malloc/free
#include <string>
#include <thread>
#include <vector>

int WIDTH;
int HEIGHT;
double G = 6.673 * pow(10, -11);
const int NUMDIMENSIONS = 2;
GLsizei winWidth = 1920, winHeight = 1080; // Initial display-window size.
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
      this->accel[i] = 0;
    }
  }
  Particle(double pos[], double vel[], double mass) {
    this->mass = mass;
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      this->pos[i] = pos[i];
      this->velocity[i] = vel[i];
      this->accel[i] = 0;
    }
  }
  void setxy(double x, double y) {
    this->pos[0] = x;
    this->pos[1] = y;
  }
  void addPull(Particle &other) {
    // add the acceleration due to gravity by other on this
    double dist = distance(this->pos, other.pos);
    double F = (this->mass) * (other.mass) * G / pow(dist, 2);
    double a = F / (this->mass);
    double theta = angle(this->pos, other.pos);
    this->accel[0] += a * cos(theta);
    this->accel[1] += a * sin(theta);
  }

  void resetPull() {
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      this->accel[i] = 0;
      // this->velocity[i] = 0;
    }
  }
  void advance(double dt) {
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      velocity[i] = velocity[i] + accel[i] * dt;
      pos[i] += velocity[i] * dt;
    }
    GLsizei limits[] = {winWidth, winHeight};
    // bounce off walls
    double decay = 0.5;
    double factor = pow(decay, 0.5);
    factor = 0.5;
    for (int s = 0; s < 2; s++) {
      if (pos[s] < 0) {
        // stay in bounds
        pos[s] = 0;
        // reduce kinetic energy
        velocity[s] *= -factor;
      }
      if (pos[s] > limits[s]) {
        pos[s] = limits[s];
        velocity[s] *= -factor;
      }
    }
  }
  std::string toString() {
    std::string out = "Particle({";
    std::string temp;
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      out += dtos(this->pos[i]);
      if (i < NUMDIMENSIONS - 1) {
        out += ", ";
      }
    }
    out += "}, {";
    for (int i = 0; i < NUMDIMENSIONS; i++) {
      out += dtos(this->velocity[i]);
      if (i < NUMDIMENSIONS - 1) {
        out += ", ";
      }
    }
    out += "}, " + dtos(this->mass) + ")";
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
      list[i].resetPull();
      for (int j = 0; j < list.size(); j++) {
        if (j != i) {
          list[i].addPull(list[j]);
        }
      }
    }
    for (int i = 0; i < list.size(); i++) {
      (list[i]).advance(dt);
    }
  }
};
struct RGB {
  double values[3];
  RGB() {}
  RGB(double rr, double gg, double bb) {

    values[0] = rr;
    values[1] = gg;
    values[2] = bb;
  }
};
double MASSMAX = pow(10, 14);
class Simulator2D {
public:
  ParticleField sim = ParticleField();
  std::vector<RGB> colors;
  Simulator2D(int N) {
    srand(time(NULL));

    ParticleField sim;
    double pos[] = {10, 10};
    double vel[] = {0, 0};
    Particle pa;
    for (int i = 0; i < N; i++) {
      // randomize initial conditions
      for (int j = 0; j < 2; j++) {
        double r = 1000;
        int g = 100;
        vel[j] = r * (((double)(rand() % g) - g / 2.0) * 2.0 / g);
      }
      pos[0] = (double)(rand() % winWidth);
      pos[1] = (double)(rand() % winHeight);
      double mass = (rand() % 100) * MASSMAX / 100.0;
      // construct particle with initial conditions
      pa = Particle(pos, vel, mass);
      (this->sim).add(pa);
      // associate a color
      RGB c = RGB();
      for (int ci = 0; ci < 3; ci++) {
        c.values[ci] = (rand() % 16) / 16.0;
      }
      colors.push_back(c);
    }
  }
  void advance(double dt) { this->sim.simulate(dt); }
  void render() {
    // OpenGL display stuff goes here
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < this->sim.list.size(); i++) {
      Particle p = ((this->sim.list[i]));
      // std::cout << "draw " << i << " " << p.toString() << "\n";
      int x, y;
      x = (int)p.pos[0];
      y = (int)p.pos[1];
      RGB c = this->colors[i];
      glColor3f(c.values[0], c.values[1], c.values[2]);
      int radius = (int)(20.0 * p.mass / MASSMAX);
      glPointSize(radius);
      glBegin(GL_POINTS);
      glVertex2f(x - radius/2, y - radius/2);
      glEnd();
    }
  };
};

Simulator2D SIMULATION = Simulator2D(20);
// ==================== OpenGL stuff ====================
void init(void) {
  glClearColor(0.2, 0.2, 0.2, 1.0); // Set display-window color to blue.

  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}

void displayFcn(void) {
  float timeStep = 1/10.0;
  int g = 100;
  for (int i = 0; i < g; i++) {

    SIMULATION.advance(timeStep / g);
  }
  SIMULATION.render();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
  glFlush();
  glutPostRedisplay();
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
