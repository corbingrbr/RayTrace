// cs473, simple color bleeding set up

camera {
  location  <-2, 0, 14>
  up        <0,  1,  0>
  right     <1.333333, 0,  0>
  look_at   <0, 0, 0>
}


light_source {<5, 10, 5> color rgb <0.5, 0.5, 0.5>}

// floor
plane {<0, 1, 0>, -3
  pigment {color rgb <0.8, 0.2, 0.2>}
  finish {ambient 0.1 diffuse 0.8}
  translate <0, -1, 0>
}

// back wall
plane {<0, 0, 1>, -5
  pigment {color rgb <0.2, 0.8, 0.2>}
  finish {ambient 0.1 diffuse 0.8}
}

// left side wall
plane {<1, 0, 0>, -5 
    pigment {color rgb <0.0, 0.0, 1.0>}
    finish {ambient 0.1 diffuse 0.8}    
}


// right side wall
plane {<-1, 0, 0>, 5 
    pigment {color rgb <0.0, 0.0, 1.0>}
    finish {ambient 0.1 diffuse 0.8}    
}

sphere {<-1.0, -1.0, -1.0>, 2.0
  pigment {color rgb <1.0, 1.0, 1.0>}
  finish {ambient 0.1 diffuse 0.8 reflection 0.6}
}

sphere {<1.0, 2.0, -1.0>, 1.0
  pigment {color rgb <1.0, 1.0, 1.0>}
  finish {ambient 0.1 diffuse 0.8 refraction 1.0 ior 1.5}
}


