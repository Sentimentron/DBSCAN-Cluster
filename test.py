import pydbscan

if __name__ == "__main__":

    expected_clusters1 = [0 for i in range(6)]
    expected_clusters2 = [1,1,1,1,2,2]
    expected_clusters3 = [1,1,1,1,0,0]

    tree = pydbscan.create_quadtree(8, 8)
    assert pydbscan.quadtree_insert(tree, 0, 1)
    assert pydbscan.quadtree_insert(tree, 1, 1)
    assert pydbscan.quadtree_insert(tree, 1, 3)
    assert pydbscan.quadtree_insert(tree, 2, 0)
    assert pydbscan.quadtree_insert(tree, 2, 3)
    assert pydbscan.quadtree_insert(tree, 3, 0)
    assert pydbscan.quadtree_insert(tree, 4, 2)
    assert pydbscan.quadtree_insert(tree, 5, 2)
    assert pydbscan.quadtree_insert(tree, 5, 4)

    gen1 = pydbscan.pyDBSCAN(tree, 6, 0.0005, 2)
    gen2 = pydbscan.pyDBSCAN(tree, 6, 0.666, 2)
    gen3 = pydbscan.pyDBSCAN(tree, 6, 0.666, 3)

    print gen1
    print gen2
    print gen3

    assert gen1 == expected_clusters1
    assert gen2 == expected_clusters2
    assert gen3 == expected_clusters3

