#include <stdio.h>
#include <string.h>

#define SIZE_SRAM 1024
#define SIZE_DRAM 8192

char SRAM[SIZE_SRAM]; // SRAM interne
char DRAM[SIZE_DRAM]; // DRAM externe

// ----- First Stage Bootloader -----
void FSBL() {
    printf("FSBL: Initialisation du CPU et SRAM\n");
    memset(SRAM, 0, SIZE_SRAM);

    printf("FSBL: Calibration de la DRAM\n");
    for (int i = 0; i < SIZE_DRAM; i++)
        DRAM[i] = 0; // simuler la calibration
}

// ----- Second Stage Bootloader -----
void SSBL() {
    printf("SSBL: Initialisation du hardware et chargement du kernel\n");
    const char* kernel = "Kernel loaded in DRAM";
    strncpy(DRAM, kernel, strlen(kernel)+1);

    printf("SSBL: Préparation de la SRAM pour stack/heap\n");
    for (int i = 0; i < SIZE_SRAM; i++)
        SRAM[i] = 0; // reset SRAM pour stack/heap
}

// ----- Application principale -----
void Application() {
    printf("Application: Execution du code en DRAM, utilisation SRAM pour stack/heap\n");

    // Exemple: écrire une variable locale dans SRAM (stack simulation)
    int stack_var = 42;
    printf("Stack variable = %d\n", stack_var);

    // Exemple: lire une donnée depuis DRAM
    printf("DRAM content: %s\n", DRAM);
}

int main() {
    printf("=== Boot Simulation Start ===\n\n");

    FSBL();    // First Stage Bootloader
    SSBL();    // Second Stage Bootloader
    Application(); // Application principale

    printf("\n=== Boot Simulation End ===\n");
    return 0;
}
