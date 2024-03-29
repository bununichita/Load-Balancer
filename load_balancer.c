/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "load_balancer.h"

unsigned int hash_function_servers(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a)
{
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

load_balancer *init_load_balancer()
{
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

void add_first(load_balancer *main, label a, label b, label c)
{
	main->serv_vect[0] = a;
	if (b.tag < a.tag) {
		main->serv_vect[0] = b;
		main->serv_vect[1] = a;
	} else {
		main->serv_vect[1] = b;
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
}

label find_next_label(load_balancer *main, label a)
{
	label next_label;
	next_label = main->serv_vect[0];
	if (a.tag >= main->serv_vect[main->serv_num - 1].tag) {
		next_label = main->serv_vect[0];
	} else {
		for (unsigned int i = 0; i < main->serv_num; i++) {
			if (main->serv_vect[i].tag > a.tag) {
				next_label = main->serv_vect[i];
				break;
			}
		}
	}
	return next_label;
}

void load_balance_add(load_balancer *main, label a, label next_label)
{
	for (unsigned int i = 0; i < next_label.server->hmax; i++) {
		linked_list_t *list = next_label.server->buckets[i];
		ll_node_t *curr;
		curr = list->head;
		for (unsigned int j = 0; j < list->size; j++) {
			unsigned int hash;
			product *prod;
			prod = curr->data;
			hash = next_label.server->hash_function(prod->key);
			ll_node_t *aux;
			aux = curr->next;
			int next_id = next_label.server_id;
			int first_id = main->serv_vect[0].server_id;
			unsigned int last_tag = main->serv_vect[main->serv_num - 1].tag;
			unsigned int first_tag = main->serv_vect[0].tag;
			if (hash < a.tag && next_id != first_id) {
				server_store(a.server, prod->key, prod->value);
				server_remove(next_label.server, prod->key);
			} else if (a.tag > last_tag && hash < a.tag && next_id == first_id) {
				server_store(a.server, prod->key, prod->value);
				server_remove(next_label.server, prod->key);
			} else if ((hash > last_tag || hash < a.tag)) {
				if (next_id == first_id && a.tag < first_tag) {
					server_store(a.server, prod->key, prod->value);
					server_remove(next_label.server, prod->key);
				}
			}
			curr = aux;
		}
	}
}

void server_vector_add(load_balancer *main, label a, label next_label)
{
	unsigned int poz;
	if (next_label.server_id == main->serv_vect[0].server_id) {
		if (a.tag < next_label.tag) {
			poz = 0;
		} else {
			poz = main->serv_num;
		}
	} else {
		for (unsigned i = 1; i < main->serv_num; i++) {
			if (main->serv_vect[i].server_id == next_label.server_id) {
				poz = i;
				break;
			}
		}
	}
	if (!poz) {
		for (unsigned int i = main->serv_num; i > 0; i--) {
			main->serv_vect[i] = main->serv_vect[i - 1];
		}
		main->serv_vect[0] = a;
		main->serv_num++;
	} else {
		for (unsigned int i = main->serv_num; i > poz; i--)
			main->serv_vect[i] = main->serv_vect[i - 1];
		main->serv_vect[poz] = a;
		main->serv_num++;
	}
}

void loader_add_server(load_balancer *main, int server_id)
{
	/* TODO 2 */
	server_memory *new_1, *new_2, *new_3;
	new_1 = init_server_memory();
	new_2 = init_server_memory();
	new_3 = init_server_memory();
	label a, b, c;
	a.server = new_1;
	b.server = new_2;
	c.server = new_3;
	a.tag = main->hash_function_servers(&server_id);
	a.server_id = server_id;
	server_id += 100000;
	b.tag = main->hash_function_servers(&server_id);
	b.server_id = server_id;
	server_id += 100000;
	c.tag = main->hash_function_servers(&server_id);
	c.server_id = server_id;
	if (main->serv_num >= main->hmax - 2) {
		size_t new_size = main->hmax * 2 * sizeof(label);
		main->serv_vect = realloc(main->serv_vect, new_size);
		main->hmax *= 2;
	}

	label next_label;
	if (!main->serv_num) {
		add_first(main, a, b, c);
	} else {
		next_label = find_next_label(main, a);

		load_balance_add(main, a, next_label);

		server_vector_add(main, a, next_label);

		next_label = find_next_label(main, b);

		load_balance_add(main, b, next_label);

		server_vector_add(main, b, next_label);

		next_label = find_next_label(main, c);

		load_balance_add(main, c, next_label);

		server_vector_add(main, c, next_label);
	}
}

void load_rem_label_vect(load_balancer *main, unsigned int id)
{
	for (unsigned int i = 0; i < main->serv_num; i++) {
		if ((unsigned int)main->serv_vect[i].server_id == id) {
			for (unsigned int j = i; j < main->serv_num - 1; j++) {
				main->serv_vect[j] = main->serv_vect[j + 1];
			}
			main->serv_num--;
			return;
		}
	}
}

void load_balance_rem(load_balancer *main, int server_id)
{
	label a;
	for (unsigned int i = 0; i < main->serv_num; i++) {
		if (main->serv_vect[i].server_id == server_id) {
			a = main->serv_vect[i];
			break;
		}
	}
	label next_label;
	next_label = find_next_label(main, a);
	for (unsigned int i = 0; i < a.server->hmax; i++) {
		linked_list_t *list = a.server->buckets[i];
		ll_node_t *curr;
		curr = list->head;
		for (unsigned int j = 0; j < list->size; j++) {
			product *prod;
			prod = curr->data;
			server_store(next_label.server, prod->key, prod->value);
			curr = curr->next;
		}
	}
	free_server_memory(a.server);
	load_rem_label_vect(main, server_id);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	/* TODO 3 */
	load_balance_rem(main, server_id);
	server_id += 100000;
	load_balance_rem(main, server_id);
	server_id += 100000;
	load_balance_rem(main, server_id);
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id)
{
	/* TODO 4 */
	unsigned int prod_id;
	server_memory *server;
	prod_id = main->hash_function_key(key);
	if (prod_id > main->serv_vect[main->serv_num - 1].tag) {
		*server_id = main->serv_vect[0].server_id;
		server = main->serv_vect[0].server;

	} else {
		for (unsigned int i = 0; i < main->serv_num; i++) {
			if (main->serv_vect[i].tag > prod_id) {
				*server_id = main->serv_vect[i].server_id;
				server = main->serv_vect[i].server;
				break;
			}
		}
	}
	*server_id = *server_id - (*server_id / 100000) * 100000;
	server_store(server, key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
	/* TODO 5 */
	unsigned int prod_id;
	server_memory *server;
	prod_id = main->hash_function_key(key);
	if (prod_id > main->serv_vect[main->serv_num - 1].tag) {
		*server_id = main->serv_vect[0].server_id;
		server = main->serv_vect[0].server;
	} else {
		for (unsigned int i = 0; i < main->serv_num; i++) {
			if (prod_id < main->serv_vect[i].tag) {
				*server_id = main->serv_vect[i].server_id;
				server = main->serv_vect[i].server;
				break;
			}
		}
	}
	*server_id = *server_id - (*server_id / 100000) * 100000;
	for (unsigned int i = 0; i < server->hmax; i++) {
		linked_list_t *list = server->buckets[i];
		ll_node_t *curr;
		curr = list->head;
		for (unsigned int j = 0; j < list->size; j++) {
			ll_node_t *aux;
			aux = curr->next;

			curr = aux;
		}
	}
	return server_retrieve(server, key);
}

void free_load_balancer(load_balancer *main)
{
	/* TODO 6 */
	server_memory *server;
	for (unsigned int i = 0; i < main->serv_num; i++) {
		server = main->serv_vect[i].server;
		if (server) {
			free_server_memory(server);
		}
	}
	free(main->serv_vect);
	free(main);
}
