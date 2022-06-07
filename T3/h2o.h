
// Los siguientes son tipos abstractos.
// Solo debe usar punteros a ellos, sin saber qué hay adentro.
typedef struct hydrogen Hydrogen;
typedef struct oxygen Oxygen;
typedef struct h2o H2O;

// Procedimiento dado en test-h2o.c:
// Invoquelo para formar la molecula de agua
H2O *makeH2O(Hydrogen *h1, Hydrogen *h2, Oxygen *o);

// Procedimientos que Ud. debe programar en h2o.c
void initH2O(void);
void endH2O(void);
H2O *combineOxy(Oxygen *o);
H2O *combineHydro(Hydrogen *h);
