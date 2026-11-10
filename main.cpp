#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>

#define SHM_NAME "/lab6_shm"
#define SEM_NAME "/lab6_sem"
#define STRUCT_SIZE sizeof(int)

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, STRUCT_SIZE);

    int* counter = (int*)mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *counter = 1;

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Eroare la fork!\n";
        return 1;
    }

    srand(time(nullptr) + getpid());

    while (true) {
        sem_wait(sem);

        if (*counter > 1000) {
            sem_post(sem);
            break;
        }

        int ban = (rand() % 2) + 1;

        if (ban == 2 || true) {
            int current = *counter;
            if (current <= 1000) {
                std::cout << (pid == 0 ? "[Fiu] " : "[Parinte] ") 
                          << "Valoare curenta: " << current << "\n";
                (*counter)++;
            }
        }

        sem_post(sem);

        usleep(100000);
    }

    if (pid > 0) {
        wait(NULL);
        std::cout << "Valoare finala in memorie: " << *counter << "\n";
        
        munmap(counter, STRUCT_SIZE);
        close(shm_fd);
        shm_unlink(SHM_NAME);
        sem_close(sem);
        sem_unlink(SEM_NAME);
    } else {
        munmap(counter, STRUCT_SIZE);
        close(shm_fd);
        sem_close(sem);
    }

    return 0;
}
