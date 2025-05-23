#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "parser.h"
#include "src/client/api.h"
#include "src/common/constants.h"
#include "src/common/io.h"

pthread_t notif_thread;
int notif_pipe;

void *notification_handler(void *arg) { //Lopes
  char buffer[2 * MAX_STRING_SIZE + 3]; // 2 * 40 + 2 for parentheses and comma + 1 for null terminator
  while (1) {
    ssize_t bytes_read = read(notif_pipe, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("%s\n", buffer);
    } else if (bytes_read == -1 && errno != EAGAIN) {
      perror("Failed to read from notification pipe");
      break;
    }
    usleep(100000); // Sleep for 100ms to avoid busy-waiting
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <client_unique_id> <register_pipe_path>\n",
            argv[0]);
    return 1;
  }

  char req_pipe_path[256] = "/tmp/req";
  char resp_pipe_path[256] = "/tmp/resp";
  char notif_pipe_path[256] = "/tmp/notif";

  char keys[MAX_NUMBER_SUB][MAX_STRING_SIZE] = {0};
  unsigned int delay_ms;
  size_t num;

  strncat(req_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
  strncat(resp_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));
  strncat(notif_pipe_path, argv[1], strlen(argv[1]) * sizeof(char));

  // Open pipes - Lopes
  if (kvs_connect(req_pipe_path, resp_pipe_path, argv[2], notif_pipe_path, &notif_pipe) != 0) {
    fprintf(stderr, "Failed to connect to the server\n");
    return 1;
  }

  // Start notifications thread
  if (pthread_create(&notif_thread, NULL, notification_handler, NULL) != 0) {
    perror("Failed to create notification thread");
    return 1;
  }

  while (1) {
    switch (get_next(STDIN_FILENO)) {
    case CMD_DISCONNECT:
      if (kvs_disconnect() != 0) {
        fprintf(stderr, "Failed to disconnect to the server\n");
        return 1;
      }
      //End notifications thread
      pthread_cancel(notif_thread); //Lopes
      pthread_join(notif_thread, NULL); //Lopes
      printf("Disconnected from server\n");
      return 0;

    case CMD_SUBSCRIBE:
      num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
      if (num == 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }

      if (kvs_subscribe(keys[0])) {
        fprintf(stderr, "Command subscribe failed\n");
      }

      break;

    case CMD_UNSUBSCRIBE:
      num = parse_list(STDIN_FILENO, keys, 1, MAX_STRING_SIZE);
      if (num == 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }

      if (kvs_unsubscribe(keys[0])) {
        fprintf(stderr, "Command subscribe failed\n");
      }

      break;

    case CMD_DELAY:
      if (parse_delay(STDIN_FILENO, &delay_ms) == -1) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        continue;
      }

      if (delay_ms > 0) {
        printf("Waiting...\n");
        delay(delay_ms);
      }
      break;

    case CMD_INVALID:
      fprintf(stderr, "Invalid command. See HELP for usage\n");
      break;

    case CMD_EMPTY:
      break;

    case EOC:
      // Lopes
      // input should end in a disconnect, or it will loop here forever
      if (kvs_disconnect() != 0) {
        fprintf(stderr, "Failed to disconnect to the server\n");
        return 1;
      }
      pthread_cancel(notif_thread);
      pthread_join(notif_thread, NULL);
      break;
    }
  }
  return 0;
}
