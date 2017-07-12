# Osteomesh
![Osteomesh](https://raw.githubusercontent.com/rics-ucsp/Osteomesh/osteomesh_dev/screenshots/osteomesh_mainview.png)

# Description
Osteomesh is a desktop application for diagnosing osteoporosis in 3D CT Images.


# Features
  - Import DICOM, VTK, XDMF, RAW files.
  - 3D Visualizer for Images and Meshes.
  - Algorithms for image-meshing, mesh-segmentation, mesh-refinement, and diagnosis.  
  - Currently built as a *Paraview Custom Application*.

### Installation

Osteomesh requires a [full build of Paraview](https://www.paraview.org/Wiki/ParaView:Build_And_Install) in order to build.

**_Shared Libraries_**: Point to the Paraview libraries directory in the PATH (Windows) or LD_LIBRARY_PATH (Linux) in order to run.

To build and run:
```sh
$ mkdir build && cd build
$ cmake -GNinja ..
$ ninja -v
```
