#include "../scheduler_instance.hpp"
#include "nbody.hpp"
#include <cmath>
#include <iostream>
#include <vector>

// Structure to represent a 2D vector
// struct Vector2D {
//     double x, y;

//     Vector2D(double _x, double _y) : x(_x), y(_y) {}
// };

// // Structure to represent a particle
// struct Particle {
//     Vector2D position;
//     Vector2D velocity;
//     double mass;

//     Particle(double x, double y, double vx, double vy, double m)
//         : position(x, y), velocity(vx, vy), mass(m) {}
// };

// Function to calculate the Euclidean distance between two points
double distance(const Vector2D &p1, const Vector2D &p2) {
  double dx = p1.x - p2.x;
  double dy = p1.y - p2.y;
  return sqrt(dx * dx + dy * dy);
}

// Function to calculate the force between two particles
Vector2D calculateForce(const Particle &p1, const Particle &p2) {
  double G = 6.674 * pow(10, -11); // gravitational constant
  double dist = distance(p1.position, p2.position);
  double force = G * p1.mass * p2.mass / (dist * dist);

  double dx = p2.position.x - p1.position.x;
  double dy = p2.position.y - p1.position.y;

  double fx = force * dx / dist;
  double fy = force * dy / dist;

  return Vector2D(fx, fy);
}

// Function to update the velocity of a particle based on the force acting on it
void updateVelocity(Particle &particle,
                    const std::vector<Particle> &particles) {
  Vector2D force(0.0, 0.0);
  for (const auto &other : particles) {
    if (&particle != &other) {
      Vector2D f = calculateForce(particle, other);
      force.x += f.x;
      force.y += f.y;
    }
  }

  double dt = 0.1; // time step
  double ax = force.x / particle.mass;
  double ay = force.y / particle.mass;

  particle.velocity.x += ax * dt;
  particle.velocity.y += ay * dt;
}

// Function to update the position of a particle based on its velocity
void updatePosition(Particle &particle) {
  double dt = 0.1; // time step
  particle.position.x += particle.velocity.x * dt;
  particle.position.y += particle.velocity.y * dt;
}

// Function to simulate the N-body problem
void simulateNBody(std::vector<Particle> &particles) {
  std::vector<std::future<void>> futures1;
  for (size_t i = 0; i < particles.size(); ++i) {
    auto fut = scheduler->spawn([i, &particles]() {
      updateVelocity(particles[i], particles);
      return 0;
    });
    futures1.push_back(std::move(fut));
  }
  for (auto &fut : futures1) {
    scheduler->sync(std::move(fut));
  }

  std::vector<std::future<void>> futures2;
  for (size_t i = 0; i < particles.size(); ++i) {
    auto fut = scheduler->spawn([i, &particles]() {
      updatePosition(particles[i]);
      return 0;
    });
    futures2.push_back(std::move(fut));
  }
  for (auto &fut : futures2) {
    scheduler->sync(std::move(fut));
  }
}

int main() {
  // Create some particles
  std::vector<Particle> particles;
  particles.push_back(
      Particle(0.0, 0.0, 0.0, 0.0, 1000.0)); // Massive particle at origin
  particles.push_back(
      Particle(1.0, 0.0, 0.0, 1.0, 1.0)); // Small particle with velocity

  // Simulate the N-body problem
  simulateNBody(particles);

  // Print the final positions of the particles
  for (const auto &particle : particles) {
    std::cout << "Particle position: (" << particle.position.x << ", "
              << particle.position.y << ")\n";
  }

  return 0;
}
