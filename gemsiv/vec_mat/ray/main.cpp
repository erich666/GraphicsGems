#include <fstream>
#include "Scene3D.h"

int main(int, char**)
{
char	script[1024],
	*image;
Scene3D scene;
vec2	res;

std::cout << "\nSimple Ray-Tracer v1.0\nby Jean-Francois Doue\n\nenter a script name:";
std::cin >> script;
std::ifstream s(script, std::ios::in);
s >> scene;
std::cout << "\nResolution of the rendering (x y):";
std::cin >> res;
image = scene.rayTrace(res);

// put some code here to save the image in your preferred format

delete image;
return 1;
}