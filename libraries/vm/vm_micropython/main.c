#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/time.h>
#include <wasm-rt-impl.h>

jmp_buf g_jmp_buf;
uint32_t g_saved_call_stack_depth;

int micropython_init();
int micropython_contract_init(const char *mpy, size_t size);
int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action);

void *micropython_get_memory();
size_t micropython_get_memory_size();

//const char * raw_code = "\x4d\x05\x02\x1f\x20\x5c\x08\x0c\x00\x07\x08\x61\x2e\x70\x79\x28\x00\x11\x00\x7b\x23\x00\x34\x01\x59\x32\x01\x16\x0a\x61\x70\x70\x6c\x79\x51\x63\x01\x01\x73\x0b\x68\x65\x6c\x6c\x6f\x2c\x77\x6f\x72\x6c\x64\x4c\x33\x0c\x01\x03\x40\x00\x12\x00\x7b\xb0\xb1\xb2\x34\x03\x59\x51\x63\x00\x00\x02\x61\x02\x62\x02\x63";

long long get_time_us() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long us = te.tv_sec*1000000LL + te.tv_usec; // calculate milliseconds
//    printf("us: %lld\n", us);
    return us;
}

int main(int argc, char **argv) {
    FILE *fp = fopen(argv[1], "rb");
    char raw_code[1024*10];
    size_t size = fread(raw_code, 1, sizeof(raw_code), fp);
    micropython_init();
    micropython_contract_init(raw_code, size);

    void *vm_memory = micropython_get_memory();
    size_t vm_memory_size = micropython_get_memory_size();
    void *vm_memory_backup = malloc(vm_memory_size);

    memcpy(vm_memory_backup, vm_memory, vm_memory_size);

    long long total_time = 0;
    int count = 500;
    for (int i=0;i<count;i++)
    {
        long long start = get_time_us();
        memcpy(vm_memory, vm_memory_backup, vm_memory_size);
        micropython_contract_apply(1, 2, 3);
        long long duration = get_time_us() - start;
        total_time += duration;
//        printf("duration: %lld\n", duration);
    }
    printf("+++avg: %lld\n", total_time/count);


    total_time = 0;
    for (int i=0;i<count;i++)
    {
        long long start = get_time_us();
//        memcpy(vm_memory, vm_memory_backup, vm_memory_size);
        micropython_contract_apply(1, 2, 3);
        long long duration = get_time_us() - start;
        total_time += duration;
//        printf("duration: %lld\n", get_time_us() - start);
    }
    printf("+++avg: %lld\n", total_time/count);
    return 0;
}

