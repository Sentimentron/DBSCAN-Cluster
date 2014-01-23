#include <assert.h>
#include <stdint.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "quadtree.h"

const unsigned int DOCUMENT_IDENTIFIER_COUNT = 1021206;
const unsigned int MAX_LABEL_COUNT = 13;
const unsigned int MAX_DOCUMENT_IDENTIFIER = 6741414;

struct verification_t {
    uint64_t *arr;
    uint64_t last_identifier;
    uint64_t last_offset;
    uint64_t last_label_offset;
};

unsigned int nextpow2(unsigned int x) {
    unsigned int ret;
    for (ret = 1; ret < x; ret *= 2);
    return ret;
}

int create_tree_callback(void *arg, int argc, char **argv, char **col) {
    uint64_t identifier, label;
    QUADTREE *tree = (QUADTREE *)arg;

    identifier = strtoul(argv[0], NULL, 10);
    label = strtoul(argv[1], NULL, 10);

    assert(quadtree_insert(tree, identifier, label));
    return 0;
}

int create_validation_callback(void *arg, int argc, char **argv, char **col) {
    uint64_t identifier, label, i, j, off;
    struct verification_t *val = (struct verification_t *)arg;

    identifier = strtoul(argv[0], NULL, 10);
    label = strtoul(argv[1], NULL, 10);

    if (val->last_offset && (val->last_identifier == identifier)) {
        off  = ((val->last_offset-1) * (MAX_LABEL_COUNT + 1));
        off += val->last_label_offset;
        val->last_label_offset++;
    }
    else {
        off = (val->last_offset * (MAX_LABEL_COUNT + 1));
        *(val->arr + off) = identifier;
        val->last_label_offset = 1;
        val->last_identifier = identifier;
        off++;
        val->last_offset++;
    }

    *(val->arr + off) = label;
    return 0;

}

int main(int argc, char **argv) {
    const char * const db_location = "cluster.sqlite";
    const char * const select_query = "SELECT document_identifier, label FROM temporary_label_clustering ORDER BY document_identifier";
    QUADTREE *tree = NULL;
    sqlite3 *db = NULL;
    char *zErrMsg = NULL;
    int rc = 0;
    uint64_t *validation, *verification;
    unsigned int label_buf[MAX_LABEL_COUNT];
    struct verification_t v;

    // Allocate some memory for those things
    validation = calloc(sizeof(uint64_t), DOCUMENT_IDENTIFIER_COUNT * (MAX_LABEL_COUNT + 1));
    verification = calloc(sizeof(uint64_t), DOCUMENT_IDENTIFIER_COUNT * (MAX_LABEL_COUNT + 1));
    if (validation == NULL || verification == NULL) {
        fprintf(stderr, "Allocation error!\n");
        return 1;
    }

    v.arr = validation;
    v.last_offset = 0;

    // Open the database
    rc = sqlite3_open(db_location, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Instantiate the quadtree
    assert(!quadtree_init(&tree, nextpow2(MAX_DOCUMENT_IDENTIFIER)-1, nextpow2(MAX_DOCUMENT_IDENTIFIER)-1));

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
    rc = sqlite3_exec(db, select_query, create_validation_callback, &v, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return 1;
    }

    // Verify the result
    fprintf(stderr, "Verifying (stage 1)...\n");
    for (int i = 0; i < DOCUMENT_IDENTIFIER_COUNT; i++) {
        uint64_t *off = validation + (i * (MAX_LABEL_COUNT + 1));
        uint64_t identifier = *off;
        unsigned int out = 0;
        assert(!quadtree_scan_x(tree, identifier, label_buf, &out, MAX_LABEL_COUNT));
        for (int j = 0; j < out; j++) {
            unsigned int label = label_buf[j];
            for (int k = 1; k < MAX_LABEL_COUNT; k++) {
                if (*(off + k) == label) {
                    *(off + k) = 0;
                }
            }
        }
    }

    fprintf(stderr, "Verifying (stage 2)...\n");
    for (int i = 0; i < DOCUMENT_IDENTIFIER_COUNT; i++) {
        uint64_t *off = validation + (i * (MAX_LABEL_COUNT + 1));
        uint64_t passed = 1;
        for (int j = 1; j < MAX_LABEL_COUNT; j++) {
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
    assert(!memcmp(validation, verification, DOCUMENT_IDENTIFIER_COUNT * (MAX_LABEL_COUNT + 1) * sizeof(uint64_t)));

    return 0;
}
