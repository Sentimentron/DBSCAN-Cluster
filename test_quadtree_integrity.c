#include <assert.h>
#include <stdint.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "quadtree.h"

int create_tree_callback(void *arg, int argc, char **argv, char **col) {
    uint64_t identifier, label;
    QUADTREE *tree = (QUADTREE *)arg; 

    identifier = strtoul(argv[0], NULL, 10);
    label = strtoul(argv[1], NULL, 10);

    assert(quadtree_insert(tree, identifier, label));
    return 0;
}

int create_validation_callback(void *arg, int argc, char **argv, char **col) {
    uint64_t identifier, label, i, j;
    uint64_t *val = (uint64_t *)arg;

    identifier = strtoul(argv[0], NULL, 10);
    label = strtoul(argv[1], NULL, 10);

    for (i = 0; i < 46431; i++) {
        uint64_t *off = val + (i * 15);
        if (!(*off)) {
            *off = identifier;
        }
        if (*off == identifier) {
            break;
        }
    }

    for (j = 1; j <= 15; j++) {
        uint64_t *off = val + (i * 15) + j;
        if (!(*off)) {
            *off = label;
            return 0;
        }
    }

    assert(0);
    return 1;

}

int main(int argc, char **argv) {
    const char * const db_location = "cluster.sqlite";
    const char * const select_query = "SELECT document_identifier, label FROM temporary_label_clustering";
    QUADTREE *tree = NULL;
    sqlite3 *db = NULL;
    char *zErrMsg = NULL;
    int rc = 0;
    uint64_t *validation, *verification;
    unsigned int label_buf[14];

    // Allocate some memory for those things
    validation = calloc(sizeof(uint64_t), 46431 * 15);
    verification = calloc(sizeof(uint64_t), 46431 * 15);
    if (validation == NULL || verification == NULL) {
        fprintf(stderr, "Allocation error!\n");
        return 1;
    }

    // Open the database
    rc = sqlite3_open(db_location, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Instantiate the quadtree
    assert(!quadtree_init(&tree, 16777215, 16777215));

    // Select the data out of the database
    fprintf(stderr, "Creating quadtree...\n");
    rc = sqlite3_exec(db, select_query, create_tree_callback, tree, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    // Create the validation structure
    fprintf(stderr, "Creating validation structure...\n");
    rc = sqlite3_exec(db, select_query, create_validation_callback, validation, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    // Verify the result
    fprintf(stderr, "Verifying (stage 1)...\n");
    for (int i = 0; i < 46431; i++) {
        uint64_t *off = validation + (i * 15);
        uint64_t identifier = *off;
        unsigned int out = 0; 
        assert(!quadtree_scan_x(tree, identifier, label_buf, &out, 14));
        for (int j = 0; j < out; j++) {
            unsigned int label = label_buf[j];
            for (int k = 1; k < 15; k++) {
                if (*(off + k) == label) {
                    *(off + k) = 0;
                }
            }
        }
    }

    fprintf(stderr, "Verifying (stage 2)...\n");
    for (int i = 0; i < 46431; i++) {
        uint64_t *off = validation + (i * 15);
        uint64_t passed = 1;
        for (int j = 1; j < 15; j++) {
            uint64_t *suboff = off + j;
            if (*suboff) {
                passed = 0;
                fprintf(stderr, "%d/%d/%d/%d\n", *off, *suboff, i, j);
                assert(0);
            }
        }
        if (passed) {
            *off = 0;
        }
    }

    fprintf(stderr, "Verifying (stage 3)...\n");
    assert(!memcmp(validation, verification, 46431 * 15 * sizeof(uint64_t)));

    return 0;
}