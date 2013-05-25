%{

#include <iostream.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"

//      DECLARATIONS

texture *deftexture;
declarations<object> odecls;
declarations<texture> tdecls;
declarations<pigment> pdecls;
declarations<normal> ndecls;
declarations<finish> fdecls;
declarations<color> cdecls;
declarations<colormap> cmdecls;
declarations<vector> vdecls;
declarations<camera> camdecls;

int max_intersections=64;
double version=2.2;
double clock=0.;
intensity background=intensity(0.,0.,0.);

%}

%start  items

%union {
        int ival;
        double dval;
        char* sval;
        vector* vval;
        object* oval;
        color* cval;
        list<object*>* olval;
        texture* tval;
        pigment* pval;
        normal* nval;
        finish* fval;
        camera* camval;
        colormap* cmval;
        lightsource *lval;
}

/*
*       KEYWORD TOKENS
*/
%token  <ival> ADAPTIVE
%token  <ival> AGATE
%token  <ival> AGATE_TURB
%token  <ival> ALL
%token  <ival> ALPHA
%token  <ival> AMBIENT
%token  <ival> AREA_LIGHT
%token  <ival> BACKGROUND
%token  <ival> BICUBIC_PATCH
%token  <ival> BLOB
%token  <ival> BLUE
%token  <ival> BOUNDED_BY
%token  <ival> BOX
%token  <ival> BOZO
%token  <ival> BRICK
%token  <ival> BRILLIANCE
%token  <ival> BUMPS
%token  <ival> BUMPY1
%token  <ival> BUMPY2
%token  <ival> BUMPY3
%token  <ival> BUMP_MAP
%token  <ival> BUMP_SIZE
%token  <ival> CAMERA_ID
%token  <ival> CAMERA
%token  <ival> CHECKER
%token  <ival> CLIPPED_BY
%token  <ival> CLOCK
%token  <ival> COLOR_ID
%token  <ival> COLOR_MAP_ID
%token  <ival> COLOR_MAP
%token  <ival> COLOR
%token  <ival> COMPONENT
%token  <ival> COMPOSITE
%token  <ival> CONE
%token  <ival> CRAND
%token  <ival> CUBIC
%token  <ival> CYLINDER
%token  <ival> DECLARE
%token  <ival> DEFAULT
%token  <ival> DENTS
%token  <ival> DIFFERENCE
%token  <ival> DIFFUSE
%token  <ival> DIRECTION
%token  <ival> DISC
%token  <ival> DISTANCE
%token  <ival> DUMP
%token  <ival> END_OF_FILE
%token  <ival> FALLOFF
%token  <ival> FILTER
%token  <ival> FINISH_ID
%token  <ival> FINISH
%token  <ival> FLATNESS
%token  <ival> FLOAT_ID
%token  <ival> FLOAT
%token  <ival> FOG
%token  <ival> FREQUENCY
%token  <ival> GIF
%token  <ival> GRADIENT
%token  <ival> GRANITE
%token  <ival> GREEN
%token  <ival> HEIGHT_FIELD
%token  <ival> HEXAGON
%token  <ival> IFF
%token  <ival> IMAGE_MAP
%token  <ival> INCLUDE
%token  <ival> INTERPOLATE
%token  <ival> INTERSECTION
%token  <ival> INVERSE
%token  <ival> IOR
%token  <ival> JITTER
%token  <ival> LAMBDA
%token  <ival> LEOPARD
%token  <ival> LIGHT_SOURCE
%token  <ival> LOCATION
%token  <ival> LOOKS_LIKE
%token  <ival> LOOK_AT
%token  <ival> MANDEL
%token  <ival> MAP_TYPE
%token  <ival> MARBLE
%token  <ival> MATERIAL_MAP
%token  <ival> MAX_INTERSECTIONS
%token  <ival> MAX_TRACE_LEVEL
%token  <ival> MERGE
%token  <ival> METALLIC
%token  <ival> MORTAR
%token  <ival> NO_SHADOW
%token  <ival> OBJECT_ID
%token  <ival> OBJECT
%token  <ival> OCTAVES
%token  <ival> OMEGA
%token  <ival> ONCE
%token  <ival> ONION
%token  <ival> OPEN
%token  <ival> PAINTED1
%token  <ival> PAINTED2
%token  <ival> PAINTED3
%token  <ival> PHASE
%token  <ival> PHONG_SIZE
%token  <ival> PHONG
%token  <ival> PIGMENT_ID
%token  <ival> PIGMENT
%token  <ival> PLANE
%token  <ival> POINT_AT
%token  <ival> POLY
%token  <ival> POT
%token  <ival> QUADRIC
%token  <ival> QUARTIC
%token  <ival> QUICK_COLOR
%token  <ival> RADIAL
%token  <ival> RADIUS
%token  <ival> RAW
%token  <ival> RED
%token  <ival> REFLECTION
%token  <ival> REFRACTION
%token  <ival> RGBF
%token  <ival> RGB
%token  <ival> RIGHT
%token  <ival> RIPPLES
%token  <ival> ROTATE
%token  <ival> ROUGHNESS
%token  <ival> SCALE
%token  <ival> SKY
%token  <ival> SMOOTH
%token  <ival> SMOOTH_TRIANGLE
%token  <ival> SPECULAR
%token  <ival> SPHERE
%token  <ival> SPOTLIGHT
%token  <ival> SPOTTED
%token  <ival> STRING
%token  <ival> STURM
%token  <ival> TEXTURE_ID
%token  <ival> TEXTURE
%token  <ival> TGA
%token  <ival> THRESHOLD
%token  <ival> TIGHTNESS
%token  <ival> TILE2
%token  <ival> TILES
%token  <ival> TNORMAL_ID
%token  <ival> NORMAL
%token  <ival> TORUS
%token  <ival> TRACK
%token  <ival> TRANSFORM_ID
%token  <ival> TRANSFORM
%token  <ival> TRANSLATE
%token  <ival> TRIANGLE
%token  <ival> TURBULENCE
%token  <ival> TYPE
%token  <ival> UNION
%token  <ival> UP
%token  <ival> USE_COLOR
%token  <ival> USE_INDEX
%token  <ival> U_STEPS
%token  <ival> VECTOR_ID
%token  <ival> VERSION
%token  <ival> V_STEPS
%token  <ival> WATER_LEVEL
%token  <ival> WAVES
%token  <ival> WOOD
%token  <ival> WRINKLES
%token  <ival> X
%token  <ival> Y
%token  <ival> Z
/*
*       OTHER TOKENS
*/
%token  <dval> LITERAL
%token  <sval> IDENTIFIER
/*
*       NONTERMINALS
*/
%type   <vval> expr
%type   <oval> object objopts
%type   <olval> objects moreobjects
%type   <cval> color colorvalues
%type   <tval> texture textureitems
%type   <pval> pigment pigmentitems pigmentpatt
%type   <nval> normal normalitems normalpatt
%type   <fval> finish finishitems
%type   <camval> camera cameraitems
%type   <cmval> colormap colormapitems
%type   <sval> identifier
%type   <lval> lightsource lightsrcitems lightsrctype spotlight
%type   <vval> tightness


%left   '+' '-'
%left   '*' '/'
%left   UMINUS

%%
/*
*       SCENE FILE ITEMS AT TOP LEVEL
*/
items   :       /* empty */
                        {deftexture=new texture;}
        |       items item
        ;

item    :       camera
                        {actcamera=*$1; delete $1;}
        |       object
                        {{if(verbose) cout<<*$1;} objects+=$1;}
        |       lightsource
                        {lightsources+=$1;}
        |       '#' DECLARE identifier '=' object
                        {odecls+=new declaration<object>($3,$5);}
        |       '#' DECLARE identifier '=' texture
                        {tdecls+=new declaration<texture>($3,$5);}
        |       '#' DECLARE identifier '=' pigment
                        {pdecls+=new declaration<pigment>($3,$5);}
        |       '#' DECLARE identifier '=' normal
                        {ndecls+=new declaration<normal>($3,$5);}
        |       '#' DECLARE identifier '=' finish
                        {fdecls+=new declaration<finish>($3,$5);}
        |       '#' DECLARE identifier '=' color
                        {cdecls+=new declaration<color>($3,$5);}
        |       '#' DECLARE identifier '=' colormap
                        {cmdecls+=new declaration<colormap>($3,$5);}
        |       '#' DECLARE identifier '=' expr
                        {vdecls+=new declaration<vector>($3,$5);}
        |       '#' DECLARE identifier '=' camera
                        {camdecls+=new declaration<camera>($3,$5);}
        |       '#' MAX_TRACE_LEVEL expr
                        {lmax=(int)((double)*$3); delete $3;}
        |       '#' MAX_INTERSECTIONS expr
                        {max_intersections=(int)((double)*$3); delete $3;}
        |       '#' VERSION expr
                        {version=(double)*$3; delete $3;}
        |       '#' DEFAULT '{' texture '}'
                        {delete deftexture; deftexture=$4;}
        |       '#' DEFAULT '{' pigment '}'
                        {deftexture->setp(*$4); delete $4;}
        |       '#' DEFAULT '{' normal '}'
                        {deftexture->setn(*$4); delete $4;}
        |       '#' DEFAULT '{' finish '}'
                        {deftexture->setf(*$4); delete $4;}
        |       BACKGROUND '{' color '}'
                        {background=(intensity)(*$3); delete $3;}
        ;

identifier      :       IDENTIFIER
                                {$$=new char[strlen($1)+1]; strcpy($$,$1);}
                ;

/*
*       FLOAT AND VECTOR EXPRESSIONS UNIFIED IN ORDER TO GET LR(1) GRAMMAR
*/
expr    :       LITERAL
                        {$$=new vector($1,$1,$1);}
        |       identifier
                        {$$=new vector; *$$=*vdecls[$1]; delete $1;}
        |       X
                        {$$=new vector(1.,0.,0.);}
        |       Y
                        {$$=new vector(0.,1.,0.);}
        |       Z
                        {$$=new vector(0.,0.,1.);}
        |       '<' expr ',' expr ',' expr '>'
                        {$$=new vector((double)*$2,(double)*$4,(double)*$6);
                         delete $2; delete $4; delete $6;}
        |       expr '+' expr
                        {$$=new vector; *$$=*$1+*$3; delete $1; delete $3;}
        |       expr '-' expr
                        {$$=new vector; *$$=*$1-*$3; delete $1; delete $3;}
        |       expr '*' expr
                        {$$=new vector; *$$=*$1*(*$3); delete $1; delete $3;}
        |       expr '/' expr
                        {$$=new vector; *$$=*$1/(*$3); delete $1; delete $3;}
        |       '-' expr %prec UMINUS
                        {$$=new vector; *$$=-*$2; delete $2;}
        |       '(' expr ')'
                        {$$=$2;}
        ;
/*
*               CAMERA DEFINITION
*/
camera          :       CAMERA '{' cameraitems '}'
                                {$$=$3;}
                ;

cameraitems     :       /* empty */
                                {$$=new camera;}
                |       cameraitems identifier
                                {$$=$1; *$$=actcamera /* *camdecls[$2] */ ; delete $2;}
                |       cameraitems LOCATION expr
                                {$$=$1; $$->setl(*$3); delete $3;}
                |       cameraitems SKY expr
                                {$$=$1; $$->sets(*$3); delete $3;}
                |       cameraitems DIRECTION expr
                                {$$=$1; $$->setd(*$3); delete $3;}
                |       cameraitems UP expr
                                {$$=$1; $$->setu(*$3); delete $3;}
                |       cameraitems RIGHT expr
                                {$$=$1; $$->setr(*$3); delete $3;}
                |       cameraitems LOOK_AT expr
                                {$$=$1; $$->seta(*$3); delete $3;}
                |       cameraitems TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       cameraitems ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       cameraitems SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;
/*
*               LIGHTSOURCE
*/
lightsource     :       LIGHT_SOURCE '{' expr color lightsrcitems '}'
                                {$$=$5; $$->setp(*$3); $$->setc(*$4);
                                 delete $3; delete $4;}
                ;

lightsrcitems   :       /* empty */
                                {$$=new lightsource;}
                |       lightsrcitems lightsrctype
                                {$$=$2; *$$=*$1; delete $1;}
                |       lightsrcitems TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       lightsrcitems ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       lightsrcitems SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;

lightsrctype    :       spotlight
                                {$$=$1;}
                ;

spotlight       :       SPOTLIGHT
                        POINT_AT expr RADIUS expr FALLOFF expr tightness
                                {$$=new spotlight(*$3,(double)*$5,(double)*$7,
                                                  (double)*$8);
                                 delete $3; delete $5; delete $7; delete $8;}
                ;

tightness       :       /* empty */
                                {$$=new vector(10.,10.,10.);}
                |       TIGHTNESS expr
                                {$$=$2;}
                ;
/*
*               OBJECT GEOMETRY
*/
object          :       OBJECT '{' identifier objopts '}'
                                {$$=odecls[$3]->copy(); *$$=*$4;
                                 delete $3; delete $4;}
                |       SPHERE '{' expr ',' expr objopts '}'
                                {$$=new sphere(*$3,(double)*$5); *$$=*$6;
                                 delete $3; delete $5; delete $6;}
                |       UNION '{' objects objopts '}'
                                {$$=new csguni($3); *$$=*$4; delete $4;}
                |       MERGE '{' objects objopts '}'
                                {$$=new csgmer($3); *$$=*$4; delete $4;}
                |       INTERSECTION '{' objects objopts '}'
                                {$$=new csgint($3); *$$=*$4; delete $4;}
                |       DIFFERENCE '{' objects objopts '}'
                                {$$=new csgdif($3); *$$=*$4; delete $4;}
                ;

objects         :       object object moreobjects
                                {$$=$3; *$$+=$2; *$$+=$1;}
                ;
                
moreobjects     :       /* empty */
                                {$$=new list<object*>;}
                |       moreobjects object
                                {$$=$1; *$$+=$2;}
                ;
/*
*               OBJECT OPTIONS
*/
objopts         :       /* empty */
                                {$$=new object; $$->sett(*deftexture);}
                |       objopts texture
                                {$$=$1; $$->sett(*$2); delete $2;}
                |       objopts pigment
                                {$$=$1; $$->setp(*$2); delete $2;}
                |       objopts normal
                                {$$=$1; $$->setn(*$2); delete $2;}
                |       objopts finish
                                {$$=$1; $$->setf(*$2); delete $2;}
                |       objopts TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       objopts ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       objopts SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;
/*
*               TEXTURES
*/
texture         :       TEXTURE '{' textureitems '}'
                                {$$=$3;}
                |       texture TEXTURE '{' textureitems '}'
                                {$$=$1; *$$+=$4;}
                ;

textureitems    :       /* empty */
                                {$$=new texture; *$$=*deftexture;}
                |       textureitems identifier
                                {delete $1; $$=tdecls[$2]->copy(); delete $2;}
                |       textureitems PIGMENT '{' pigmentitems '}'
                                {$$=$1; $$->setp(*$4); delete $4;}
                |       textureitems NORMAL '{' normalitems '}'
                                {$$=$1; $$->setn(*$4); delete $4;}
                |       textureitems FINISH '{' finishitems '}'
                                {$$=$1; $$->setf(*$4); delete $4;}
                |       textureitems TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       textureitems ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       textureitems SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;
/*
*               PIGMENT
*/
pigment         :       PIGMENT '{' pigmentitems '}'
                                {$$=$3;}
                ;

pigmentitems    :       /* empty */
                                {$$=deftexture->copyp();}
                |       pigmentitems identifier
                                {delete $1; $$=pdecls[$2]->copy(); delete $2;}
                |       pigmentitems pigmentpatt
                                {$$=$2; *$$|=*$1; delete $1;}
                |       pigmentitems TURBULENCE expr
                                {$$=$1; $$->sett(*$3); delete $3;}
                |       pigmentitems OCTAVES expr
                                {$$=$1;$$->seto((int)((double)*$3));delete $3;}
                |       pigmentitems OMEGA expr
                                {$$=$1; $$->setm((double)*$3); delete $3;}
                |       pigmentitems LAMBDA expr
                                {$$=$1; $$->setl((double)*$3); delete $3;}
                |       pigmentitems FREQUENCY expr
                                {$$=$1; $$->setf((double)*$3); delete $3;}
                |       pigmentitems PHASE expr
                                {$$=$1; $$->setp((double)*$3); delete $3;}
                |       pigmentitems TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       pigmentitems ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       pigmentitems SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;

pigmentpatt     :       color
                                {$$=new solid(*$1); delete $1;}
                |       CHECKER
                                {$$=new checker;}
                |       CHECKER color
                                {$$=new checker(*$2); delete $2;}
                |       CHECKER color color
                                {$$=new checker(*$2,*$3);delete $2;delete $3;}
                ;

color           :       COLOR identifier
                                {$$=new color; *$$=*cdecls[$2]; delete $2;}
                |       COLOR RGB '<' expr ',' expr ',' expr '>'
                                {$$=new color((double)*$4,(double)*$6,
                                              (double)*$8); 
                                 delete $4; delete $6; delete $8;}
                |       COLOR RGBF '<' expr ',' expr ',' expr ',' expr '>'
                                {$$=new color((double)*$4,(double)*$6,
                                              (double)*$8,(double)*$10); 
                                 delete $4; delete $6; delete $8; delete $10;}
                |       COLOR colorvalues
                                {$$=$2;}
                ;

colorvalues     :       RED expr
                                {$$=new color;$$->setr((double)*$2);delete $2;}
                |       GREEN expr
                                {$$=new color;$$->setg((double)*$2);delete $2;}
                |       BLUE expr
                                {$$=new color;$$->setb((double)*$2);delete $2;}
                |       FILTER expr
                                {$$=new color;$$->setf((double)*$2);delete $2;}
                |       colorvalues RED expr
                                {$$=$1; $$->setr((double)*$3); delete $3;}
                |       colorvalues GREEN expr
                                {$$=$1; $$->setg((double)*$3); delete $3;}
                |       colorvalues BLUE expr
                                {$$=$1; $$->setb((double)*$3); delete $3;}
                |       colorvalues FILTER expr
                                {$$=$1; $$->setf((double)*$3); delete $3;}
                ;

colormap        :       COLOR_MAP '{' identifier colormapitems '}'
                                {$$=cmdecls[$3]->copy(); delete $3; delete $4;}
                |       COLOR_MAP '{' colormapitems '}'
                                {$$=$3;}
                ;

colormapitems   :       /* empty */
                                {$$=new colormap;}
                ;
/*
*               NORMAL
*/
normal          :       NORMAL '{' normalitems '}'
                                {$$=$3;}
                ;

normalitems     :       /* empty */
                                {$$=deftexture->copyn();}
                |       normalitems identifier
                                {delete $1; $$=ndecls[$2]->copy(); delete $2;}
                |       normalitems normalpatt
                                {$$=$2; *$$|=*$1; delete $1;}
                |       normalitems TURBULENCE expr
                                {$$=$1; $$->sett(*$3); delete $3;}
                |       normalitems FREQUENCY expr
                                {$$=$1; $$->setf((double)*$3); delete $3;}
                |       normalitems PHASE expr
                                {$$=$1; $$->setp((double)*$3); delete $3;}
                |       normalitems TRANSLATE expr
                                {$$=$1; $$->traT(*$3); delete $3;}
                |       normalitems ROTATE expr
                                {$$=$1; $$->rotT(*$3); delete $3;}
                |       normalitems SCALE expr
                                {$$=$1; $$->scaT(*$3); delete $3;}
                ;

normalpatt      :       BUMPS expr
                                {$$=new bumps((double)*$2); delete $2;}
                |       DENTS expr
                                {$$=new dents((double)*$2); delete $2;}
                ;
/*
*               FINISH
*/
finish          :       FINISH '{' finishitems '}'
                                {$$=$3;}
                ;

finishitems     :       /* empty */
                                {$$=deftexture->copyf();}
                |       finishitems identifier
                                {delete $1; $$=fdecls[$2]->copy(); delete $2;}
                |       finishitems DIFFUSE expr
                                {$$=$1; $$->setkd((double)*$3); delete $3;}
                |       finishitems BRILLIANCE expr
                                {$$=$1; $$->setkb((double)*$3); delete $3;}
                |       finishitems CRAND expr
                                {$$=$1; $$->setkc((double)*$3); delete $3;}
                |       finishitems AMBIENT expr
                                {$$=$1; $$->setka((double)*$3); delete $3;}
                |       finishitems REFLECTION expr
                                {$$=$1; $$->setkr((double)*$3); delete $3;}
                |       finishitems PHONG expr
                                {$$=$1; $$->setks((double)*$3); delete $3;}
                |       finishitems PHONG_SIZE expr
                                {$$=$1; $$->setkp((double)*$3); delete $3;}
                |       finishitems SPECULAR expr
                                {$$=$1; $$->setkh((double)*$3); delete $3;}
                |       finishitems ROUGHNESS expr
                                {$$=$1; $$->setkg((double)*$3); delete $3;}
                |       finishitems METALLIC
                                {$$=$1; $$->setkm(1);}
                |       finishitems REFRACTION expr
                                {$$=$1; $$->setkt((double)*$3); delete $3;}
                |       finishitems IOR expr
                                {$$=$1; $$->setki((double)*$3); delete $3;}
                ;
%%

void yyerror(char* s) {cout<<s<<"\n";}
