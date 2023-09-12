# CPP_Raytracer

A simple raytracing API. Partly for use in other graphics projects, mostly for my own education in graphics and C++.

To include this in other projects, first build this API by navigating to `lib`. From there:
- `mkdir build && cd build && cmake .. `

This set of commands will create two libraries, `libRtxRelease` and `libRtxDebug`. The __Release__ version
has all the compiler optimizations that make this library run fast, while the __Debug__ version has debug
symbols in case you want to inspect the API in a debugger. For all intents and purposes, the __Release__
version should be fine. Link this library with your project, and #include `include/rtcore.h` in your main file.

You can check out some examples in `./demo`. Just run `mkdir build && cd build && cmake .. && make` to run it.

## Features

- Renders spheres (still and moving) and triangle meshes. Meshes can be loaded from OBJ files, as shown in `./demos/main.cpp`.
- Lazy construction of acceleration structures such as Bounding Volume Hierarchies and AABBs. 
- Different surface material types, such as Lambertian, Dielectric, Metallic, and Glossy. 
- A Transform class for linear transformations of objects. 


## TODOs

- [ ] Implement Texture and Sampler classes.
- [ ] Create Renderer class that accepts a scene, along with user parameters, and renders that scene. Currently, the task of  rendering a scene is left up to the user, though the `./demo` explains how to do it. Parameters should include the output texture dimensions, samples per pixel, maximum num of ray bounces, etc.
- [ ] Ray-scattering should be iterative. Currently, it is implemented recursively, which is more intuitive but inefficient, since recursive functions require lots of allocation on the stack.
- [ ] A lot of other stuff...

## References

[Raytracing In One Weekend ](https://raytracing.github.io/books/RayTracingInOneWeekend.html) 
    - For getting started

[Physically Based Rendering: From Theory to Implementation (3rd ed.)](https://pbr-book.org/)
    - For doing everything else
