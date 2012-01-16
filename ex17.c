#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

//#define MAX_DATA 512
//#define MAX_ROWS 100

struct Address {
  int id;
  int set;
  char *name;
  char *email;
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



void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}


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


void Database_write(struct Connection *conn) {
  rewind(conn->file);
  int rc = 1;
  
  // Take a chunk of data the size of 1 int, from the address
  // 'conn->db->max_data' points to, and write it to 'conn->file'.
  rc = fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);

  // Repeat for 'max_rows'
  rc = fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);


  // Since 'rows' is stored in the struct as a pointer to an array of
  // undetermined length, we can't just dump it straight into a file, 
  // because all that will do is save the _address_ the pointer points
  // to. So instead, we have to save the each entry in it manually,
  // explicitly stating how big each one is along the way...

  // Calculate the size of each of the strings in the Address structs.
  int string_size = sizeof(char) * conn->db->max_data;

  int i = 0;
  for (i = 0; i < conn->db->max_rows; i++) {
      struct Address *addr = &conn->db->rows[i];

      // Same as with 'max_data' and 'max_rows', we're just taking an
      // 'int'-sized chunk of data from the address each member points
      // to, and saving it to the file.
      fwrite(&addr->id, sizeof(int), 1, conn->file);
      fwrite(&addr->set, sizeof(int), 1, conn->file);

      // Here however, since the string members are already pointers
      // to addresses, we don't need to get their addresses; all we
      // need is to know is how big they are (which we figured out
      // before entering the loop).
      fwrite(addr->name, string_size, 1, conn->file);
      rc = fwrite(addr->email, string_size, 1, conn->file);
  }

  if (rc != 1)
    die("Failed to write database.", conn);

  rc = fflush(conn->file);
  if (rc == -1)
    die("Could not flush database.", conn);
}


void Database_load(struct Connection *conn) {
  int rc = 1;
  
  // Notice that the following 'read' operations are performed in the same order
  // that their 'write' analogs were in 'Database_write'...

  // Read in data from 'conn->file' that's the size of 1 int,
  // and store it at the address 'conn->db->max_data' points to.
  rc = fread(&conn->db->max_data, sizeof(int), 1, conn->file);

  // Repeat for 'conn->db->max_rows'
  rc = fread(&conn->db->max_rows, sizeof(int), 1, conn->file); 

  if (rc != 1)
    die("Failed to load database.", conn);

  
  // Since we don't know what size the rows array might be,
  // we need to calculate the amount of memory it needs,
  // so that we can allocate it and store stuff in there.
  int string_size = sizeof(char) * conn->db->max_data;
  int address_size = (sizeof(int) * 2) + (string_size * 2);
  int rows_size = address_size * conn->db->max_rows;
  conn->db->rows = malloc(rows_size);
  

  // 'rows' is an array of arbirtrarily sized structs, so we can't
  // just read it in one big chunk. Instead, we have to break it down 
  // and manually populate it by reading and storing each of the
  // fields for each Address.

  int i = 0;
  for (i = 0; i < conn->db->max_rows; i++) {
      struct Address *addr = &conn->db->rows[i];

      // Since the first 2 members of 'Address' aren't pointers,
      // just read in the int data and store it in the same addresses
      // as the members themselves.
      fread(&addr->id, sizeof(int), 1, conn->file);
      fread(&addr->set, sizeof(int), 1, conn->file);


      // For the strings however, since they are pointers (to char arrays),
      // we need to first allocate the memory that they're going to be pointing to,
      // so that we can use it to store the strings from the file.
      addr->name = malloc(string_size);
      fread(addr->name, string_size, 1, conn->file);

      addr->email = malloc(string_size);
      rc = fread(addr->email, string_size, 1, conn->file);
  }

  if (rc != 1)
    die("Failed to load database.", conn);
}

void Database_create(struct Connection *conn) {
  int max_data = conn->db->max_data;
  int max_rows = conn->db->max_rows;
  
  // Calculate, and allocate the amount of memory needed for the rows array.
  int string_size = sizeof(char) * max_data;
  int address_size = (sizeof(int) * 2) + (string_size * 2);
  int rows_size = address_size * max_rows;
  conn->db->rows = malloc(rows_size);
  
  int i = 0;
  for (i = 0; i < max_rows; i++) {
    // make an Address struct prototype to initialize it
    struct Address addr = {.id = i, .set = 0};

    // and don't forget to allocate the memory for the strings!
    addr.name = malloc(sizeof(char) * max_data);
    addr.email = malloc(sizeof(char) * max_data);
    conn->db->rows[i] = addr;
  }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email) {

  int max_data = conn->db->max_data;
  struct Address *addr = &conn->db->rows[id];
  
  if (addr->set)
    die("Already set, delete it first.", conn);
  
  addr->set = 1;
  addr->name = calloc(max_data, sizeof(char));
  addr->email = calloc(max_data, sizeof(char));

  // Bug: strncpy doesn't guarantee a null-terminated string
  char *res = strncpy(addr->name, name, max_data);

  // Fix: append a null-terminator to the end of it
  addr->name[max_data-1] = '\0';

  if (!res)
    die("Name copy failed.", conn);

  res = strncpy(addr->email, email, max_data);
  addr->email[max_data-1] = '\0';
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

void Database_find(struct Connection *conn, char *term) {
    int i = 0;
    for (i = 0; i < conn->db->max_rows; i++) {
        struct Address *addr = &conn->db->rows[i];
        
        if (strcmp(addr->name, term) == 0 || strcmp(addr->email, term) == 0) {
            
            Address_print(addr);
            return;
        }
    }
    
    printf("No records matching '%s'\n", term);
}

void Database_delete(struct Connection *conn, int id) {
  struct Address addr = {.id = id, .set = 0};

  // Yep, need to allocate the memory for these strings here too;
  // just like we did when we first initialized all the Addresses.
  addr.name = malloc(sizeof(char) * conn->db->max_data);
  addr.email = malloc(sizeof(char) * conn->db->max_data);
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

    // We malloc'd a whole lot of strings up until now,
    // so that means we gotta 'free' em too.
    int i = 0;
    for (i = 0; i < conn->db->max_rows; i++) {
        free(conn->db->rows[i].name);
        free(conn->db->rows[i].email);
    }

    // Not to mention the actual array they were stored in.
    if (conn->db->rows) free(conn->db->rows);

    if (conn->db)       free(conn->db);
    if (conn->file)     fclose(conn->file);
    free(conn);
  }
}



int main(int argc, char *argv[]) {
  if (argc < 3)
    die("USAGE: ex17 <dbfile> <action> [action params]", NULL);

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  int id = 0;
  
  if (action != 'c' && action != 'f' && argc > 3) {
    id = atoi(argv[3]);

    if (id >= conn->db->max_rows)
      die("There are not that many records.", conn);
  }

  switch(action) {
    // Create 
    case 'c':
      if (argc != 5)
        die("MAX_DATA and MAX_ROWS required.", conn);
      
      conn->db->max_data = atoi(argv[3]);
      conn->db->max_rows = atoi(argv[4]);
      Database_create(conn);
      Database_write(conn);
      break;

    // Get
    case 'g':
      if (argc != 4)
        die("Need an ID to get.", conn);
      Database_get(conn, id);
      break;

    // Set
    case 's':
      if (argc != 6)
        die("Need ID, name, and email to set.", conn);
      Database_set(conn, id, argv[4], argv[5]);
      Database_write(conn);
      break;

    // Find
    case 'f':
      if (argc != 4)
        die("Need a name or email to search for.", conn);
      Database_find(conn, argv[3]);
      break;

    // Delete
    case 'd':
      if (argc != 4)
        die("Need ID to delete.", conn);
      Database_delete(conn, id);
      Database_write(conn);
      break;

    // List
    case 'l':
      Database_list(conn);
      break;

    default:
      die("Invalid action, only:  c=create,  g=get,  s=set,  d=delete,  l=list", conn);
  }

  Database_close(conn);
  return 0;
}

