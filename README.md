DBSCAN-Cluster
==============

DBSCAN implementation in C. Uses a quadtree datastructure to handle very large, sparse, binary feature spaces. Implements Jaccard distance as the default distance metric (`neighbours.c`).

## Building and running the tests
1. Use `cmake .` to generate the build files.
2. Run `make` to build the library, applications and Python SWIG wrapper (if applicable)
3. Run `make test` to run the tests. 

## Using the Python wrapper

See `test.py` for an example.

### Create the quadtree
```
  import pydbscan
  tree = pydbscan.create_quadtree(8, 8)
```
* Its recommended to have height and width as the same power of two.

### Insert points into the quadtree
```
  pydbscan.quadtree_insert(tree, 0, 1) # Sets a 1 for x = 0, y = 1
```
* This function returns 0 if the insert did not succeed (e.g. the point was out of range or was already set).
* For most typical applications, the *document* is the first argument, the label is the second.
* It's recommended to count documents from zero.

### Cluster 
```
  pydbscan.pyDBSCAN(tree, 6, 0.67, 2)
```
* The first argument is the number of documents input into the quadtree.
* The second is the epsilon value (points are considered part of another's neighbourhood if their distance is less than 1 - epsilon).
* The third argument is the minimum number of points needed to make a cluster.

## Using the C API
* `quadtree_init` allocates and initialises the quadtree (arguments must be one less than a powers of two).
* `quadtree_insert` adds a document-label pair in the quadtree, and returns a non-zero value if the insert succeeded.
* `DBSCAN` takes a quadtree as a reference, an array of unsigned integers of length of the size of the documents, the number of documents, the epsilon value,the minimum points, and a pointer to a neighbourhood distance function 
** `neighbours_search` implements neighborhood filtering via the Jaccard index.
