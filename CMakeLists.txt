cmake_minimum_required(VERSION 2.8)
project(Cluster)
enable_testing()

add_definitions (-pedantic -std=c99)

add_executable (test_quadtree_init quadtree.c test_quadtree_init.c)
add_test(test_quadtree_init test_quadtree_init)

add_executable (Cluster quadtree.c)
