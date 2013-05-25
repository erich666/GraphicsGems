#include <stream.h>
#include "Scene3D.h"

int main(int, char**)
{
char	script[1024],
	*image;
Scene3D scene;
vec2	res;

cout << "\nSimple Ray-Tracer v1.0\nby Jean-Francois Doue\n\nenter a script name:";
cin >> script;
istream s(script, io_readonly, a_useonly);
s >> scene;
cout << "\nResolution of the rendering (x y):";
cin >> res;
image = scene.rayTrace(res);

// put some code here to save the image in your preferred format

delete image;
return 1;
}