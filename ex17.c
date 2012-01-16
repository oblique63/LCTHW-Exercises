#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address {
  int id;
  int set;
  char name[MAX_DATA];
  char email[MAX_DATA];
};

struct Database {
  int max_data;
  int max_rows;
  struct Address *rows;
};

struct Connection {
  FILE *file;
  struct Database *db;
};


void Database_close(struct Connection *conn);

void die(const char *message, struct Connection *conn) {
  if (errno) {
    perror(message);
  }
  else {
    printf("ERROR: %s\n", message);
  }
  
  Database_close(conn);
  exit(1);
}


void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
  int rc = 1;
  
  rc = fread(&conn->db->max_data, sizeof(int), 1, conn->file);
  rc = fread(&conn->db->max_rows, sizeof(int), 1, conn->file); 

  int rows_size = sizeof(struct Address) * conn->db->max_rows;
  conn->db->rows = malloc(rows_size);
  rc = fread(conn->db->rows, rows_size, 1, conn->file);

  if (rc != 1)
    die("Failed to load database.", conn);
}

void Database_write(struct Connection *conn) {
  rewind(conn->file);
  int rc = 1;
  
  rc = fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);
  rc = fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);

  int rows_size = sizeof(struct Address) * conn->db->max_rows;
  rc = fwrite(conn->db->rows, rows_size, 1, conn->file);

  if (rc != 1)
    die("Failed to write database.", conn);

  rc = fflush(conn->file);
  if (rc == -1)
    die("Could not flush database.", conn);
}


struct Connection* Database_open(const char *filename, char mode) {
  struct Connection *conn = malloc(sizeof(struct Connection));
  if (!conn)
    die("Memory error.", conn);
  
  conn->db = malloc(sizeof(struct Database));

  if (!conn->db)
    die("Memory error", conn);

  if (mode == 'c') {
    conn->file = fopen(filename, "w");
  }
  else {
    conn->file = fopen(filename, "r+");

    if (conn->file)
      Database_load(conn);
  }
  
  if (!conn->file)
    die("Failed to open file.", conn);
  
  return conn;
}

void Database_close(struct Connection *conn) {
  if (conn) {
    if (conn->file)     fclose(conn->file);
    if (conn->db->rows) free(conn->db->rows);
    if (conn->db)       free(conn->db);
    free(conn);
  }
}

void Database_create(struct Connection *conn) {
  int max_data = conn->db->max_data;
  int max_rows = conn->db->max_rows;
  int rows_size = sizeof(struct Address) * max_rows;
  conn->db->rows = malloc(rows_size);
  
  int i = 0;
  for (i = 0; i < max_rows; i++) {
    // make a prototype to initialize it
    struct Address addr = {.id = i, .set = 0};
    //struct Address *addr = malloc(sizeof(struct Address) + (sizeof(char) * max_data * 2));
    //struct Address *addr = malloc(sizeof(struct Address));
    //addr->id = i;
    //addr->set = 0;
    
    conn->db->rows[i] = addr;
  }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email) {
  //int max_data = conn->db->max_data;

  struct Address *addr = &conn->db->rows[id];
  
  if (addr->set)
    die("Already set, delete it first.", conn);
  
  addr->set = 1;
  //addr->name = malloc(sizeof(name));
  //addr->email = malloc(sizeof(email));

  // Bug: strncpy doesn't guarantee a null-terminated string
  char *res = strncpy(addr->name, name, conn->db->max_data);

  // Fix: append a null-terminator to it
  res[strlen(name)] = '\0';
  if (!res)
    die("Name copy failed.", conn);

  res = strncpy(addr->email, email, conn->db->max_data);
  res[strlen(email)] = '\0';
  if (!res)
    die("Email copy failed.", conn);

}

void Database_get(struct Connection *conn, int id) {
  struct Address *addr = &conn->db->rows[id];
  if (addr->set)
    Address_print(addr);
  else
    die("ID is not set.", conn);
}


void Database_delete(struct Connection *conn, int id) {
  struct Address addr = {.id = id, .set = 0};
  conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn) {
  struct Database *db = conn->db;
  
  printf("MAX_DATA: %d\n", db->max_data);
  printf("MAX_ROWS: %d\n", db->max_rows);

  int i = 0;
  for (i = 0; i < db->max_rows; i++) {
    struct Address *cur = &db->rows[i];
    if (cur->set)
      Address_print(cur);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    die("USAGE: ex17 <dbfile> <action> [action params]", NULL);

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  int id = 0;
  
  if (action != 'c' && argc > 3) {
    id = atoi(argv[3]);

    if (id >= conn->db->max_rows)
      die("There are not that many records.", conn);
  }

  switch(action) {
    case 'c':
      if (argc != 5)
        die("MAX_DATA and MAX_ROWS required.", conn);
      
      conn->db->max_data = atoi(argv[3]);
      conn->db->max_rows = atoi(argv[4]);
      Database_create(conn);
      Database_write(conn);
      break;

    case 'g':
      if (argc != 4)
        die("Need an ID to get.", conn);
      Database_get(conn, id);
      break;
    
    case 's':
      if (argc != 6)
        die("Need ID, name, and email to set.", conn);
      Database_set(conn, id, argv[4], argv[5]);
      Database_write(conn);
      break;

    case 'd':
      if (argc != 4)
        die("Need ID to delete.", conn);
      Database_delete(conn, id);
      Database_write(conn);
      break;

    case 'l':
      Database_list(conn);
      break;

    default:
      die("Invalid action, only:  c=create,  g=get,  s=set,  d=delete,  l=list", conn);
  }

  Database_close(conn);
  return 0;
}

