#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  // For bool; in pure C23, you can use built-in bool without this header

#define MAX_TASKS 100
#define MAX_DESC 256
#define TODO_FILE ".todo.txt"  // In home dir; use getenv("HOME") to prepend path

typedef struct {
    bool done;
    char desc[MAX_DESC];
} Task;

int load_tasks(Task tasks[], FILE *file) {
    int count = 0;
    char line[MAX_DESC + 10];
    while (fgets(line, sizeof(line), file) && count < MAX_TASKS) {
        int status;
        if (sscanf(line, "%d|%[^\n]", &status, tasks[count].desc) == 2) {
            tasks[count].done = (status == 1);
            count++;
        }
    }
    return count;
}

void save_tasks(const Task tasks[], int count, FILE *file) {
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%d|%s\n", tasks[i].done ? 1 : 0, tasks[i].desc);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [args]\nCommands: add \"task\", list, done <index>, remove <index>\n", argv[0]);
        return 1;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", getenv("HOME"), TODO_FILE);

    Task tasks[MAX_TASKS];
    int task_count = 0;

    // Load existing tasks
    FILE *file = fopen(filepath, "r");
    if (file) {
        task_count = load_tasks(tasks, file);
        fclose(file);
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "add") == 0 && argc == 3) {
        if (task_count < MAX_TASKS) {
            tasks[task_count].done = false;
            strncpy(tasks[task_count].desc, argv[2], MAX_DESC - 1);
            task_count++;
        } else {
            printf("Task limit reached.\n");
        }
    } else if (strcmp(cmd, "list") == 0) {
        if (task_count == 0) {
            printf("No tasks to display.\n");
        } else {
            for (int i = 0; i < task_count; ++i) {
                printf("%d: [%s] %s\n", i + 1, tasks[i].done ? "x" : " ", tasks[i].desc);
            }
        }
        return 0;  // No save needed for list
    } else if (strcmp(cmd, "done") == 0 && argc == 3) {
        int idx = atoi(argv[2]) - 1;
        if (idx >= 0 && idx < task_count) {
            tasks[idx].done = true;
        } else {
            printf("Invalid index.\n");
        }
    } else if (strcmp(cmd, "remove") == 0 && argc == 3) {
        int idx = atoi(argv[2]) - 1;
        if (idx >= 0 && idx < task_count) {
            for (int j = idx; j < task_count - 1; ++j) {
                tasks[j] = tasks[j + 1];
            }
            task_count--;
        } else {
            printf("Invalid index.\n");
        }
    } else {
        printf("Unknown command.\n");
        return 1;
    }

    // Save changes
    file = fopen(filepath, "w");
    if (file) {
        save_tasks(tasks, task_count, file);
        fclose(file);
    } else {
        printf("Error saving file.\n");
    }

    return 0;
}
