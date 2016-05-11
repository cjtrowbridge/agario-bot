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
    int ncells;         // Number of cells
};

// The actions for the bot to take
struct action {
    double dx;      // delta x
    double dy;      // delta y
    int fire;       // 0 or 1
    int split;      // 0 or 1
};


// Function prototypes
#ifdef __cplusplus
extern "C" {
#endif

struct action playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass);

#ifdef __cplusplus
}
#endif