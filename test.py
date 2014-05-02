import pydbscan

if __name__ == "__main__":

    expected_clusters1 = [0 for i in range(6)]
    expected_clusters2 = [1,1,1,1,2,2]
    expected_clusters3 = [1,1,1,1,0,0]

    tree = pydbscan.create_quadtree(8, 8)
    assert pydbscan.quadtree_insert(tree, 0, 1)
    assert pydbscan.quadtree_insert(tree, 1, 1)
    assert pydbscan.quadtree_insert(tree, 0, 3)
    assert pydbscan.quadtree_insert(tree, 2, 0)
    assert pydbscan.quadtree_insert(tree, 0, 3)
    assert pydbscan.quadtree_insert(tree, 3, 0)
    assert pydbscan.quadtree_insert(tree, 4, 2)
    assert pydbscan.quadtree_insert(tree, 5, 2)
    assert pydbscan.quadtree_insert(tree, 5, 4)



