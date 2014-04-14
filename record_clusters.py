#!/usr/bin/env python

import sqlite3
import subprocess
import multiprocessing.dummy as mp
import sys
import pdb

from datetime import datetime, timedelta

def main():
    
    db = sys.argv[1]
    conn = sqlite3.connect(db, detect_types=sqlite3.PARSE_DECLTYPES)
    cursor = conn.cursor()
    output_file = open('output.tmp','r')

    cursor.execute("DELETE FROM temporary_label_clusters")

    for line in output_file:
        line = line.strip()
        if len(line) == 0:
            continue
        identifier, _, cluster = line.partition(' ')
        identifier = int(identifier)
        cluster = int(cluster)
        cursor.execute("INSERT INTO temporary_label_clusters VALUES (?, ?)", (identifier, cluster))

    conn.commit()

if __name__ == "__main__":
    main()
