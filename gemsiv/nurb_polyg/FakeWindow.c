/* This is a fake version, used to test on other machines. */

extern void MakeWindow(void);

void MakeWindow()
{}

extern void MoveTo( short, short );
extern void LineTo( short, short );



void LineTo( short x, short y )
{
	printf("L %3d, %3d\n", (int) x, (int) y );
}
void MoveTo( short x, short y )
{
	printf("M %3d, %3d\n", (int) x, (int) y );
}
