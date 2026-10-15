#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MAX_FLIGHTS 100
#define MAX_PASSENGERS 200
#define MAX_STACK 100
#define MAX_QUEUE 100

// Structure to represent a flight
typedef struct {
    int flightNum;
    char origin[50];
    char destination[50];
    int numSeats;
    int bookedSeats;
} Flight;

// Structure to represent a passenger
typedef struct {
    char name[50];
    int flightNum;
    int action; // 1 for booking, -1 for cancellation
} Passenger;

// Global variables
Flight flights[MAX_FLIGHTS];
Passenger passengers[MAX_PASSENGERS];
Passenger stack[MAX_STACK];
Passenger queue[MAX_QUEUE];
int numFlights = 0;
int numPassengers = 0;
int top = -1;
int front = -1, rear = -1;

// Function prototypes
void initializeFlights();
void displayFlights();
void bookFlight();
void cancelFlight();
void displayPurchasedTickets();
void undoLastOperation();
void push(Passenger p);
Passenger pop();
void enqueue(Passenger p);
Passenger dequeue();
int isQueueFull();
int isQueueEmpty();
void processWaitingList(int flightNum);
void saveDataToFile();
void loadDataFromFile();
void clearScreen();
void printHeader();

// Main function
int main() {
    int choice;
    initializeFlights();
    loadDataFromFile();

    printf("Welcome to the Flight Booking System\n");
    printHeader();

    do {
        printf("\nMake your choice:\n");
        printf("\t1. Display Flights\n");
        printf("\t2. Book Flight\n");
        printf("\t3. Cancel Flight\n");
        printf("\t4. Display Purchased Tickets\n");
        printf("\t5. Undo Last Operation\n");
        printf("\t6. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        clearScreen();

        switch (choice) {
            case 1:
                displayFlights();
                break;
            case 2:
                bookFlight();
                break;
            case 3:
                cancelFlight();
                break;
            case 4:
                displayPurchasedTickets();
                break;
            case 5:
                undoLastOperation();
                break;
            case 6:
                printf("Saving data and exiting program...\n");
                saveDataToFile();
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);

    return 0;
}

// Function to initialize flights
void initializeFlights() {
    // Add some sample flights
    flights[0] = (Flight){101, "New York", "Los Angeles", 150, 0};
    flights[1] = (Flight){202, "Chicago", "Miami", 120, 0};
    flights[2] = (Flight){303, "Seattle", "San Francisco", 100, 0};
    flights[3] = (Flight){404, "Houston", "Denver", 180, 0};
    flights[4] = (Flight){505, "Atlanta", "Las Vegas", 200, 0};
    flights[5] = (Flight){606, "Boston", "Orlando", 160, 0};
    flights[6] = (Flight){707, "Dallas", "Phoenix", 170, 0};
    flights[7] = (Flight){808, "Washington DC", "New Orleans", 140, 0};
    flights[8] = (Flight){909, "San Diego", "Salt Lake City", 130, 0};
    flights[9] = (Flight){1010, "Minneapolis", "Portland", 110, 0};
    numFlights = 10; // Update the total number of flights
}

// Function to display flights
void displayFlights() {
    printf("\n\n\tFlight List:\n");
    printf("\t------------\n\n");
    printf("\t%-10s %-15s %-15s %-10s %-10s\n", "Flight No", "Origin", "Destination", "Seats", "Booked");
    printf("\t%-10s %-15s %-15s %-10s %-10s\n", "---------", "------", "-----------", "-----", "------");
    for (int i = 0; i < numFlights; i++) {
        printf("\t%-10d %-15s %-15s %-10d %-10d\n", flights[i].flightNum, flights[i].origin, flights[i].destination,
               flights[i].numSeats, flights[i].bookedSeats);
    }
}

// Function to book a flight
void bookFlight() {
    int flightNum;
    char passengerName[50];

    printf("\nEnter Flight Number: ");
    scanf("%d", &flightNum);

    int found = 0;
    for (int i = 0; i < numFlights; i++) {
        if (flights[i].flightNum == flightNum) {
            if (flights[i].bookedSeats < flights[i].numSeats) {
                printf("Enter Passenger Name: ");
                scanf("%s", passengerName);

                Passenger p = {0};
                p.flightNum = flightNum;
                strcpy(p.name, passengerName);
                p.action = 1;

                passengers[numPassengers++] = p;
                flights[i].bookedSeats++;
                push(p);

                printf("\n------------------------\n");
                printf("Flight booked successfully!\n");
                printf("------------------------\n");
            } else {
                printf("Flight is fully booked. Adding to waiting list.\n");
                printf("Enter Passenger Name: ");
                scanf("%s", passengerName);

                Passenger p = {0};
                p.flightNum = flightNum;
                strcpy(p.name, passengerName);
                p.action = 1;

                if (!isQueueFull()) {
                    enqueue(p);
                    printf("Added to waiting list.\n");
                } else {
                    printf("Waiting list is full. Cannot add to waiting list.\n");
                }
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Flight not found.\n");
    }
}

// Function to cancel a flight
void cancelFlight() {
    int flightNum;
    char passengerName[50];
    printf("\nEnter Flight Number to Cancel: ");
    scanf("%d", &flightNum);
    printf("Enter Passenger Name to Cancel: ");
    scanf("%s", passengerName);

    int found = 0;
    for (int i = 0; i < numPassengers; i++) {
        if (passengers[i].flightNum == flightNum && strcmp(passengers[i].name, passengerName) == 0) {
            Passenger canceledPassenger = passengers[i];
            canceledPassenger.action = -1;
            push(canceledPassenger);

            for (int j = i; j < numPassengers - 1; j++) {
                passengers[j] = passengers[j + 1];
            }
            numPassengers--;

            for (int j = 0; j < numFlights; j++) {
                if (flights[j].flightNum == flightNum) {
                    flights[j].bookedSeats--;
                    processWaitingList(flightNum);
                    break;
                }
            }
            printf("\n------------------------------------------------------\n");
            printf("Flight %d for passenger %s canceled successfully!\n", flightNum, passengerName);
            printf("------------------------------------------------------\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("No booking found for the given flight and passenger name.\n");
    }
}

// Function to display purchased flight tickets
void displayPurchasedTickets() {
    printf("\n\n\tPurchased Flight Tickets:\n");
    printf("\t-------------------------\n\n");
    printf("\t%-15s %-10s\n", "Passenger Name", "Flight No");
    printf("\t%-15s %-10s\n", "--------------", "---------");
    int found = 0;
    for (int i = 0; i < numPassengers; i++) {
        printf("\t%-15s %-10d\n", passengers[i].name, passengers[i].flightNum);
        found = 1;
    }
    if (!found) {
        printf("\tNo purchased flight tickets.\n");
    }
}

// Stack functions
void push(Passenger p) {
    if (top < MAX_STACK - 1) {
        stack[++top] = p;
    } else {
        printf("Stack overflow. Cannot push operation.\n");
    }
}

Passenger pop() {
    if (top >= 0) {
        return stack[top--];
    } else {
        Passenger p = {0};
        printf("Stack underflow. Cannot pop operation.\n");
        return p;
    }
}

void undoLastOperation() {
    if (top >= 0) {
        Passenger p = pop();
        if (p.action == 1) { // Undo booking
            for (int i = 0; i < numPassengers; i++) {
                if (passengers[i].flightNum == p.flightNum && strcmp(passengers[i].name, p.name) == 0) {
                    // Shift remaining passengers left
                    for (int j = i; j < numPassengers - 1; j++) {
                        passengers[j] = passengers[j + 1];
                    }
                    numPassengers--;

                    // Decrement the bookedSeats for the flight
                    for (int j = 0; j < numFlights; j++) {
                        if (flights[j].flightNum == p.flightNum) {
                            flights[j].bookedSeats--;
                            processWaitingList(p.flightNum);
                            break;
                        }
                    }
                    printf("Undo successful: Booking for passenger %s on flight %d has been canceled.\n", p.name, p.flightNum);
                    return;
                }
            }
        } else if (p.action == -1) { // Undo cancellation
            for (int i = 0; i < numFlights; i++) {
                if (flights[i].flightNum == p.flightNum && flights[i].bookedSeats < flights[i].numSeats) {
                    flights[i].bookedSeats++;
                    passengers[numPassengers++] = p;
                    printf("Undo successful: Cancellation for passenger %s on flight %d has been restored.\n", p.name, p.flightNum);
                    return;
                }
            }
            printf("Undo failed: No available seats to restore the booking for passenger %s on flight %d.\n", p.name, p.flightNum);
        }
    } else {
        printf("No operations to undo.\n");
    }
}

// Queue functions
void enqueue(Passenger p) {
    if ((rear + 1) % MAX_QUEUE == front) {
        printf("Queue is full. Cannot enqueue.\n");
    } else {
        if (front == -1) front = 0;
        rear = (rear + 1) % MAX_QUEUE;
        queue[rear] = p;
    }
}

Passenger dequeue() {
    Passenger p = {0};
    if (front == -1) {
        printf("Queue is empty. Cannot dequeue.\n");
    } else {
        p = queue[front];
        if (front == rear) {
            front = -1;
            rear = -1;
        } else {
            front = (front + 1) % MAX_QUEUE;
        }
    }
    return p;
}

int isQueueFull() {
    return (rear + 1) % MAX_QUEUE == front;
}

int isQueueEmpty() {
    return front == -1;
}

// Process waiting list for a given flight number
void processWaitingList(int flightNum) {
    if (!isQueueEmpty()) {
        for (int i = 0; i < numFlights; i++) {
            if (flights[i].flightNum == flightNum && flights[i].bookedSeats < flights[i].numSeats) {
                Passenger p = dequeue();
                if (p.flightNum == flightNum) {
                    passengers[numPassengers++] = p;
                    flights[i].bookedSeats++;
                    printf("Passenger %s moved from waiting list to flight %d.\n", p.name, flightNum);
                } else {
                    enqueue(p);  // Re-enqueue the passenger if they are waiting for a different flight
                }
            }
        }
    }
}

// Function to save data to a file
void saveDataToFile() {
    FILE *fp = fopen("flightdata.txt", "w");
    if (fp == NULL) {
        printf("Error saving data to file.\n");
        return;
    }
    // Save flights
    fprintf(fp, "%d\n", numFlights);
    for (int i = 0; i < numFlights; i++) {
        fprintf(fp, "%d %s %s %d %d\n", flights[i].flightNum, flights[i].origin, flights[i].destination,
                flights[i].numSeats, flights[i].bookedSeats);
    }
    // Save passengers
    fprintf(fp, "%d\n", numPassengers);
    for (int i = 0; i < numPassengers; i++) {
        fprintf(fp, "%s %d\n", passengers[i].name, passengers[i].flightNum);
    }
    fclose(fp);
}

// Function to load data from a file
void loadDataFromFile() {
    FILE *fp = fopen("flightdata.txt", "r");
    if (fp == NULL) {
        printf("No previous data found.\n");
        return;
    }
    // Load flights
    fscanf(fp, "%d", &numFlights);
    for (int i = 0; i < numFlights; i++) {
        fscanf(fp, "%d %s %s %d %d", &flights[i].flightNum, flights[i].origin, flights[i].destination,
               &flights[i].numSeats, &flights[i].bookedSeats);
    }
    // Load passengers
    fscanf(fp, "%d", &numPassengers);
    for (int i = 0; i < numPassengers; i++) {
        fscanf(fp, "%s %d", passengers[i].name, &passengers[i].flightNum);
    }
    fclose(fp);
}

// Function to clear the screen
void clearScreen() {
    printf("\033[H\033[J");
}

// Function to print a header
void printHeader() {
    printf("\033[1;34m-------------------------------------------\033[0m\n");
    printf("\033[1;34m|    Welcome to the Flight Booking System   |\033[0m\n");
    printf("\033[1;34m-------------------------------------------\033[0m\n");
}
