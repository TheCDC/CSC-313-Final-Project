# Description
This is my final project for CBU CSC313 Graphics and Visual Computing. It is a 2D n-body physics simulation.
# Screenshots
![Screenshot_2017-04-18_20-57-32.png](shots/Screenshot_2017-04-18_20-57-32.png)
![Screenshot_2017-04-18_20-57-50.png](shots/Screenshot_2017-04-18_20-57-50.png)
![Screenshot_2017-04-18_20-57-50.png](shots/Screenshot_2017-04-18_20-57-50.png)

# Class Breakdown
I defined three classes in this project.
- Particle
- ParticleField
- Simulator2D

## Particle 
Knows the physics, `F=ma=G*m_1*m_2/r^2`.
Instantiated with:
- Initial velocity
- Initial position
- Mass
Important methods
- void addPull(Particle &other) F=G*m_1*m_2/r^2
- void resetPull()
- void advance(double dt)

## ParticleField
Important methods
- void add(Particle p)
- void simulate(double dt)

## Simulator2D
Initialize with desired number of Particles.
Methods:
- void advance(double dt)
- void render()

# Closing notes
Edited using Visual Studio Code.
