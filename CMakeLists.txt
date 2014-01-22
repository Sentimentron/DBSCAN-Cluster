cmake_minimum_required(VERSION 2.8)
project(Cluster C)
enable_testing()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
              FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET (CMAKE_C_FLAGS "-pedantic -std=c99 -O0 -ggdb")
SET (CMAKE_LD_FLAGS "-g")

add_definitions(-Wall -W -g -O0)

add_executable (test_quadtree_init quadtree.c test_quadtree_init.c)
add_test(test_quadtree_init test_quadtree_init)

add_executable (test_quadtree_node_contains test_quadtree_node_contains.c quadtree.c)
add_test(test_quadtree_node_contains test_quadtree_node_contains)

add_executable (test_quadtree_insert_simple test_quadtree_insert_simple.c quadtree.c)
add_test (test_quadtree_insert_simple test_quadtree_insert_simple)

add_executable (test_quadtree_subdivide_1 test_quadtree_subdivide_1.c quadtree.c)
add_test (test_quadtree_subdivide_1 test_quadtree_subdivide_1)

add_executable(test_quadtree_insert_2 test_quadtree_insert_2.c quadtree.c)
add_test (test_quadtree_insert_2 test_quadtree_insert_2)

add_executable(test_quadtree_query test_quadtree_query.c quadtree.c)
add_test (test_quadtree_query test_quadtree_query)

add_executable(test_quadtree_insert_query_rand test_quadtree_insert_query_rand.c quadtree.c)
add_test (test_quadtree_insert_query_rand test_quadtree_insert_query_rand)

add_executable(test_quadtree_scan_x test_quadtree_scan_x.c quadtree.c)
add_test (test_quadtree_scan_x test_quadtree_scan_x)

add_executable(test_quadtree_scan_y test_quadtree_scan_y.c quadtree.c)
add_test (test_quadtree_scan_y test_quadtree_scan_y)

add_executable(test_dbscan_1 test_dbscan_1.c neighbours_naive.c quadtree.c dbscan.c)
add_test (test_dbscan_1 test_dbscan_1)

add_executable(test_dbscan_2 test_dbscan_2.c neighbours_naive.c quadtree.c dbscan.c)
add_test (test_dbscan_2 test_dbscan_2)

add_executable(test_dbscan_2a test_dbscan_2.c neighbours.c quadtree.c dbscan.c )
add_test (test_dbscan_2a test_dbscan_2a)

#add_executable (Cluster quadtree.c)
