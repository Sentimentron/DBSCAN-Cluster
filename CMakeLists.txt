cmake_minimum_required(VERSION 2.6 C)
project(Cluster)

add_definitions (-pedantic -std=c99)
add_executable (Cluster quadtree.c)
