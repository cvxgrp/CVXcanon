# CVXcanon

## Introduction
Convex optimization modeling tools like CVX, CVXPY, and Convex.Jl all translate high-level problem descriptions into low-level, canonical forms that are then passed to an backend solver. We introduce CVXCanon, a software package that factors out the common operations that all such modeling systems perform into a single library with a simple C interface. CVXCanon not only removes the need to reimplement this canonicalization process in new languages, but also provides significant performance gains on many systems.


## Usage with CVXPY
To install CVXCanon with CVXPY, simply run ```pip install cvxcanon```. From there, simply set the flag ```cvxpy.settings.USE_CANON = TRUE``` in your Python code:

``` python
from cvxpy import *
settings.USE_CVXCANON = True
x = Variable()
Minimize(x, [x == 0]).solve()
```
Note that on the use of CVXPY's Parameters is currently disabled in CVXCanon. One can expect a 2 - 4x  reduction in solve time on most other problems.


## Layout
- **/src** contains the source code for CVXCanon
	- **CVXcanon.(c/h)pp** implements our matrix building algorithm. This file also provides the main access point into CVXCanon's functionality, the ```build_matrix``` function.
	-  **LinOp.hpp** defines the LinOp class, our linear atoms which we traverse when building our matrix.
	- **LinOpOperations.(c/h)pp** defines functions which we can use to get coefficients corresponding to each of the LinOp. This includes 18 special cases, one for each LinOp.
    - **ProblemData.hpp** defines the structure returned by ```build_matrix```, which incldues a sparse representation of our problem matrix and our constant vector. 






