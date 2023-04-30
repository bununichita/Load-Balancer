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

// void key_val_free_function(void *data) {
// 	/* TODO */
//     free(((server *)data)->key);
// 	free(((server *)data)->value);
// 	free(data);
// }

load_balancer *init_load_balancer() {
    /* TODO 1 */

    load_balancer *l_b;
    l_b = malloc(sizeof(load_balancer));
    l_b->hash_function_servers = hash_function_servers;
    l_b->hash_function_key = hash_function_key;
    l_b->serv_num = 0;
    l_b->hmax = 10;
    l_b->serv_vect = malloc(10 * sizeof(label));
    return l_b;
}

void loader_add_server(load_balancer *main, int server_id) {
    /* TODO 2 */
    server_memory *new;
    new = malloc(sizeof(server_memory));
    label a, b, c;
    a.parent = new;
    b.parent = new;
    c.parent = new;
    a.tag = main->hash_function_servers(&server_id);
    server_id += 100000;
    b.tag = main->hash_function_servers(&server_id);
    server_id += 100000;
    c.tag = main->hash_function_servers(&server_id);
    if (main->serv_num == main->hmax) {
        main->serv_vect = realloc(main->serv_vect, main->hmax * 2 * sizeof(server_memory *));
        main->hmax *= 2;
    }

    if (!main->serv_num) {
        main->serv_vect[0] = a;
        if (b.tag < a.tag) {
            main->serv_vect[0] = b;
            main->serv_vect[1] = a;
        }
        if (c.tag < main->serv_vect[0].tag) {
            main->serv_vect[2] = main->serv_vect[1];
            main->serv_vect[1] = main->serv_vect[0];
            main->serv_vect[0] = c;
        } else if (c.tag > main->serv_vect[1].tag) {
            main->serv_vect[2] = c;
        } else {
            main->serv_vect[2] = main->serv_vect[1];
            main->serv_vect[1] = c;
        }
        main->serv_num = 3;
        return;
    }

    if (a.tag < main->serv_vect[0].tag) {
        for (int i = main->serv_num; i > 0; i--)
            main->serv_vect[i] = main->serv_vect[i - 1];
        main->serv_vect[0] = a;
        main->serv_num++;
    } else {
        for (int i = 1; i < main->serv_num; i++) {
            if (main->serv_vect[i - 1].tag < a.tag) {
                for (int j = main->serv_num; j > i; j--)
                    main->serv_vect[j] = main->serv_vect[j - 1];
                main->serv_vect[i] = a;
                main->serv_num++;
                break;
            }
        }
    }

    if (b.tag < main->serv_vect[0].tag) {
        for (int i = main->serv_num; i > 0; i--)
            main->serv_vect[i] = main->serv_vect[i - 1];
        main->serv_vect[0] = b;
        main->serv_num++;
    } else {
        for (int i = 1; i < main->serv_num; i++) {
            if (main->serv_vect[i - 1].tag < b.tag) {
                for (int j = main->serv_num; j > i; j--)
                    main->serv_vect[j] = main->serv_vect[j - 1];
                main->serv_vect[i] = b;
                main->serv_num++;
                break;
            }
        }
    }

    if (c.tag < main->serv_vect[0].tag) {
        for (int i = main->serv_num; i > 0; i--)
            main->serv_vect[i] = main->serv_vect[i - 1];
        main->serv_vect[0] = c;
        main->serv_num++;
    } else {
        for (int i = 1; i < main->serv_num; i++) {
            if (main->serv_vect[i - 1].tag < c.tag) {
                for (int j = main->serv_num; j > i; j--)
                    main->serv_vect[j] = main->serv_vect[j - 1];
                main->serv_vect[i] = c;
                main->serv_num++;
                break;
            }
        }
    }

}

void server_free(server_memory **serv)
{
    server_memory *server;
    server = *serv;
    for (int i = 0; i < server->hmax; i++) {
		server->buckets[i] = ll_free(sizeof(product));
	}
    free(server);
}

void loader_remove_aux(load_balancer *main, unsigned int id)
{
    for (int i = 0; i < main->serv_num; i++) {
        if (main->serv_vect[i].tag == id) {
            if (main->serv_vect[i].parent) {
                server_free(&main->serv_vect[i].parent);
            }            
            for (int j = i; j < main->serv_num - 1; j++) {
                main->serv_vect[j] = main->serv_vect[j + 1];
            }
            main->serv_num--;
        }
    }
}

void loader_remove_server(load_balancer *main, int server_id) {
    /* TODO 3 */
    unsigned int id = main->hash_function_servers(server_id);
    loader_remove_aux(main, id);
    server_id += 100000;
    id = main->hash_function_servers(server_id);
    loader_remove_aux(main, id);
    server_id += 100000;
    id = main->hash_function_servers(server_id);
    loader_remove_aux(main, id);
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
    /* TODO 4 */
    unsigned int prod_id;
    server_memory *server;
    prod_id = main->hash_function_key(key);
    if (prod_id > main->serv_vect[main->serv_num - 1].tag) {
        server_id = &main->serv_vect[0].tag;
        server = main->serv_vect[0].parent;
    } else {
        for (int i = 0; i < main->serv_num; i++) {
            if (prod_id < main->serv_vect[i].tag) {
                server_id = &main->serv_vect[i].tag;
                server = main->serv_vect[i].parent;
                break;
            }
        }
    }
    server_store(server, key, value);
    
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    /* TODO 5 */
    unsigned int prod_id;
    server_memory *server;
    prod_id = main->hash_function_key(key);
    if (prod_id > main->serv_vect[main->serv_num - 1].tag) {
        server_id = &main->serv_vect[0].tag;
        server = main->serv_vect[0].parent;
    } else {
        for (int i = 0; i < main->serv_num; i++) {
            if (prod_id < main->serv_vect[i].tag) {
                server_id = &main->serv_vect[i].tag;
                server = main->serv_vect[i].parent;
                break;
            }
        }
    }
    return server_retrieve(server, key);
    return NULL;
}

void free_load_balancer(load_balancer *main) {
    /* TODO 6 */
    server_memory *server;
    for (int i = 0; i < main->serv_num; i++) {
        server = main->serv_vect[i].parent;
        if (server) {
            free_server_memory(server);
        }
    }
    free(main);
}
