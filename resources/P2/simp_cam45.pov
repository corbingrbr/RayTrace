camera {
    location  <7, 0,7> 
    up        <0,  1,  0>
    right     <.94, 0,  -.94>
    look_at   <0, 0, 0>
}

light_source { <-100, 100, 100> color rgb <1.5, 1.5, 1.5> }

sphere { <-3.5, 0, 0>, 1
  pigment { color rgb <1.0, 0.0, 1.0>}
  finish { ambient 0.2 diffuse 0.4 specular 0.1 roughness 0.01 }
}

sphere { <1, 0, 0>, 1.5 
  pigment { color rgb <0.0, 1.0, 1.0>}
  finish { ambient 0.2 diffuse 0.4 specular 0.1 roughness 0.01 }
}

sphere { <-2.5, 0, -4>, 2 
  pigment { color rgb <1.0, 1.0, 0.0>}
  finish { ambient 0.2 diffuse 0.4 specular 0.1 roughness 0.01 }
}

sphere { <2.8, 0, -4>, 2.5
  pigment { color rgb <0.1, 1.0, 0.5> }
}