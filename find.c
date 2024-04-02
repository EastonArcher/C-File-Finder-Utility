//import all relevant libraries
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

//define a maximum path size; 4096 is a common default value
#ifndef PATH_MAX
#define PATH_MAX 4096 
#endif

char * full_path;

// Function to perform specified action (e.g., delete) on a file
void perform_action(const char *path, const char *action, FILE *outputFile) {
    // Check if action is provided and it is 'delete'
    if (!action || strcmp(action, "delete") != 0) {
        // Print an error message for unsupported actions
        fprintf(stderr, "Unsupported action: %s\n", action ? action : "NULL");
        return;
    }

    // Attempt to remove the file at the given path
    if (remove(path) == 0) {
        // If removal is successful, inform the user
        fprintf(outputFile, "Deleted: %s\n", path);
    } else {
        // If removal is unsuccessful, print an error message
        perror("Error deleting file");
    }
}

// Recursive function to search for files in a directory and its subdirectories
void read_sub(char *sub_dir, char *name, char *mmin, char *inum, char *action, FILE *outputFile) {
    // Open the directory specified by sub_dir
    DIR *sub_dp = opendir(sub_dir);
    struct dirent *sub_dirp;
    struct stat filestat;

    // Check if the directory is successfully opened
    if (sub_dp != NULL) {
        // Iterate through each entry in the directory
        while ((sub_dirp = readdir(sub_dp)) != NULL) {
            // Skip over '.' and '..' entries
            if (strcmp(sub_dirp->d_name, ".") == 0 || strcmp(sub_dirp->d_name, "..") == 0) {
                continue;
            }

            // Create the full path of the current entry
            char temp_full_path[PATH_MAX];
            snprintf(temp_full_path, sizeof(temp_full_path), "%s/%s", sub_dir, sub_dirp->d_name);

            // Retrieve file information using stat
            if (stat(temp_full_path, &filestat) == -1) {
                // If stat fails, skip to the next entry
                continue;
            }

            // Check if the entry matches the specified name
            if (name && strcmp(sub_dirp->d_name, name) == 0) {
                fprintf(outputFile, "%s\n", temp_full_path);
                perform_action(temp_full_path, action, outputFile);
                continue;
            }

            // Check if mmin is specified
            if (mmin) {
                time_t current_time = time(NULL);
                char *endptr;
                long mmin_val = strtol(mmin, &endptr, 10);
                long diff_minutes = (current_time - filestat.st_mtime) / 60;

                // Check if the file modification time satisfies the mmin criteria
                if ((*mmin == '+' && diff_minutes > mmin_val) ||
                    (*mmin == '-' && diff_minutes < -mmin_val) ||
                    (*mmin != '+' && *mmin != '-' && diff_minutes == mmin_val)) {
                    fprintf(outputFile, "%s\n", temp_full_path);
                    perform_action(temp_full_path, action, outputFile);
                }
            }

            // Check if inum is specified
            if (inum) {
                ino_t inum_val = (ino_t)strtoul(inum, NULL, 10);
                // Check if the file inode number matches the specified inum
                if (filestat.st_ino == inum_val) {
                    fprintf(outputFile, "%s\n", temp_full_path);
                    perform_action(temp_full_path, action, outputFile);
                    continue;
                }
            }

            // Check if no filtering criteria are specified
            if (name == NULL && mmin == NULL && inum == NULL) {
                fprintf(outputFile, "%s\n", temp_full_path);
            }

            // Recursively process subdirectories
            if (S_ISDIR(filestat.st_mode)) {
                read_sub(temp_full_path, name, mmin, inum, action, outputFile);
            }
        }

        // Close the directory
        closedir(sub_dp);
    } else {
        // If the directory cannot be opened, print an error and exit
        perror("Cannot open directory");
        exit(2);
    }
}

// Main function to handle command line arguments and perform file searching
int main(int argc, char **argv) {
    FILE *outputFile;

    // Check if the output file "output.txt" already exists
    if (access("output.txt", F_OK) != -1) {
        outputFile = fopen("output.txt", "a"); // Open existing file for appending
    } else {
        outputFile = fopen("output.txt", "w"); // Create a new file for writing
    }

    // Check if the output file is successfully opened
    if (outputFile == NULL) {
        perror("Error opening output file");
        return 1;
    }

    // Initialize variables to track command line arguments and their values
    int w = 0, n = 0, m = 0, i = 0, a = 0;
    char *where = NULL, *name = NULL, *mmin = NULL, *inum = NULL, *action = NULL;

    char c; // Initialize a character to store each command line option
    while ((c = getopt(argc, argv, "w:n:m:i:a:")) != -1) {
        switch (c) {
            case 'w': //Directory
                w = 1;
                where = optarg;
                fprintf(outputFile, "where: %s\n", optarg);
                break;
            case 'n': //Name
                n = 1;
                name = optarg;
                fprintf(outputFile, "name: %s\n", optarg);
                break;
            case 'm': //Modified Minutes
                m = 1;
                mmin = optarg;
                fprintf(outputFile, "mmin: %s\n", optarg);
                break;
            case 'i': 
                i = 1;
                inum = optarg;
                fprintf(outputFile, "inum: %s\n", optarg);
                break;
            case 'a': 
                a = 1;
                action = optarg;
                fprintf(outputFile, "action: %s\n", optarg);
                break;
            case '?': // To handle unknown or unsupported options
            default:
                fprintf(stderr, "Usage: %s [-w directory] [-n name] [-m mmin] [-i inum] [-a action]\n", argv[0]);
                return 1; // Return with error status code
        }
    }

    // Check if a directory name was provided
    if (w) {
        read_sub(where, name, mmin, inum, action, outputFile); // Function call to mimic the find utility
    } else {
        fprintf(outputFile, "No directory given or directory does not exist.\n");
    }

    // Process any remaining command-line arguments
    argc -= optind;
    argv += optind;

    if (argc > 0) {
        fprintf(outputFile, "There are %d command-line arguments left to process:\n", argc);
        for (i = 0; i < argc; i++) {
            fprintf(outputFile, "Argument %d: '%s'\n", i + 1, argv[i]);
        }
    }

    // Close the output file
    fclose(outputFile);

    return 0;
}

