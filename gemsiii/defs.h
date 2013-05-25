  /* Dummy include file of definitions to make rolling-ball compile. */

#define Button1 1
#define ERASE   0
#define DRAW    3

typedef struct {
  char *name;       
  double frame[4][4];} Polyhedron;

typedef struct {
  char *name;
  int id;} Display;
