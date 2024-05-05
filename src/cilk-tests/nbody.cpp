#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <cilk/cilk.h>

// Structure to represent a 2D vector
struct Vector2D {
    double x, y;

    Vector2D(double _x, double _y) : x(_x), y(_y) {}
};

// Structure to represent a particle
struct Particle {
    Vector2D position;
    Vector2D velocity;
    double mass;

    Particle(double x, double y, double vx, double vy, double m) 
        : position(x, y), velocity(vx, vy), mass(m) {}
};

// Function to calculate the Euclidean distance between two points
double distance(const Vector2D& p1, const Vector2D& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

// Function to calculate the force between two particles
Vector2D calculateForce(const Particle& p1, const Particle& p2) {
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
void updateVelocity(Particle& particle, const std::vector<Particle>& particles) {
    Vector2D force(0.0, 0.0);
    for (const auto& other : particles) {
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
void updatePosition(Particle& particle) {
    double dt = 0.1; // time step
    particle.position.x += particle.velocity.x * dt;
    particle.position.y += particle.velocity.y * dt;
}

// Function to simulate the N-body problem
void simulateNBody(std::vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        cilk_spawn updateVelocity(particles[i], particles);
    }
    cilk_sync;

    for (size_t i = 0; i < particles.size(); ++i) {
        cilk_spawn updatePosition(particles[i]);
    }
    cilk_sync;
}

double getRandomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

int main() {
    // Create some particles
    int n = 100000;

    std::vector<Particle> particles;

    // Generate random input for the simulation
    for (int i = 0; i < n; ++i) {
        double x = getRandomDouble(-100.0, 100.0);
        double y = getRandomDouble(-100.0, 100.0);
        double vx = getRandomDouble(-10.0, 10.0);
        double vy = getRandomDouble(-10.0, 10.0);
        double m = getRandomDouble(1, 1000.0);
        particles.push_back(Particle(x, y, vx, vy, m));
    }


    // Simulate the N-body problem
    auto start = std::chrono::steady_clock::now();
    simulateNBody(particles);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Nbody's time: " << duration.count() << " ms" << std::endl;

    return 0;
}
