#!/usr/bin/env python
import numpy as np
import math

SPH_KernelSize = 0.1
SPH_KernelMass = 1.0
print(f'KernelSize: {SPH_KernelSize} m, KernelMass: {SPH_KernelMass} kg')
SPH_SupportRadius = (3.0 * 1 * 32 / (4.0 * math.pi * 1000))**(1/3.0)
print(SPH_SupportRadius)
SPH_KernelSize = SPH_SupportRadius


def Poly6(r):
    r = np.clip(r, 0, SPH_KernelSize)
    d = SPH_KernelSize
    t = (d * d - r * r) / (d * d * d)
    return 315.0 / (64 * math.pi) * t * t * t

def Poly6_2(r):
    r = np.clip(r, 0, SPH_KernelSize)
    d = SPH_KernelSize
    return 315.0 / (64.0 * math.pi * math.pow(d, 9)) * math.pow(d**2 - r**2, 3)


def GradSpiky(v):
    r = math.length(v)
    d = SPH_KernelSize
    if r == 0.0:
        return np.array([0.0, 0.0, 0.0])
    return -45.0 / (math.pi * math.pow(d, 6)) * math.pow(d - r, 2) * v / r


def EvaluateDensityKernel(SPH_SpawnDistance):
    currentPosition = np.array([0.0, 0.0, 0.0])
    kernelVector = np.array([SPH_KernelSize, SPH_KernelSize, SPH_KernelSize])
    spawnVector = np.array(
        [SPH_SpawnDistance, SPH_SpawnDistance, SPH_SpawnDistance])
    lowerPosition = currentPosition - kernelVector
    nx = int(SPH_KernelSize * 2 / SPH_SpawnDistance)
    ny = nx
    nz = nx

    SPH_Density = 0.0
    SPH_NNeighbor = 0
    for x, y, z in np.ndindex((nx, ny, nz)):
        neighborPosition = lowerPosition + \
            spawnVector * np.array([x, y, z])
        distance = np.linalg.norm(neighborPosition - currentPosition)
        if distance < SPH_KernelSize:
            SPH_Density += SPH_KernelMass * Poly6(distance)
            SPH_NNeighbor += 1
    return SPH_Density, SPH_NNeighbor


def EvaluateDensity(SPH_SpawnDistance):
    # Acquire SPH_SpawnDistance from global variable
    print(f'Density: {EvaluateDensityKernel(SPH_SpawnDistance)} kg/m^3')
    print("Kernel invoked finished")


def main():
    EvaluateDensity(SPH_KernelSize * 0.5)
    print(Poly6(0.01))


if __name__ == '__main__':
    main()
