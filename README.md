# MPI-filter-images

Using MPI, an image filtering program that uses .ppm image files.
The GUI is made with raylib.
## Build

With cmake.
```bash
./build.sh
```

## GUI Usage
```bash
./gui/run.sh
```
## Usage
```
Usage: mpiexec -np <proc_number> ./build/filter <filter_type> <input_file> <output_file>
Available Filter Types:
  - blur_3
  - blur_5
  - gaussian_3
  - gaussian_5
  - motion_blur
  - find_edges_horizontal
  - find_edges_vertical
  - find_edges_45_degrees
  - find_edges_all_directions
  - sharpen
  - sharpen_subtle
  - edge_detection_exaggerated
  - emboss_45_degrees
  - emboss_exaggerated
```