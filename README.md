# Osteomesh
![Osteomesh](https://raw.githubusercontent.com/rics-ucsp/Osteomesh/master/screenshots/osteomesh_mainview.png)

# Description
Osteomesh is a desktop application for diagnosing osteoporosis in 3D CT Images.


# Features
  - Import DICOM, VTK, XDMF, RAW files.
  - 3D Visualizer for Images and Meshes.
  - Algorithms for image-meshing, mesh-segmentation, mesh-refinement, and diagnosis.  
  - Currently built as a *Paraview Custom Application*.

### Installation

Osteomesh requires a full install of [Paraview](https://www.paraview.org/) in order to build.

**_Shared Libraries_**: Add the Paraview libraries directory to the PATH (Windows) or LD_LIBRARY_PATH (Linux) in order to run. 

To build and run:
```sh
$ cmake .
$ make
$ ./paraview_revised_2
```
