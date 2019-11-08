# AyaMath Development document

last edit time: 2019/11/8

author: g1n0st

## Basic Target

A lite utility linear algebra math library including basic functionalities of Matrix and Vector and some simple math  algorithms. 

As a component of AyaRay developed by [Chang Yu](http://g1n0st.xyz/). 

## Design Mode

+ **Support `SIMD`  hardware acceleration (as a default mode).**

+  Adapt the architecture of `pbrt-v3`, ought to achieve all the functions existed in `pbrt-v3`'s `gemetry.h` in the same interface.


+ Avoid the tedious codes of building different base classes of Vector/Point/Normal，which means unnecessary overloading, copying and conversion. Abstract all this classes into `vec3`,  all             subclasses inherit from it (including illegal methods)


+ Considering the encode demand of `SIMD` and the confusing error `LNK2019` caused by c++  template use. Only support `float` and `double` types by control of macro switch.


+ Only support Windows(x86) platform currently.


+ Guarantee multi-thread safety.


+ Follow Google code style

## File Structure

~

|-Constant.hpp

|-MathUtility.hpp

|-Vector3.hpp

|-BBox.hpp

|-Ray.hpp

|-Matrix4x4.hpp

|-Transform.hpp