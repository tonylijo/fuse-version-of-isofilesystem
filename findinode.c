#include<stdio.h>
#include<string.h>
#include"iso_fs.h"
#include"globals.h"
#include"prototypes.h"
char *negative_value = "no such path";
guint local_g_strv_length (gchar **str_array) {
    guint i = 0;
    g_return_val_if_fail (str_array != NULL, 0);
    while (str_array[i])
    ++i;
    return i;
}
struct iso_inode *isofs_lookup(const char *path) {
    if(path[0] == '\0') {
        return NULL;
    }
    struct iso_inode *inode = g_hash_table_lookup(lookup_table, path);
    if(inode) {
        return inode;
    }
    if(g_hash_table_lookup(negative_lookup_table, path)) {
        return NULL;
    }
    gchar **parts = g_strsplit(path, "/", -1);
    guint parts_len = local_g_strv_length(parts);
    int partno = 1;
    gchar *rpath = g_strdup("/");
    gchar *rpath1 = "";
    gchar *part = parts[partno];
    while(part && partno < parts_len) {
        rpath1 = g_strconcat(rpath1, "/", part, NULL);
        inode = g_hash_table_lookup(lookup_table, rpath1);
        if(!inode) {
            int rc = isofs_readdir(rpath, NULL, NULL);
            if(rc) {
                fprintf(stderr, "lookup: error %d from readdir: %s\n", rc, strerror(-rc));
                g_strfreev(parts);
                g_free(rpath);
                return NULL;
            }
        }
        partno++;
        part = parts[partno];
        g_free(rpath);
        rpath = rpath1;
    }
    g_strfreev(parts);
    g_free(rpath);
    inode = g_hash_table_lookup(lookup_table, path);
    if(!inode) {
        g_hash_table_insert(negative_lookup_table, g_strdup(path), negative_value);
    }
    return inode;
}
