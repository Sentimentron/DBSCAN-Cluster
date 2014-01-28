#include <assert.h>
#include <stdint.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dbscan.h"
#include "quadtree.h"

struct identifier_map_t{
    uint64_t *offset_to_identifier;
    QUADTREE *identifer_offset_map;
    QUADTREE *cluster_tree;
    uint64_t offset;
};


int nextpow2(int x) {
    int i;
    for (i = 1; i < x; i *= 2);
    return i;
}

int create_identity_mapping_callback(void *arg, int argc, char **argv, char **col) {

    struct identifier_map_t *map = (struct identifier_map_t *) arg;
    uint64_t count = strtoul(argv[0], NULL, 10);

    map->offset_to_identifier = malloc(sizeof(uint64_t) * count);

    if (map->offset_to_identifier == NULL) {
        fprintf(stderr, "%s\n", "Allocation error!");
        exit(1);
    }

    // Really super-inefficient use of the QUADTREE here
    if (quadtree_init(&(map->identifer_offset_map), nextpow2(count)-1, nextpow2(count)-1)) {
        fprintf(stderr, "%s\n", "Quadtree error!");
        exit(1);
    }

    map->offset = 0;

    return 0;
}

int insert_identity_mapping_callback(void *arg, int argc, char **argv, char **col) {

    uint64_t identifier;
    struct identifier_map_t *map = (struct identifier_map_t *) arg;

    identifier = strtoul(argv[0], NULL, 10);

    map->offset_to_identifier[map->offset] = identifier;
    if(!quadtree_insert(map->identifer_offset_map, map->offset, identifier)) {
        fprintf(stderr, "%s %d %d\n", "Quadtree insert error!", map->offset, identifier);
        exit(1);
    }

    map->offset++;
    return 0;
}

int create_tree_callback(void *arg, int argc, char **argv, char **col) {
    uint64_t identifier, label;
    unsigned int offset, out;
    struct identifier_map_t *map = (struct identifier_map_t *) arg;

    identifier = strtoul(argv[0], NULL, 10);
    label = strtoul(argv[1], NULL, 10);

    out = 0;
    if (quadtree_scan_y(map->identifer_offset_map, identifier, &offset, &out, 1)) {
        fprintf(stderr, "%s\n", "Offset scan error!");
        exit(1);
    }

    assert(out == 1);
    assert(quadtree_insert(map->cluster_tree, offset, label));
    return 0;
}

int main(int argc, char **argv) {
    const char * const db_location = "cluster.sqlite";
    const char * const count_query = "SELECT MAX(idn) FROM (SELECT DISTINCT document_identifier AS idn FROM temporary_label_clustering)";
    const char * const id_query = "SELECT DISTINCT document_identifier FROM temporary_label_clustering";
    const char * const select_query = "SELECT document_identifier, label FROM temporary_label_clustering";
    const char * const insert_query = "INSERT INTO temporary_label_clusters VALUES (?, ?)";
    QUADTREE *tree = NULL;
    sqlite3 *db = NULL;
    char *zErrMsg = NULL;
    int rc = 0;
    struct identifier_map_t id_map;
    unsigned int *clusters;
    sqlite3_stmt *insert_statement = NULL;



    // Open the database
    rc = sqlite3_open(db_location, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Reading identifiers...\n");
    rc = sqlite3_exec(db, count_query, create_identity_mapping_callback, &id_map, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_exec(db, id_query, insert_identity_mapping_callback, &id_map, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Creating quadtree...\n");
    assert(!quadtree_init(&tree, nextpow2(id_map.offset)-1, nextpow2(id_map.offset)-1));
    id_map.cluster_tree = tree;

    rc = sqlite3_exec(db, select_query, create_tree_callback, &id_map, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_prepare(db, insert_query, -1, &insert_statement, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: failed to prepare statement: '%s'\n", sqlite3_errmsg(db));
        return 1;
    }

    clusters = calloc(sizeof(unsigned int), id_map.offset);
    fprintf(stderr, "Clustering...");
    assert(!DBSCAN(tree, clusters, id_map.offset, 0.20, 2, &neighbours_search));
    for (int i = 0 ; i < id_map.offset; i++) {
        unsigned int cluster = clusters[i];
        unsigned int identifier = id_map.offset_to_identifier[i];

        // Skip if blank
        if (!cluster) continue;

        printf("%d %d\n", identifier, cluster);
        rc = sqlite3_bind_int64(insert_statement, 1, identifier);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "ERROR: failed to bind identifier\n%s", sqlite3_errmsg(db));
            return 1;
        }
        rc = sqlite3_bind_int64(insert_statement, 2, cluster);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "ERROR: failed to bind identifier\n%s", sqlite3_errmsg(db));
            return 1;
        }
        rc = sqlite3_step(insert_statement);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "ERROR: insertion error\n%s", sqlite3_errmsg(db));
            return 1;
        }
        rc = sqlite3_reset(insert_statement);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "ERROR: cannot reset statement\n%s", sqlite3_errmsg(db));
            return 1;
        }
    }

    sqlite3_close(db);
}
