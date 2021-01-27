My first attempt at a rendering engine in OpenGL and C++. Written over the summer in 2018, as I was following along the tutorials from https://learnopengl.com/. The focus was on learning and experimenting with techniques, and exposing as much engine state to the UI as possible. 

Later, trying, and failing to refactor the engine to use a modular, demand-driven pipeline is what prompted me to start from scratch with my current side-project, [rendershop](https://github.com/alex-tdrn/rendershop).

HDRi's from https://hdrihaven.com/

Some eyecandy:

![alt text][shadowmapping]
![alt text][shadowmapping2]
![alt text][debug_depth]
![alt text][debug_maps1]
![alt text][debug_maps2]
![alt text][debug_maps4]
![alt text][debug_uvs]
![alt text][reflection]
![alt text][refraction]
![alt text][frustum_vis]
![alt text][frustum_vis2]
![alt text][frustum_vis3]
![alt text][wireframe]
![alt text][fuzzy_bunny]
![alt text][exploding_bunny]

[shadowmapping]: screenshots/shadowmapping.png "Shadow mapping"
[shadowmapping2]: screenshots/shadowmapping2.png "Shadow mapping"
[debug_depth]: screenshots/debug_depth.png "Depth debug shader"
[debug_maps1]: screenshots/debug_maps1.png "Debug normal map"
[debug_maps2]: screenshots/debug_maps2.png "Debug occlusion map"
[debug_maps3]: screenshots/debug_maps3.png "Debug base map"
[debug_maps4]: screenshots/debug_maps4.png "Debug metallic-roughness map"
[debug_uvs]: screenshots/debug_uvs.png "UV debug shader"
[frustum_vis]: screenshots/frustum_vis.png "Perspective frustum visualisation"
[frustum_vis2]: screenshots/frustum_vis2.png "Perspective frustum visualisation"
[frustum_vis3]: screenshots/frustum_vis3.png "Orthographic frustum visualisation"
[reflection]: screenshots/reflection.png "Reflection shader"
[refraction]: screenshots/refraction.png "Refraction shader"
[unlit]: screenshots/unlit.png "Unlit shader"
[wireframe]: screenshots/wireframe.png "Wireframe bunny"
[fuzzy_bunny]: screenshots/fuzzy_bunny.png "Fuzzy bunny"
[exploding_bunny]: screenshots/exploding_bunny.png "Exploding bunny"