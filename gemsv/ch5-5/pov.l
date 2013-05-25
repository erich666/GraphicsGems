%e      3000
%p      7000
%n      1000
%a      3000

%s      COMMENT


W       [ \t]+
D       [0-9]
E       [DEde][-+]?{D}+

%{

#include <stdio.h>

int comment;

template <class T> struct stackelem {
        T v;
        stackelem<T> *n;
};

template <class T> class stack {
        stackelem<T> *h;
public:
        stack() {h=(stackelem<T>*)0;}
        ~stack() {stackelem<T> *n; for(;h;h=n) {n=h->n; delete h;}}
        void operator<<(T& v) {
                stackelem<T> *e=new stackelem<T>; e->v=v; e->n=h; h=e;
        }
        void operator>>(T& v) {stackelem<T> *e=h; h=e->n; v=e->v; delete e;}
        int empty() {return h==(stackelem<T>*)0;}
};

struct fileident {
        char* n;
        FILE* f;
        int l;
        fileident() {}
        fileident(char* n, FILE* f, int l) {this->n=n; this->f=f; this->l=l;}
};

stack<fileident> files;
FILE* inputfile=stdin;
char* inputfilename="";
int line=1;

INPUT() {
        int c=fgetc(inputfile);
        fileident f;
        switch(c) {
            case EOF:
                fprintf(stderr, "\n");
                if(files.empty())
                        return 0;
                fclose(inputfile); delete inputfilename;
                files>>f; inputfile=f.f; inputfilename=f.n; line=f.l;
                return '\n';
            case '\n':
                line++;
                fprintf(stderr,"reading line %d of file %s\r",line,inputfilename);
        }
        return c;
}

UNPUT(int c) {return ungetc(c, inputfile);}

#undef input
#define input() INPUT()
#undef unput
#define unput(c) UNPUT(c)

void include(char* s) {
        register int i; char *n; fileident f(inputfilename,inputfile,line);
        files<<f;
        for(i=0; s[i]!='"'; i++); n=&s[i+1];
        for(i=0; n[i]!='"'; i++); n[i]='\0'; 
        inputfilename=new char[strlen(n)+1]; strcpy(inputfilename,n);
        inputfile=fopen(n, "r");
        line=1;
}

%}

%%

"/*"                               {comment=1; BEGIN COMMENT;}
<COMMENT>"/*"                      {comment++;}
<COMMENT>"*/"                      {comment--; if(!comment) {BEGIN INITIAL;}}
<COMMENT>.                         ;

<INITIAL>"*/"                      printf("unmatched */ (ignored)\n");
<INITIAL>"//".*$                   ;

<INITIAL>#include{W}\"[^\"\n]*\"   include((char*)yytext);

<INITIAL>adaptive                  return ADAPTIVE;
<INITIAL>agate                     return AGATE;
<INITIAL>agate_turb                return AGATE_TURB;
<INITIAL>all                       return ALL;
<INITIAL>alpha                     return ALPHA;
<INITIAL>ambient                   return AMBIENT;
<INITIAL>area_light                return AREA_LIGHT;
<INITIAL>background                return BACKGROUND;
<INITIAL>bicubic_patch             return BICUBIC_PATCH;
<INITIAL>blob                      return BLOB;
<INITIAL>blue                      return BLUE;
<INITIAL>bounded_by                return BOUNDED_BY;
<INITIAL>box                       return BOX;
<INITIAL>bozo                      return BOZO;
<INITIAL>brick                     return BRICK;
<INITIAL>brilliance                return BRILLIANCE;
<INITIAL>bumps                     return BUMPS;
<INITIAL>bumpy1                    return BUMPY1;
<INITIAL>bumpy2                    return BUMPY2;
<INITIAL>bumpy3                    return BUMPY3;
<INITIAL>bump_map                  return BUMP_MAP;
<INITIAL>bump_size                 return BUMP_SIZE;
<INITIAL>camera{W}identifier       return CAMERA_ID;
<INITIAL>camera                    return CAMERA;
<INITIAL>checker                   return CHECKER;
<INITIAL>clipped_by                return CLIPPED_BY;
<INITIAL>clock                     return CLOCK;
<INITIAL>colour{W}identifier       return COLOR_ID;
<INITIAL>colour{W}map{W}identifier return COLOR_MAP_ID;
<INITIAL>color_map                 return COLOR_MAP;
<INITIAL>colour_map                return COLOR_MAP;
<INITIAL>color                     return COLOR;
<INITIAL>colour                    return COLOR;
<INITIAL>component                 return COMPONENT;
<INITIAL>composite                 return COMPOSITE;
<INITIAL>cone                      return CONE;
<INITIAL>crand                     return CRAND;
<INITIAL>cubic                     return CUBIC;
<INITIAL>cylinder                  return CYLINDER;
<INITIAL>declare                   return DECLARE;
<INITIAL>default                   return DEFAULT;
<INITIAL>dents                     return DENTS;
<INITIAL>difference                return DIFFERENCE;
<INITIAL>diffuse                   return DIFFUSE;
<INITIAL>direction                 return DIRECTION;
<INITIAL>disc                      return DISC;
<INITIAL>distance                  return DISTANCE;
<INITIAL>dump                      return DUMP;
<INITIAL>End{W}of{W}File           return END_OF_FILE;
<INITIAL>falloff                   return FALLOFF;
<INITIAL>filter                    return FILTER;
<INITIAL>finish{W}identifier       return FINISH_ID;
<INITIAL>finish                    return FINISH;
<INITIAL>flatness                  return FLATNESS;
<INITIAL>float{W}identifier        return FLOAT_ID;
<INITIAL>float{W}constant          return FLOAT;
<INITIAL>fog                       return FOG;
<INITIAL>frequency                 return FREQUENCY;
<INITIAL>gif                       return GIF;
<INITIAL>gradient                  return GRADIENT;
<INITIAL>granite                   return GRANITE;
<INITIAL>green                     return GREEN;
<INITIAL>height_field              return HEIGHT_FIELD;
<INITIAL>hexagon                   return HEXAGON;
<INITIAL>undeclared{W}identifier   return IDENTIFIER;
<INITIAL>iff                       return IFF;
<INITIAL>image_map                 return IMAGE_MAP;
<INITIAL>interpolate               return INTERPOLATE;
<INITIAL>intersection              return INTERSECTION;
<INITIAL>inverse                   return INVERSE;
<INITIAL>ior                       return IOR;
<INITIAL>jitter                    return JITTER;
<INITIAL>lambda                    return LAMBDA;
<INITIAL>leopard                   return LEOPARD;
<INITIAL>light_source              return LIGHT_SOURCE;
<INITIAL>location                  return LOCATION;
<INITIAL>looks_like                return LOOKS_LIKE;
<INITIAL>look_at                   return LOOK_AT;
<INITIAL>mandel                    return MANDEL;
<INITIAL>map_type                  return MAP_TYPE;
<INITIAL>marble                    return MARBLE;
<INITIAL>material_map              return MATERIAL_MAP;
<INITIAL>max_intersections         return MAX_INTERSECTIONS;
<INITIAL>max_trace_level           return MAX_TRACE_LEVEL;
<INITIAL>merge                     return MERGE;
<INITIAL>metallic                  return METALLIC;
<INITIAL>mortar                    return MORTAR;
<INITIAL>normal                    return NORMAL;
<INITIAL>no_shadow                 return NO_SHADOW;
<INITIAL>object{W}identifier       return OBJECT_ID;
<INITIAL>object                    return OBJECT;
<INITIAL>octaves                   return OCTAVES;
<INITIAL>omega                     return OMEGA;
<INITIAL>once                      return ONCE;
<INITIAL>onion                     return ONION;
<INITIAL>open                      return OPEN;
<INITIAL>painted1                  return PAINTED1;
<INITIAL>painted2                  return PAINTED2;
<INITIAL>painted3                  return PAINTED3;
<INITIAL>phase                     return PHASE;
<INITIAL>phong_size                return PHONG_SIZE;
<INITIAL>phong                     return PHONG;
<INITIAL>pigment{W}identifier      return PIGMENT_ID;
<INITIAL>pigment                   return PIGMENT;
<INITIAL>plane                     return PLANE;
<INITIAL>point_at                  return POINT_AT;
<INITIAL>poly                      return POLY;
<INITIAL>pot                       return POT;
<INITIAL>quadric                   return QUADRIC;
<INITIAL>quartic                   return QUARTIC;
<INITIAL>quick_color               return QUICK_COLOR;
<INITIAL>quick_colour              return QUICK_COLOR;
<INITIAL>radial                    return RADIAL;
<INITIAL>radius                    return RADIUS;
<INITIAL>raw                       return RAW;
<INITIAL>red                       return RED;
<INITIAL>reflection                return REFLECTION;
<INITIAL>refraction                return REFRACTION;
<INITIAL>rgbf                      return RGBF;
<INITIAL>rgb                       return RGB;
<INITIAL>right                     return RIGHT;
<INITIAL>ripples                   return RIPPLES;
<INITIAL>rotate                    return ROTATE;
<INITIAL>roughness                 return ROUGHNESS;
<INITIAL>scale                     return SCALE;
<INITIAL>sky                       return SKY;
<INITIAL>smooth                    return SMOOTH;
<INITIAL>smooth_triangle           return SMOOTH_TRIANGLE;
<INITIAL>specular                  return SPECULAR;
<INITIAL>sphere                    return SPHERE;
<INITIAL>spotlight                 return SPOTLIGHT;
<INITIAL>spotted                   return SPOTTED;
<INITIAL>string                    return STRING;
<INITIAL>sturm                     return STURM;
<INITIAL>texture{W}identifier      return TEXTURE_ID;
<INITIAL>texture                   return TEXTURE;
<INITIAL>tga                       return TGA;
<INITIAL>threshold                 return THRESHOLD;
<INITIAL>tightness                 return TIGHTNESS;
<INITIAL>tile2                     return TILE2;
<INITIAL>tiles                     return TILES;
<INITIAL>tnormal{W}identifier      return TNORMAL_ID;
<INITIAL>torus                     return TORUS;
<INITIAL>track                     return TRACK;
<INITIAL>transform{W}identifier    return TRANSFORM_ID;
<INITIAL>transform                 return TRANSFORM;
<INITIAL>translate                 return TRANSLATE;
<INITIAL>triangle                  return TRIANGLE;
<INITIAL>turbulence                return TURBULENCE;
<INITIAL>type                      return TYPE;
<INITIAL>union                     return UNION;
<INITIAL>up                        return UP;
<INITIAL>use_color                 return USE_COLOR;
<INITIAL>use_colour                return USE_COLOR;
<INITIAL>use_index                 return USE_INDEX;
<INITIAL>u_steps                   return U_STEPS;
<INITIAL>vector{W}identifier       return VECTOR_ID;
<INITIAL>version                   return VERSION;
<INITIAL>v_steps                   return V_STEPS;
<INITIAL>water_level               return WATER_LEVEL;
<INITIAL>waves                     return WAVES;
<INITIAL>wood                      return WOOD;
<INITIAL>wrinkles                  return WRINKLES;
<INITIAL>x                         return X;
<INITIAL>y                         return Y;
<INITIAL>z                         return Z;

<INITIAL>{D}+                 |
<INITIAL>{D}+"."{D}*({E})?    |
<INITIAL>{D}*"."{D}+({E})?    |
<INITIAL>{D}+{E}              {yylval.dval=atof((char*)yytext);return LITERAL;}
<INITIAL>[a-zA-Z][a-zA-Z0-9]* {yylval.sval=(char*)yytext; return IDENTIFIER;}

<INITIAL>{W}                  ;
<INITIAL>\n                   ;

<INITIAL>.                    {return yytext[0];}
