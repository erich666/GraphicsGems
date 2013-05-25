#declare Black=color rgb<0,0,0>
#declare Red=color rgb<1,0,0>
#declare White=color rgb<1,1,1>
#declare Yellow=color rgb<1,1,0>
#declare Blue=color rgb<0,0,1>

camera{location<0,0,-4> direction<0,0,1> right<1,0,0> up<0,1,0>}
light_source{<0,0,-4> color White }

background {color Black}

sphere {5*z,3 pigment {color Yellow} finish {phong 0.9 phong_size 60 metallic}}
sphere {<0,0,3>,2 rotate <0,45,0> pigment {checker color Red color Yellow} finish{diffuse .9}}
sphere {<-2,-1,2> ,2 pigment {color Blue} normal {bumps 0.7 scale 0.8} finish{ phong 0.9 phong_size 80}}

sphere {<0,0,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<5,0,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<0,5,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<6,6,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<7,4,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<4,8,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<3,3,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<2,5,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<5,1,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<2,2,10>,.5 pigment {color White} finish{diffuse .7}}
sphere {<3,-3,10>,.5 pigment {color White} finish{diffuse .7}}
