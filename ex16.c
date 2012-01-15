#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct Person {
  char *name;
  int age;
  int height;
  int weight;
};

// function that returns a Person struct
struct Person *Person_pointer_create(char *name, int age, int height, int weight) {
  // allocate the memory needed for a Person struct
  struct Person *who = malloc(sizeof(struct Person));
  
  // ensure that the memory for 'who' has been allocated
  assert(who != NULL);

  // allocate memory for a copy of the 'name' string, then duplicate it
  who->name = strdup(name);
  
  // fill out the rest of the Person's properties
  who->age = age;
  who->height = height;
  who->weight = weight;
  
  return who;
}

struct Person Person_create(char *name, int age, int height, int weight) {
  struct Person who;
  
  who.name = name;
  who.age = age;
  who.height = height;
  who.weight = weight;
  
  return who;
}

void Person_pointer_destroy(struct Person *who) {
  assert(who != NULL);

  // free the memory that was allocated to the duplicated name string,
  // and to the Person struct
  free(who->name);
  free(who);
}

void Person_pointer_print(struct Person *who) {
  printf("Name: %s\n ", who->name);
  printf("\tAge: %d\n", who->age);
  printf("\tHeight: %d\n", who->height);
  printf("\tWeight: %d\n", who->weight);
}

void Person_print(struct Person who) {
  printf("Name: %s\n ", who.name);
  printf("\tAge: %d\n", who.age);
  printf("\tHeight: %d\n", who.height);
  printf("\tWeight: %d\n", who.weight);
}


int main(int argc, char *argv[]) {
  // create a couple Person structs
  struct Person joe = Person_create("Joe Alex", 32, 64, 140);
  struct Person *frank = Person_pointer_create("Frank Blank", 20, 72, 180);
  
  //printf("Joe is at memory location: %p\n", joe);
  Person_print(joe);
  
  printf("Frank is at memory location: %p\n", frank);
  Person_pointer_print(frank);

  
  // time warp
  joe.age += 20;
  joe.height -= 2;
  joe.weight += 40;
  Person_print(joe);

  frank->age += 20;
  frank->weight += 20;
  Person_pointer_print(frank);

  
  // free up the memory allocated to the structs
  Person_pointer_destroy(frank);
  
  return 0;
}
