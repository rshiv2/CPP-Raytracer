# CPP_Raytracer

A simple, header-only raytracing API. Partly for use in other graphics projects, mostly for my own education in graphics and C++.

To include this in projects, all you have to do is `#include rtcore.h`.

You can check out some examples in `./demo`. Just run `make build` while in that directory.

## Features

- Supports spheres and triangle meshes. Meshes can be loaded from OBJ files, as is done in the triangle scene in `./demo/main.cpp`.
- Provides acceleration structures such as Bounding Volume Hierarchies and AABBs. 


## TODOs

- [] Implement Texture and Sampler classes.
- [] Create Renderer class that accepts a scene, along with user parameters, and renders that scene. Currently, the task of  rendering a scene is left up to the user, though the `./demo` explains how to do it. Parameters should include the output texture dimensions, samples per pixel, maximum num of ray bounces, etc.
- [] Expand material class to include materials with a mixture of diffuse and specular components
- [] Implement Transform class for linear transformations of triangle mesh vertices and normals. Currently, these vertices and normals must be transformed manually :\
- [] Ray-scattering should be iterative. Currently, it is implemented recursively, which is more intuitive but inefficient, since recursive functions require lots of allocation on the stack.
- [] A lot of other stuff...

## References

[Raytracing In One Weekend ](https://raytracing.github.io/books/RayTracingInOneWeekend.html) 
    - For getting started

[Physically Based Rendering: From Theory to Implementation (3rd ed.)](https://pbr-book.org/)
    - For doing everything else