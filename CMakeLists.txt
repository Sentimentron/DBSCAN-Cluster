cmake_minimum_required(VERSION 2.8)
project(Cluster)

add_definitions (-pedantic -std=c99)
add_executable (Cluster quadtree.c)
