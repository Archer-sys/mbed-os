#include "mbed.h"
#include "SDFileSystem.h"
#include "test_env.h"
#include <algorithm>
#include <stdlib.h>

#if defined(TARGET_KL25Z)
SDFileSystem sd(PTD2, PTD3, PTD1, PTD0, "sd");

#elif defined(TARGET_KL46Z)
SDFileSystem sd(PTD6, PTD7, PTD5, PTD4, "sd");

#elif defined(TARGET_K64F)
SDFileSystem sd(PTD2, PTD3, PTD1, PTD0, "sd");

#elif defined(TARGET_K20D50M)
SDFileSystem sd(PTD2, PTD3, PTD1, PTC2, "sd");

#elif defined(TARGET_nRF51822)
SDFileSystem sd(p12, p13, p15, p14, "sd");

#elif defined(TARGET_NUCLEO_F103RB) || \
    defined(TARGET_NUCLEO_L152RE) || \
    defined(TARGET_NUCLEO_F302R8) || \
    defined(TARGET_NUCLEO_F030R8) || \
    defined(TARGET_NUCLEO_F401RE) || \
    defined(TARGET_NUCLEO_F411RE) || \
    defined(TARGET_NUCLEO_F072RB) || \
    defined(TARGET_NUCLEO_F334R8) || \
    defined(TARGET_NUCLEO_L053R8)
SDFileSystem sd(D11, D12, D13, D10, "sd");

#elif defined(TARGET_DISCO_F051R8)
SDFileSystem sd(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS, "sd");

#elif defined(TARGET_LPC2368)
SDFileSystem sd(p11, p12, p13, p14, "sd");

#elif defined(TARGET_LPC11U68)
SDFileSystem sd(D11, D12, D13, D10, "sd");

#elif defined(TARGET_LPC1549)
SDFileSystem sd(D11, D12, D13, D10, "sd");

#else
SDFileSystem sd(p11, p12, p13, p14, "sd");
#endif

namespace
{
char buffer[1024];
const int KIB_RW = 128;
Timer timer;
const char *bin_filename = "/sd/testfile.bin";
}

bool test_sf_file_write_stdio(const char *filename, const int kib_rw)
{
    bool result = true;
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        int byte_write = 0;
        timer.start();
        for (int i = 0; i < kib_rw; i++) {
            if (fwrite(buffer, sizeof(char), sizeof(buffer), file) != sizeof(buffer)) {
                result = false;
                fclose(file);
                printf("Write error!\r\n");
                break;
            } else {
                byte_write++;
            }
        }
        timer.stop();
        fclose(file);
        double test_time_sec = timer.read_us() / 1000000.0;
        printf("%d KiB write in %.3f sec with speed of %.4f KiB/s\r\n", byte_write, test_time_sec, kib_rw / test_time_sec);
    } else {
        printf("File '%s' not opened\r\n", filename);
        result = false;
    }
    timer.reset();
    return result;
}

bool test_sf_file_read_stdio(const char *filename, const int kib_rw)
{
    bool result = true;
    FILE* file = fopen(filename, "r");
    if (file) {
        timer.start();
        int byte_read = 0;
        while (fread(buffer, sizeof(char), sizeof(buffer), file) == sizeof(buffer)) {
            byte_read++;
        }
        timer.stop();
        fclose(file);
        double test_time_sec = timer.read_us() / 1000000.0;
        printf("%d KiB read in %.3f sec with speed of %.4f KiB/s\r\n", byte_read, test_time_sec, kib_rw / test_time_sec);
    } else {
        printf("File '%s' not opened\r\n", filename);
        result = false;
    }
    timer.reset();
    return result;
}

char RandomChar()
{
    return rand() % 100;
}

int main()
{
    // Test header
    printf("\r\n");
    printf("SD Card Stdio Performance Test\r\n");
    printf("File name: %s\r\n", bin_filename);
    printf("Buffer size: %d KiB\r\n", (KIB_RW * sizeof(buffer)) / 1024);

    // Initialize buffer
    srand(testenv_randseed());
    char *buffer_end = buffer + sizeof(buffer);
    std::generate (buffer, buffer_end, RandomChar);

    bool result = true;
    for (;;) {
        printf("Write test...\r\n");
        if (test_sf_file_write_stdio(bin_filename, KIB_RW) == false) {
            result = false;
            break;
        }

        printf("Read test...\r\n");
        if (test_sf_file_read_stdio(bin_filename, KIB_RW) == false) {
            result = false;
            break;
        }
        break;
    }
    notify_completion(result);
}