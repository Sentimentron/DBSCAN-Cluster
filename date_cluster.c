#include <assert.h>
#include <stdint.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dbscan.h"
#include "quadtree.h"

void sqlite3_assert_msg(int rc, const char *msg) {
    if(!rc) return;
    fprintf(stderr, "Database error: %s:\n", msg);
    exit(1);
}

void sqlite3_assert_db(int rc, sqlite3 *db) {
    if(!rc) return;
    sqlite3_assert_msg(rc, sqlite3_errmsg(db));
}

void print_usage() {
    fprintf(stderr, "Usage: date_cluster --db /path/to/db --start start_date --end end_date\n");
    exit(1);
}

int main(int argc, char **argv) {

    int insert_new;

    char *db_location = NULL;
    char *db_start_date = NULL;
    char *db_end_date = NULL;
    
    uint64_t ofti_max, ofti_cur; 
    uint64_t *offset_to_identifier_map = NULL;
    QUADTREE *cluster_tree = NULL; 

    int rc, i;
    char *db_err_str = NULL;
    unsigned int *clusters;
    sqlite3_stmt *select;
    sqlite3 *db = NULL; 

    for (i = 0; i < argc-1; i++) {
        if (!strcmp("--db", argv[i])) db_location = argv[i+1];
        if (!strcmp("--start", argv[i])) db_start_date = argv[i+1];
        if (!strcmp("--end", argv[i])) db_end_date = argv[i+1];
    }

    if(db_location == NULL) print_usage();
    if(db_start_date == NULL) print_usage();
    if(db_end_date == NULL) print_usage();
    
    rc = sqlite3_open(db_location, &db);
    sqlite3_assert_db(rc, db);

    rc = sqlite3_prepare_v2(db, "SELECT DISTINCT document_identifier, label FROM temporary_label_clustering WHERE document_identifier IN (SELECT identifier FROM input WHERE date > ? AND date <= ?) ORDER BY document_identifier ASC", -1 , &select, NULL);
    sqlite3_assert_db(rc, db); 

    // Bind select parameters
    rc = sqlite3_bind_text(select,
        1, 
        db_start_date, 
        strlen(db_start_date),
        SQLITE_STATIC
    );
    sqlite3_assert_db(rc, db); 
    rc = sqlite3_bind_text(
        select, 
        2, 
        db_end_date, 
        strlen(db_end_date),
        SQLITE_STATIC
    );
    sqlite3_assert_db(rc, db);

    // Allocate space for the offset to identifier map 
    ofti_max = 64;
    ofti_cur = 0;
    offset_to_identifier_map = malloc(sizeof(uint64_t) * ofti_max);
    if (offset_to_identifier_map == NULL) {
        fprintf(stderr, "Allocation error!\n");
        return 1;
    }

    assert(!quadtree_init(&cluster_tree, 4194303, 4194303));
    insert_new = 1;
    while(sqlite3_step(select) == SQLITE_ROW) {
        // Insert new identifier into offset/identifier mapping 
        sqlite3_int64 new_identifier = sqlite3_column_int64(select, 0);
        int label = sqlite3_column_int(select, 1);
        int insert_offset;
        if (ofti_cur > 0) {
            uint64_t candidate_id = offset_to_identifier_map[ofti_cur-1]; 
            insert_new = (candidate_id != new_identifier);
        }
        if (insert_new) {
            if (ofti_cur >= ofti_max-1) {
                // Reallocate 
                ofti_max += 64;
                offset_to_identifier_map = realloc(offset_to_identifier_map, sizeof(uint64_t) * ofti_max);
                if (offset_to_identifier_map == NULL) { 
                    fprintf(stderr, "Reallocation failure!\n");
                    exit(1);
                }
            }
            offset_to_identifier_map[ofti_cur] = new_identifier;
            insert_offset = ofti_cur;
        }
        else {
            insert_offset = ofti_cur-1;
        }
//        assert(quadtree_insert(cluster_tree, new_identifier, label));

        fprintf(stderr, "%ld %ld %ld\n", insert_offset, new_identifier, label); 
        assert(quadtree_insert(cluster_tree, insert_offset, label));
        if (insert_new) ofti_cur++;
    }

    if (ofti_cur == 0) return 1;
    if (insert_new) ofti_cur--;
    fprintf(stderr, "%u document identifiers read...\n", ofti_cur);

    fprintf(stderr, "Clustering...\n");
    clusters = calloc(sizeof(unsigned int), ofti_cur); 
    if(clusters == NULL) {
        fprintf(stderr, "Allocation failure!\n");
        return 1; 
    }
    assert(!DBSCAN(cluster_tree, clusters, ofti_cur, 0.20, 2, &neighbours_search));
    for (uint64_t i = 0; i < ofti_cur; i++) {
        unsigned int identifier = offset_to_identifier_map[i];
        unsigned int cluster = clusters[i];
        printf("%u %u\n", identifier, cluster);
    }
    sqlite3_finalize(select);
    sqlite3_close(db);
}
