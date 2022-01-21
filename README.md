# Fluid Visualization

The code compilation is only tested on `Arch Linux x86_64`, Linux kernel `5.15.13-arch1`, with `gcc 11.1.0`, `CMake 3.22.1`, `Xorg X server 21.1.3`, **currently it does not support MSVC ver. CLI**.

Build with
```bash
mkdir build
cmake -G Ninja -B ./build
cmake --build ./build -j
```

The binaries are in `build/bin`.

Run the example by

```  
cd ./build/bin
./SPHInterface -s data/TwoCubes.json
```

Since cache needs to be warmed up, FPS will rise after a few seconds in, ideally 30 with `iterate=5, NParticles=13k` on AMD 5900X.

You can find the first scene settings in `data/TwoCubes.json`, all of which are very self-explanatory.

Almost all code is in `src/` directory for convenience, if one intends to add a solver, follow these steps:

- Check `Solver.hpp` and `Solver.cpp` for the definition of base class.
- Create new files in the `src/` directory, e.g. `NewSolver.hpp` and `NewSolver.cpp`, be sure to add `makeNewSolver()` in your source code.
- Add your solver to `src/IncludeSolvers.h`, then set your index of solver in scene settings. (Until of the date of writing the report, this feature has not been completed yet).

This project has two implementations of `NSearch`, one is in our code and the other one is imported by `etc/Ext_NSearch.cmake`.
You can switch between them by changing `makeNSearch` to or from `makeNSearchExt`.

This interface contains several navigation features:

- Drag with middle mouse button to rotate orbitally.
- Drag with right mouse button to zoom in and out.
- Press `<space>` to pause the simulation, you can still navigate around when the simulation is paused.
- Press `<enter>` to take the snapshot for particle positions, which is saved in the working directory.

In order to use the mesher in `build/bin/VDBMesher`, two parameters are required, the filename, i.e. `particles_1920.txt`, and the scale factor. Due to the limit of kernel size, which can be very small, our simulation is not performed on physical based measurement, therefore the particles might be scaled down by a factor `scale` so the final mesh output can be limited to an acceptable range. Generally, with simulation kernel radius of $3$, the scale is set to be approximately $0.6$, which exports a reasonable mesh.

The output file is always named `fluid.obj`, which is exported to the working directory.
