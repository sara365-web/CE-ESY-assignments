#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Circular buffer structure
typedef struct {
    char *data;      // array to store characters
    int size;        // maximum capacity
    int head;        // read pointer
    int tail;        // write pointer
    int count;       // current number of elements
} CircularBuffer;

// Initialize buffer: allocate memory and set initial values
CircularBuffer* circ_init(int size) {
    CircularBuffer *cb = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    if (!cb) return NULL;
    cb->data = (char*)malloc(sizeof(char) * size);
    if (!cb->data) {
        free(cb);
        return NULL;
    }
    cb->size = size;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    return cb;
}

// Free allocated memory
void circ_free(CircularBuffer *cb) {
    if (cb) {
        free(cb->data);
        free(cb);
    }
}

// Check if buffer is full
int circ_is_full(CircularBuffer *cb) {
    return cb->count == cb->size;
}

// Check if buffer is empty
int circ_is_empty(CircularBuffer *cb) {
    return cb->count == 0;
}

// Write a character into the buffer (returns 1 on success, 0 on overflow)
int circ_write(CircularBuffer *cb, char c) {
    if (circ_is_full(cb)) {
        printf("⚠️ Overflow: Cannot write '%c', buffer is full.\n", c);
        return 0;
    }
    cb->data[cb->tail] = c;
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count++;
    return 1;
}

// Read a character from the buffer (returns 1 on success, 0 on underflow)
int circ_read(CircularBuffer *cb, char *c) {
    if (circ_is_empty(cb)) {
        printf("⚠️ Underflow: No data to read.\n");
        return 0;
    }
    *c = cb->data[cb->head];
    cb->head = (cb->head + 1) % cb->size;
    cb->count--;
    return 1;
}

// Helper function to print current buffer contents (without consuming)
void circ_print(CircularBuffer *cb) {
    printf("Buffer [%d/%d]: ", cb->count, cb->size);
    int idx = cb->head;
    for (int i = 0; i < cb->count; i++) {
        printf("%c", cb->data[idx]);
        idx = (idx + 1) % cb->size;
    }
    printf("\n");
}

int main() {
    // 1. Read name from user
    char name[100];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';  // remove newline

    // 2. Append the string "CE-ESY" to the name
    char suffix[] = "CE-ESY";
    char *fullString = (char*)malloc(strlen(name) + strlen(suffix) + 1);
    if (!fullString) {
        printf("Memory allocation error.\n");
        return 1;
    }
    strcpy(fullString, name);
    strcat(fullString, suffix);
    int totalLen = strlen(fullString);
    printf("\nFull string to store: \"%s\" (length %d)\n\n", fullString, totalLen);

    // 3. Test two cases: buffer size smaller than string, and buffer size larger
    int testSizes[] = {5, 50};   // 5 < totalLen (usually) and 50 > totalLen
    for (int t = 0; t < 2; t++) {
        int bufSize = testSizes[t];
        printf("========== Testing with buffer size = %d ==========\n", bufSize);
        CircularBuffer *cb = circ_init(bufSize);
        if (!cb) {
            printf("Buffer initialization failed.\n");
            continue;
        }

        // Write each character of fullString into the buffer
        printf("📝 Writing...\n");
        int written = 0;
        for (int i = 0; i < totalLen; i++) {
            if (circ_write(cb, fullString[i])) {
                written++;
            } else {
                // Stop on overflow (proper handling)
                printf("Stopped writing after %d characters because buffer became full.\n", written);
                break;
            }
        }
        printf("Wrote %d out of %d characters.\n", written, totalLen);
        circ_print(cb);

        // Read data and display it
        printf("📖 Reading...\n");
        char *readBuffer = (char*)malloc(totalLen + 1);
        if (!readBuffer) {
            printf("Memory allocation error.\n");
            circ_free(cb);
            continue;
        }
        int readIdx = 0;
        char ch;
        while (circ_read(cb, &ch)) {
            readBuffer[readIdx++] = ch;
        }
        readBuffer[readIdx] = '\0';
        printf("Read content: \"%s\"\n", readBuffer);

        // Verify the buffer is empty after reading
        if (circ_is_empty(cb)) {
            printf("✅ Buffer is now empty.\n");
        } else {
            printf("❌ Buffer is not empty (%d items left).\n", cb->count);
        }

        free(readBuffer);
        circ_free(cb);
        printf("\n");
    }

    free(fullString);
    return 0;
}
