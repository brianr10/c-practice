#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DATA_TYPE {
  int jobNumber;
  int jobType;
  int jobSize;
};

struct NODE_TYPE {
  struct DATA_TYPE data;
  struct NODE_TYPE *next;
};

const char *DATA_FILE = "jobs.data";

struct NODE_TYPE *load_data(const char *path);

void save_data(const char *path, struct NODE_TYPE *head);

struct NODE_TYPE *processChoice(int choice, struct NODE_TYPE *head);

void display_jobs(struct NODE_TYPE *head);
struct NODE_TYPE *add_job(struct NODE_TYPE *head);
struct NODE_TYPE *delete_job(struct NODE_TYPE *head);
void get_average_job_size(struct NODE_TYPE *head);
void search_jobs(struct NODE_TYPE *head);

int get_user_input(const char *prompt, const char *parseErrorMessage) {
  printf("%s", prompt);
  int value = 0;
  int result = scanf("%d", &value);
  printf("\n");

  if (result == EOF) {
    printf("End of user input stream\n");
    exit(1);
  } else if (result == 0) {
    printf("%s\n", parseErrorMessage);
    exit(1);
  }
  return value;
}

int main() {
  struct NODE_TYPE *head = load_data(DATA_FILE);

  while (1) {
    int choice = get_user_input(
        "Enter option: ", "Invlaid input! Choice must be in range 1..=6\n");
    printf("\n");

    if (choice < 1 || choice > 6) {
      printf("Invalid input! Choice must be in rang 0..=6\n");
    } else {
      head = processChoice(choice, head);
    }
  }
}

struct NODE_TYPE *processChoice(int choice, struct NODE_TYPE *head) {
  switch (choice) {
  case 1:
    display_jobs(head);
    return head;
  case 2:
    search_jobs(head);
    return head;
  case 3:
    return add_job(head);
  case 4:
    return delete_job(head);
  case 5:
    get_average_job_size(head);
    return head;
  case 6:
    save_data(DATA_FILE, head);
    while (head != NULL) {
      printf("Deleted element\n");
      struct NODE_TYPE *next = head->next;
      head->next = NULL;
      free(head);
      head = next;
    }
    exit(0);
  }
  return head;
}

struct NODE_TYPE *load_data(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    printf("Unable to open %s for reading. Creating empty list\n", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  void *buf = malloc(size);
  if (buf == NULL) {
    printf("Unable to allocate memory for readin input file\n");
    exit(1);
  }

  int elements = size / sizeof(struct DATA_TYPE);
  if (size % sizeof(struct DATA_TYPE) != 0) {
    printf("Input file corrupted\n");
    exit(1);
  }

  unsigned long elements_read =
      fread(buf, sizeof(struct DATA_TYPE), elements, file);
  if (elements_read != elements) {
    printf("Input file corruption\n");
    exit(1);
  }

  struct NODE_TYPE *head = NULL;
  struct NODE_TYPE *last = NULL;

  for (int i = 0; i < elements; i++) {
    const struct DATA_TYPE *src = &buf[i * sizeof(struct DATA_TYPE)];
    struct NODE_TYPE *new =
        (struct NODE_TYPE *)malloc(sizeof(struct NODE_TYPE));
    if (new == NULL) {
      printf("Unable to allocat memory for node\n");
      exit(1);
    }

    memcpy(&new->data, src, sizeof(struct DATA_TYPE));
    new->next = NULL;

    if (head == NULL) {
      head = new;
    }
    if (last != NULL) {
      last->next = new;
    }
    last = new;
  }
  fclose(file);
  return head;
}

void save_data(const char *path, struct NODE_TYPE *head) {
  FILE *file = fopen(path, "wb");
  if (file == NULL) {
    printf("Unable to open %s for writing\n", path);
    return;
  }
  while (head != NULL) {
    fwrite(&head->data, sizeof(struct DATA_TYPE), 1, file);
    head = head->next;
  }
  fclose(file);

  printf("Saved job data to %s successfully", path);
}

void display_jobs(struct NODE_TYPE *head) {
  if (head == NULL) {
    printf("No stored jobs\n");
    return;
  }
  int i = 0;
  while (head != NULL) {
    printf("Job #%d:\n", i + 1);
    printf("    Number: %d\n", head->data.jobNumber);
    printf("    Type: %d\n", head->data.jobType);
    printf("    Size: %d\n", head->data.jobSize);
    printf("\n");
    i++;
    head = head->next;
  }
}
void search_jobs(struct NODE_TYPE *head) {
  if (head == NULL) {
    printf("No stored jobs\n");
    return;
  }

  int desired_job_num = get_user_input("Enter job number to search for: ",
                                       "Please enter a integer job number");
  printf("\n");

  int i = 0;
  while (head != NULL) {
    if (head->data.jobNumber == desired_job_num) {
      printf("Found in list index: %d:\n", i + 1);
      printf("Number: %d\n", head->data.jobNumber);
      printf("Type: %d\n", head->data.jobType);
      printf("Size: %d\n", head->data.jobSize);
      return;
    }
    i++;
    head = head->next;
  }
  printf("Unable to find job #%d\n", desired_job_num);
}
struct NODE_TYPE *add_job(struct NODE_TYPE *head) {
  struct NODE_TYPE *new = (struct NODE_TYPE *)malloc(sizeof(struct NODE_TYPE));
  if (new == NULL) {
    printf("Unable to allocate memory for node\n");
    exit(1);
  }
  int jobNumber = get_user_input("Enter job number: ", "Enter an integer");
  int jobType = get_user_input("Enter job type: ", "Enter an integer");
  int jobSize = get_user_input("Enter job size: ", "Enter an integer");

  new->data.jobNumber = jobNumber;
  new->data.jobType = jobType;
  new->data.jobSize = jobSize;
  new->next = NULL;

  if (head == NULL) {
    head = new;
  } else {
    struct NODE_TYPE *tmp = head;
    while (tmp != NULL) {
      if (tmp->next == NULL) {
        tmp->next = new;
        break;
      }
      tmp = tmp->next;
    }
  }
  return head;
}
struct NODE_TYPE *delete_job(struct NODE_TYPE *head) {
  int desired_job_num = get_user_input("Enter job number to search for: ",
                                       "Please enter a integer job number.");

  struct NODE_TYPE *tmp = head;
  struct NODE_TYPE *prev = NULL;
  while (tmp != NULL) {
    if (tmp->data.jobNumber == desired_job_num) {
      struct NODE_TYPE *next = tmp->next;
      if (prev == NULL) {
        printf("Deleting first element...\n");
        head->next = NULL;
        free(head);
        return next;
      } else {
        printf("Deleting non first element...\n");
        free(tmp);
        prev->next = next;
      }
      break;
    }
    prev = tmp;
    tmp = tmp->next;
  }
  return head;
}

void get_average_job_size(struct NODE_TYPE *head) {
  if (head == NULL) {
    printf("No stored jobs\n");
    return;
  }

  int total_job_size = 0;
  int i = 0;
  while (head != NULL) {
    total_job_size += head->data.jobSize;
    i++;
    head = head->next;
  }
  float avg = (float)total_job_size / (float)i;
  printf("Average job size: %f\n", avg);
}
