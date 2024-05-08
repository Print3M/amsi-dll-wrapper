# Manual DLL-Wrapping Technique (AMSI DLL-Implant)

Visit related blog post for a better understanding of this code: [print3m.github.io/blog/amsi-dll-wrapper](https://print3m.github.io/blog/amsi-dll-wrapper)

This repository demonstrates a technique for creating DLL-Wrappers using a manual implementation of each exported function from the original DLL module. The built-in Windows `amsi.dll` was used as an example here. This technique can be used to perform some **more stealthy DLL-Hijacking tactics**, or for conveniently manipulating module parameters (function input and output) to get a better idea of what is going on.
