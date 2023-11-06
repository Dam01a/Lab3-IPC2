#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int *BankAccount;
int *Turn;

// Function to generate a random sleep interval between 0 and 5 seconds
void randomSleep() {
    int sleepTime = rand() % 6;
    sleep(sleepTime);
}

int main() {
    // Create shared memory for BankAccount and Turn
    int shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    BankAccount = (int *)shmat(shmid, NULL, 0);
    Turn = BankAccount + 1;

    *BankAccount = 0;
    *Turn = 0;

    srand(time(NULL)); // Seed the random number generator

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    }

    if (pid == 0) {
        // Child process (Poor Student)
        for (int i = 0; i < 25; i++) {
            randomSleep();
            int account = *BankAccount;
            while (*Turn != 1)
                ; // Wait for Turn to be 1

            int balanceNeeded = rand() % 51;
            printf("Poor Student needs $%d\n", balanceNeeded);

            if (balanceNeeded <= account) {
                account -= balanceNeeded;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", balanceNeeded, account);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", account);
            }

            *BankAccount = account;
            *Turn = 0;
        }
    } else {
        // Parent process (Dear Old Dad)
        for (int i = 0; i < 25; i++) {
            randomSleep();
            int account = *BankAccount;
            while (*Turn != 0)
                ; // Wait for Turn to be 0

            if (account <= 100) {
                int balance = rand() % 101;
                if (balance % 2 == 0) {
                    account += balance;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", balance, account);
                } else {
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", account);
            }

            *BankAccount = account;
            *Turn = 1;
        }
    }

    shmdt(BankAccount);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
