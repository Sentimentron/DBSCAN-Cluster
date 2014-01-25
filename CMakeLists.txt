cmake_minimum_required(VERSION 2.6)
project(Cluster C)
enable_testing()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
              FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET (CMAKE_C_FLAGS "-pedantic -std=c99 -O3 -ggdb -march=native")
SET (CMAKE_LD_FLAGS "-static -g")

add_definitions(-Wall -W -g -O0)

add_executable (test_neighbour_distance test_neighbour_distance.c neighbours.c bitvec.c quadtree.c)
add_test(test_neighbour_distance test_neighbour_distance)

add_executable (test_bitvec_alloc test_bitvec_alloc.c bitvec.c)
add_test(test_bitvec_alloc test_bitvec_alloc)

add_executable (test_bitvec_set test_bitvec_set.c bitvec.c)
add_test(test_bitvec_set test_bitvec_set)

add_executable (test_bitvec_set_extend test_bitvec_set_extend.c bitvec.c)
add_test(test_bitvec_set_extend test_bitvec_set_extend)

add_executable (test_bitvec_set_extend_clear test_bitvec_set_extend_clear.c bitvec.c)
add_test(test_bitvec_set_extend_clear test_bitvec_set_extend_clear)

add_executable (test_bitvec_clear test_bitvec_clear.c bitvec.c)
add_test(test_bitvec_clear test_bitvec_clear)

add_executable (test_bitvec_check test_bitvec_check.c bitvec.c)
add_test(test_bitvec_check test_bitvec_check)

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

add_executable(test_quadtree_scan_y_sorted test_quadtree_scan_y_sorted.c quadtree.c)
add_test (test_quadtree_scan_y_sorted test_quadtree_scan_y_sorted)

add_executable(test_quadtree_integrity test_quadtree_integrity.c quadtree.c)
add_test (test_quadtree_integrity test_quadtree_integrity)
target_link_libraries(test_quadtree_integrity sqlite3)


add_executable(test_dbscan_1 test_dbscan_1.c neighbours_naive.c quadtree.c dbscan.c bitvec.c)
add_test (test_dbscan_1 test_dbscan_1)

add_executable(test_dbscan_2 test_dbscan_2.c neighbours_naive.c quadtree.c dbscan.c bitvec.c)
add_test (test_dbscan_2 test_dbscan_2)

add_executable(test_dbscan_2a test_dbscan_2.c neighbours.c quadtree.c dbscan.c bitvec.c)
add_test (test_dbscan_2a test_dbscan_2a)

add_executable (cluster quadtree.c dbscan.c bitvec.c cluster.c neighbours.c)
target_link_libraries(cluster sqlite3)
