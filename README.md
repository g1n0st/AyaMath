# AyaMath Development document

last edit time: 2019/11/6 

author: g1n0st

## Basic purpose

A lite utility linear algebra math library including basic functionalities of Matrix and Vector and some simple math  algorithms. 

As a component of AyaRay developed by [Chang Yu](http://g1n0st.xyz/). 

## Design Mode

+  Adapt the architecture of `pbrt-v3`, ought to achieve all the functions existed in `pbrt-v3`'s `gemetry.h` in the same interface.


+ Avoid the tedious codes of building different base classes of Vector/Point/Normal，which means unnecessary overloading, copying and conversion. Abstract all this classes into `vec3`,  all             subclasses inherit from it (including illegal methods)


+ Considering the encode demand of `SIMD` and the confusing error `LNK2019` caused by c++  template use. Only support `float` and `double` types by control of macro switch.


+ Support `SIMD`  hardware acceleration (as a default mode).


+ Support different platforms (Linux/Windows) 


+ Guarantee multi-thread safety.


+ Follow Google code style

## File structure

~

|-constant.hpp

|-ayamath.hpp

|-vector.hpp

|-bbox.hpp

|-ray.hpp

|-matrix.hpp

|-transform.hpp