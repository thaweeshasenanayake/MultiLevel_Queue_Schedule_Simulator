#include <stdio.h>
#include <stdlib.h>

// Structure to represent a process
struct Process {
    int id;            // Process ID
    int priority;      // Priority of the process
    int burst_time;    // Burst time required for execution
    int remaining_time; // Remaining burst time
    int arrival_time;  // Arrival time of the process
};

// Structure to represent a queue
struct Queue {
    struct Process* processes; // Array of processes in the queue
    int front, rear;           // Front and rear indices of the queue
    int capacity;              // Maximum capacity of the queue
};

// Function to initialize a queue
struct Queue* createQueue(int capacity) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->processes = (struct Process*)malloc(sizeof(struct Process) * capacity);
    queue->front = queue->rear = -1;
    queue->capacity = capacity;
    return queue;
}

// Function to check if a queue is empty
int isEmpty(struct Queue* queue) {
    return queue->front == -1;
}

// Function to enqueue a process into a queue
void enqueue(struct Queue* queue, struct Process process) {
    if (queue->rear == queue->capacity - 1) {
        printf("Queue overflow\n");
        return;
    }

    if (isEmpty(queue)) {
        queue->front = 0;
    }

    queue->rear++;
    queue->processes[queue->rear] = process;
}

// Function to dequeue a process from a queue
struct Process dequeue(struct Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue underflow\n");
        exit(EXIT_FAILURE);
    }

    struct Process process = queue->processes[queue->front];

    if (queue->front == queue->rear) {
        queue->front = queue->rear = -1;
    } else {
        queue->front++;
    }

    return process;
}

// Function to simulate Round Robin scheduling within a queue
void RR_Scheduler(struct Queue* queue, int timeQuantum) {
    int timeElapsed = 0; // Total time elapsed

    while (!isEmpty(queue)) {
        struct Process currentProcess = dequeue(queue);

        printf("Executing Process P%d for %d seconds\n", currentProcess.id, (currentProcess.remaining_time < timeQuantum) ? currentProcess.remaining_time : timeQuantum);

        if (currentProcess.remaining_time > timeQuantum) {
            currentProcess.remaining_time -= timeQuantum;
            // Re-enqueue the process to the same queue
            enqueue(queue, currentProcess);
            timeElapsed += timeQuantum; // Increment by the actual time spent in execution
        } else {
            timeElapsed += currentProcess.remaining_time; // Increment by the remaining time
            currentProcess.remaining_time = 0;
            printf("Process P%d completed execution\n", currentProcess.id);
        }
    }

    printf("Total time elapsed: %d seconds\n", timeElapsed);
}

// Function to simulate MLQ CPU scheduling
void MLQScheduler(struct Queue* queue1, struct Queue* queue2, int rrTimeQuantum) {
    int timeElapsed = 0;  // Total time elapsed
    int currentTime = 0;  // Current time in the simulation

    // Simulate MLQ scheduling until both queues are empty
    while (!isEmpty(queue1) || !isEmpty(queue2)) {
        // Execute processes in Queue 1 with priority scheduling and consideration of arrival time
        while (!isEmpty(queue1) && queue1->processes[queue1->front].arrival_time <= currentTime) {
            struct Process currentProcess = dequeue(queue1);

            printf("Executing Process P%d from Queue 1 for %d seconds\n", currentProcess.id, (currentProcess.remaining_time < rrTimeQuantum) ? currentProcess.remaining_time : rrTimeQuantum);

            if (currentProcess.remaining_time > rrTimeQuantum) {
                currentProcess.remaining_time -= rrTimeQuantum;
                // Re-enqueue the process to the same queue
                enqueue(queue1, currentProcess);
                timeElapsed += rrTimeQuantum; // Increment by the actual time spent in execution
            } else {
                timeElapsed += currentProcess.remaining_time; // Increment by the remaining time
                currentProcess.remaining_time = 0;
                printf("Process P%d completed execution\n", currentProcess.id);
            }
        }

        // If no processes in Queue 1 or processes in Queue 1 have higher arrival times, execute processes in Queue 2
        if ((isEmpty(queue1) || queue1->processes[queue1->front].arrival_time > currentTime) && !isEmpty(queue2) && queue2->processes[queue2->front].arrival_time <= currentTime) {
            struct Process currentProcess = dequeue(queue2);

            printf("Executing Process P%d from Queue 2 in FCFS order for %d seconds\n", currentProcess.id, currentProcess.remaining_time);

            timeElapsed += currentProcess.remaining_time; // Increment by the remaining time
            currentProcess.remaining_time = 0;
            printf("Process P%d completed execution\n", currentProcess.id);
        }

        currentTime++;
    }

    printf("Total time elapsed: %d seconds\n", timeElapsed);
}


int main() {
    int numProcesses, rrTimeQuantum;
    printf("Enter the number of processes: ");
    scanf("%d", &numProcesses);

    // Create two queues for MLQ
    struct Queue* queue1 = createQueue(10);
    struct Queue* queue2 = createQueue(10);

    printf("Enter the time quantum for Round Robin scheduling: ");
    scanf("%d", &rrTimeQuantum);

    // Input process details
    for (int i = 0; i < numProcesses; ++i) {
        struct Process process;
        printf("Enter details for Process P%d:\n", i + 1);
        process.id = i + 1;
        printf("Priority (1 for high, 2 for low): ");
        scanf("%d", &process.priority);
        printf("Burst Time: ");
        scanf("%d", &process.burst_time);
        printf("Arrival Time: ");
        scanf("%d", &process.arrival_time);
        process.remaining_time = process.burst_time;

        // Enqueue processes to the initial queue based on priority
        if (process.priority == 1) {
            enqueue(queue1, process);
        } else {
            enqueue(queue2, process);
        }
    }

    // Simulate MLQ CPU scheduling
    MLQScheduler(queue1, queue2, rrTimeQuantum);

    // Cleanup
    free(queue1->processes);
    free(queue1);
    free(queue2->processes);
    free(queue2);

    return 0;
}
