# AyaMath Development document

last edit time: 2019/11/9

author: g1n0st

## Basic Target

A lite utility linear algebra math library including basic functionalities of Matrix and Vector and some simple math  algorithms. 

As a component of AyaRay developed by [Chang Yu](http://g1n0st.xyz/). 

## Design Mode

+ **Support `SIMD`  hardware acceleration (as a default mode).**

+  Adapt the architecture of `pbrt-v3`, ought to achieve all the functions existed in `pbrt-v3`'s `gemetry.h` in the same interfaces.


+ Avoid the tedious codes of building different base classes of Vector/Point/Normal，which means unnecessary overloading, copying and conversion. Abstract all this classes into `vec3`,  all  subclasses inherit from it (including illegal methods)


+ Considering the encode demand of `SIMD` and the confusing error `LNK2019` caused by c++  template use. Only support `float` type.
+ All functions are implemented as class member functions, following object-oriented thinking to ensure that namespaces are not contaminated. (Some functions need to use like `Matrix3x3().getIdentity()`)


+ Only support Windows(x86) platform currently.


+ Guarantee multi-thread safety.


+ Follow Google code style

## File Structure

~

|-MathUtility.hpp

|-Vector3.hpp

|-BBox.hpp

|-Ray.hpp

|-Matrix3x3.hpp

|-Transform.hpp