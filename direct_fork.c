//
// Created by flassabe on 26/10/22.
//

#include "direct_fork.h"

#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "analysis.h"
#include "utility.h"

/*!
 * @brief direct_fork_directories runs the directory analysis with direct calls to fork
 * @param data_source the data source directory with 150 directories to analyze (parallelize with fork)
 * @param temp_files the path to the temporary files directory
 * @param nb_proc the maximum number of simultaneous processes
 */
void direct_fork_directories(char *data_source, char *temp_files, uint16_t nb_proc) {
    DIR *data_dir;
    struct dirent *entry;
    pid_t pid;
    uint16_t proc_count = 0;

    // 1. Check parameters
    if (data_source == NULL || temp_files == NULL || nb_proc == 0) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return;
    }

    // 2. Iterate over directories (ignore . and ..)
    data_dir = opendir(data_source);
    if (data_dir == NULL) {
        perror("Error opening data source directory");
        return;
    }

    while ((entry = readdir(data_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Ignore special directories
        }

        // 3. fork and start a task on current directory.
        pid = fork();
        if (pid == 0) {  // Child process
            // Perform analysis on the current directory
            char dir_path[256];
            snprintf(dir_path, sizeof(dir_path), "%s/%s", data_source, entry->d_name);
            printf("Analyzing directory: %s\n", dir_path);
            // TODO: Add code to perform actual analysis here
            return;
        } else if (pid > 0) {  // Parent process
            proc_count++;
            // 3 bis: if max processes count already run, wait for one to end before starting a task.
            if (proc_count >= nb_proc) {
                wait(NULL);  // Wait for a child process to end
                proc_count--;
            }
        } else {  // Error
            perror("Error creating new process");
            return;
        }
    }

    // 4. Cleanup
    closedir(data_dir);
}

/*!
 * @brief direct_fork_files runs the files analysis with direct calls to fork
 * @param data_source the data source containing the files
 * @param temp_files the temporary files to write the output (step2_output)
 * @param nb_proc the maximum number of simultaneous processes
 */
void direct_fork_files(char *data_source, char *temp_files, uint16_t nb_proc) {
    DIR *data_dir;
    struct dirent *entry;
    pid_t pid;
    uint16_t proc_count = 0;

    // 1. Check parameters
    if (data_source == NULL || temp_files == NULL || nb_proc == 0) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return;
    }

    // 2. Iterate over files in data_source
    data_dir = opendir(data_source);
    if (data_dir == NULL) {
        perror("Error opening data source directory");
        return;
    }

    while ((entry = readdir(data_dir)) != NULL) {
        if (entry->d_type != DT_REG) {  // Skip non-regular files
            continue;
        }

        // 3. fork and start a task on current file.
        pid = fork();
        if (pid == 0) {  // Child process
            // Perform analysis on the current file
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%s", data_source, entry->d_name);
            printf("Analyzing file: %s\n", file_path);
            // TODO: Add code to perform actual analysis here
            return;
        } else if (pid > 0) {  // Parent process
            proc_count++;
            // 3 bis: if max processes count already run, wait for one to end before starting a task.
            if (proc_count >= nb_proc) {
                wait(NULL);  // Wait for a child process to end
                proc_count--;
            }
        } else {  // Error
            perror("Error creating new process");
            return;
        }
    }

    // 4. Cleanup
    closedir(data_dir);
}
