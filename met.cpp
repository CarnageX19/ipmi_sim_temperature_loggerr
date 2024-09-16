#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#define DEFAULT_PORT "9001"
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_SENSOR "MBTemp"
#define DEFAULT_ITERATIONS 5

void log_temperature(FILE *output_file, const char *port, const char *host, const char *sensor) {
    char buffer[128];
    FILE *pipe;
    time_t current_time;
    struct tm *time_info;
    char time_str[20];
    char command[256];

    snprintf(command, sizeof(command), "ipmitool -I lanplus -U ipmiusr -P test -p %s -H %s -C 0 sdr list | grep -i '%s'", port, host, sensor);

    pipe = popen(command, "r");
    if (pipe == NULL) {
        fprintf(stderr, "Failed to run command\n");
        return;
    }
    
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

    if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        char *temp_str = strstr(buffer, "|");
        if (temp_str) {
            temp_str += 1; 
            char *temp_end = strstr(temp_str, " degrees");
            if (temp_end) {
                *temp_end = '\0'; 
            }
            fprintf(output_file, "%s - Temperature: %s Degrees C\n", time_str, temp_str);
            printf("Logging Temperature: %s degrees C for sensor: %s \n",temp_str,sensor);
            fflush(output_file);  // Clear file buffer
        }
    }
    
    pclose(pipe);
}

int main(int argc, char *argv[]) {
    const char *port = DEFAULT_PORT;
    const char *host = DEFAULT_HOST;
    const char *sensor = DEFAULT_SENSOR;
    int iterations = DEFAULT_ITERATIONS;  

    int opt;
    while ((opt = getopt(argc, argv, "p:H:s:i:")) != -1) {
        switch (opt) {
            case 'p':
                port = optarg;
                break;
            case 'H':
                host = optarg;
                break;
            case 's':
                sensor = optarg;
                break;
            case 'i':
                iterations = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port] [-H host] [-s sensor] [-i iterations]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    FILE *output_file = fopen("met.txt", "a");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    int count = 0;
    iterations = (iterations==DEFAULT_ITERATIONS)?DEFAULT_ITERATIONS:iterations;

    while (count < iterations) {
        log_temperature(output_file, port, host, sensor);
        sleep(1);  
        count++;
    }

    fclose(output_file);
    return 0;
}
