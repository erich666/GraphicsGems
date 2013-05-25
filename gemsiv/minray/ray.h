/* ray.h for test1, first test scene */
#define DEPTH 3		/* max ray tree depth */
#define SIZE 32		/* resolution of picture in x and y */
#define AOV 25		/* total angle of view in degrees */
#define NSPHERE 5	/* number of spheres */

AMBIENT = {.02, .02, .02};	/* ambient light color */

/* sphere: x y z  r g b  rad  kd ks kt kl  ir */
SPHERE = {
     0., 6., .5,    1., 1., 1.,   .9,   .05, .2, .85, 0.,  1.7,
    -1., 8., -.5,   1., .5, .2,   1.,   .7, .3, 0., .05,   1.2,
     1., 8., -.5,   .1, .8, .8,   1.,   .3, .7, 0., 0.,    1.2,
     3., -6., 15.,  1., .8, 1.,   7.,   0., 0., 0., .6,    1.5,
    -3., -3., 12.,  .8, 1., 1.,   5.,   0., 0., 0., .5,    1.5,
};
