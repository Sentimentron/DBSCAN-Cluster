#!/usr/bin/env python

import sqlite3
import subprocess
import multiprocessing.dummy as mp
import sys
import pdb

from datetime import datetime, timedelta

def process(timespec, db, result_queue):

    day, mindate, maxdate = timespec
    args = ["./date_cluster", "--db", db]
    args.append("--start")
    args.append(mindate.strftime("%Y-%m-%d %H:%M:%S")) 
    args.append("--end")
    args.append(maxdate.strftime("%Y-%m-%d %H:%M:%S"))
    print " ".join(args)
    pipe = subprocess.Popen(args, stdout=subprocess.PIPE)
    stdout, stderr = pipe.communicate() 
    return stdout 

def main():
    
    db = sys.argv[1] 
    conn = sqlite3.connect(db, detect_types=sqlite3.PARSE_DECLTYPES)
    cursor = conn.cursor()
    cursor.execute("SELECT MIN(date) FROM input")
    for mindate, in cursor.fetchall():
        mindate = datetime.strptime(mindate, "%Y-%m-%d %H:%M:%S")

    cursor.execute("SELECT MAX(date) FROM input")
    for maxdate, in cursor.fetchall():
        maxdate = datetime.strptime(maxdate, "%Y-%m-%d %H:%M:%S")

    days = (maxdate - mindate).days 
    t_queue = []
    for i in range(days+2):
        t_queue.append((i, mindate + timedelta(i), mindate + timedelta(i+1)))


    cursor.execute("DELETE FROM temporary_label_clusters")
    result_queue = mp.Queue() 
    pool = mp.Pool(1) 
    robj = pool.imap_unordered(lambda x: process(x, db, result_queue), t_queue)


    # TODO: write result processing 
    max_cluster_id = 0
    for x in robj:
        tmp_cluster_id = 0
        for line in x.split("\n"):
            line = line.strip()
            if len(line) == 0:
                continue
            identifier, _, cluster = line.partition(' ')
            sql = """INSERT INTO temporary_label_clusters VALUES (?, ?)"""
            identifier = int(identifier)
            cluster = int(cluster)
            if cluster > tmp_cluster_id:
                tmp_cluster_id = cluster
            if cluster == 0:
                continue
            cluster += max_cluster_id
            cursor.execute(sql, (identifier, cluster))
        conn.commit()
        max_cluster_id += tmp_cluster_id

if __name__ == "__main__":
    main()
