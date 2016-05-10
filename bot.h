struct food {
	double x;
	double y;
	double mass;
};

struct cell {
	double x;
	double y;
	double mass;
	double radius;
};

struct player {
    double x;           // Center of mass
    double y;
    double totalMass;   // Sum of cell masses
    struct cell *cells; // Array of cells
};

// The target of a player move
struct target {
    double dx;      // delta x
    double dy;      // delta y
};


// Function prototypes
#ifdef __cplusplus
extern "C" {
#endif

struct target playerMove(struct player me, 
                         struct food *foods, int foodLen);

#ifdef __cplusplus
}
#endif