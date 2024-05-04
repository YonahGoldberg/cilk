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

void simulateNBody(std::vector<Particle>& particles);