/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

void key_val_free_function(void *data) {
	/* TODO */
    free(((server *)data)->key);
	free(((server *)data)->value);
	free(data);
}

load_balancer *init_load_balancer() {
    /* TODO 1 */
    return NULL;
}

void loader_add_server(load_balancer *main, int server_id) {
    /* TODO 2 */
}

void loader_remove_server(load_balancer *main, int server_id) {
    /* TODO 3 */
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
    /* TODO 4 */
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    /* TODO 5 */
    return NULL;
}

void free_load_balancer(load_balancer *main) {
    /* TODO 6 */
}
