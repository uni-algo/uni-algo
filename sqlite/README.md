DRAFT

To compile dynamic extension copy sqlite3.h, sqlite3ext.h from SQLite amalgamation to this folder and run:

gcc -shared -fPIC -o unisqlite.so -O3 -s -DUNI_SQLITE_DYNAMIC_EXTENSION unisqlite.c