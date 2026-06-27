cmake_minimum_required(VERSION 3.16)
project(RoutePack C)

set(CMAKE_C_STANDARD 11)

add_executable(RoutePack
        main.c
        client_bst.h
        client_bst.c
        package_list.h
        package_list.c
        package_avl.h
        package_avl.c
        delivery_queue.h
        delivery_queue.c
        returns_stack.h
        returns_stack.c
        routes_graph.h
        routes_graph.c
        files.h
        files.c)
