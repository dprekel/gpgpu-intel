# OpenCL matrix multiplication

## Compute Kernel
The kernel multiplies two matrices $A\cdot B = C$. In our example all matrices are quadratic with $A, B, C\in\mathbb{R}^{3968\times 3968}$. 
The kernel assumes that $A$ and $C$ are stored in column-major order while $B$ is stored in row-major order.
The resulting matrix $C$ can be partitioned into chunks of predefined size. 
These chunks of data are called tiles. 
Each tile is computed by one OpenCL work item.
There are two macros that determine the tile size:

- `TILE_SIZE_N`: size (number of matrix elements) of a tile along the x dimension
- `TILE_SIZE_M`: size (number of matrix elements) of a tile along the y dimension

Two other macros are used to set the work group size:

- `TILE_GROUP_N`: number of tiles/work items that make up a work group along the x dimension
- `TILE_GROUP_M`: number of tiles/work items that make up a work group along the y dimension

The kernel starts with setting the initial indices:

```c
int Aind = get_group_id(0)*TILE_GROUP_M*TILE_SIZE_M + get_local_id(0);
int Bind = get_group_id(1)*TILE_GROUP_N*TILE_SIZE_N + get_local_id(1);
int Cind = Aind + Bind*ldc;
```

