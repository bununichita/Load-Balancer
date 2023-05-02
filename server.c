/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>

#include "server.h"

// typedef struct product;
// struct product {
// 	void *key;
// 	void *value;
// };

//struct server_memory;
// typedef struct server_memory server_memory;
// struct server_memory {
//     linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
// 	/* Nr. total de noduri existente curent in toate bucket-urile. */
// 	unsigned int size;
// 	unsigned int hmax; /* Nr. de bucket-uri. */
// 	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
// 	unsigned int (*hash_function)(void*);
// 	/* (Pointer la) Functie pentru a compara doua chei. */
// 	int (*compare_function)(void*, void*);
// 	/* (Pointer la) Functie pentru a elibera memoria ocupata de cheie si valoare. */
// 	void (*key_val_free_function)(void*);
// }

linked_list_t *ll_create(unsigned int data_size)
{
    linked_list_t* ll;
    ll = malloc(sizeof(*ll));
    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;
    return ll;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }
    if (n > list->size) {
        n = list->size;
    }
    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }
	new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);
    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }
    list->size++;
}

ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        key_val_free_function(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

unsigned int hash_function_k(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;
	return strcmp(str_a, str_b);
}

void key_val_free_function(void *data) {
	/* TODO */
    free(((product *)data)->key);
	free(((product *)data)->value);
	free(data);
}

int ht_has_key(server_memory *ht, void *key)
{
	/* TODO */
	unsigned int valoare_hash = ht->hash_function(key) % ht->hmax;
	ll_node_t *current;
	if (ht->buckets[valoare_hash]->size == 0) {
		return 0;
	}
	current = ht->buckets[valoare_hash]->head;
	while (current->next) {
		if (!ht->compare_function(key, ((product *)current->data)->key)) {
			return 1;
		}
		current = current->next;
	}
	if (!ht->compare_function(key, ((product *)current->data)->key)) {
		return 1;
	}
	
	return 0;
}

char *ht_get(server_memory *ht, void *key)
{
	/* TODO */
	unsigned int valoare_hash = ht->hash_function(key) % ht->hmax;
	ll_node_t *current;
	if (ht->buckets[valoare_hash]->size == 0) {
		return NULL;
	}
	current = ht->buckets[valoare_hash]->head;
	while (current->next) {
		if (!ht->compare_function(key, ((product *)current->data)->key)) {
			return ((product *)current->data)->value;
		}
		current = current->next;
	}
	if (!ht->compare_function(key, ((product *)current->data)->key)) {
		return ((product *)current->data)->value;
	}
	return NULL;
}

server_memory *init_server_memory()
{
	/* TODO 1 */

	server_memory *server;
	server = malloc(sizeof(server_memory));
	server->compare_function = compare_function_strings;
	server->hash_function = hash_function_k;
	server->key_val_free_function = key_val_free_function;
	server->key_size = 128;
    server->value_size = 65536;
	server->hmax = 20;
	server->size = 0;
	server->buckets = malloc(server->hmax * sizeof(linked_list_t *));
	for (unsigned int i = 0; i < server->hmax; i++) {
		server->buckets[i] = ll_create(sizeof(product));
	}
	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	/* TODO 2 */
	unsigned int valoare_hash = server->hash_function(key) % server->hmax;
	if(ht_get(server, key)) {
		//Exista cheia si modific valoarea
		// ll_node_t *current;
		// current = ht->buckets[valoare_hash]->head;
		// product *new = (product *)malloc(sizeof(product));
		// new->key = malloc(server->key_size);
		// new->value = malloc(server->value_size);
		// memcpy((product *)new->key, (const void *)key, server->key_size);
		// memcpy((product *)new->value, (const void *)value, server->value_size);
		memcpy(ht_get(server, key), value, server->value_size);
	}
		product *new = (product*)malloc(sizeof(product));
		new->key = malloc(server->key_size);
		new->value = malloc(server->value_size);
		memcpy(new->key, (const void *)key, server->key_size);
		memcpy(new->value, (const void *)value, server->value_size);
		ll_add_nth_node(server->buckets[valoare_hash], 0, (const void *)new);
		free(new);

}

char *server_retrieve(server_memory *server, char *key) {
	/* TODO 3 */
	unsigned int valoare_hash = server->hash_function(key) % server->hmax;
	ll_node_t *current;
	if (server->buckets[valoare_hash]->size == 0) {
		return NULL;
	}
	current = server->buckets[valoare_hash]->head;
	while (current->next) {
		if (!server->compare_function(key, ((product *)current->data)->key)) {
			return ((product *)current->data)->value;
		}
		current = current->next;
	}
	if (!server->compare_function(key, ((product *)current->data)->key)) {
		return ((product *)current->data)->value;
	}
	return NULL;
}

void server_remove(server_memory *server, char *key) {
	/* TODO 4 */
	if (!server_retrieve(server, key)) {
		return;
	}
	unsigned int valoare_hash = server->hash_function(key) % server->hmax;
	ll_node_t *current;
	int pozitie = 0;
	current = server->buckets[valoare_hash]->head;
	while (current->next) {
		if (!server->compare_function(key, ((product *)current->data)->key)) {
			break;
		}
		current = current->next;
		pozitie++;
	}
	ll_node_t *aux;
	aux = ll_remove_nth_node(server->buckets[valoare_hash], pozitie);	
	
	server->key_val_free_function(aux->data);
	free(aux);
}

void free_server_memory(server_memory *server) {
	/* TODO 5 */
	for (unsigned int i = 0; i < server->hmax; i++) {
		ll_free(&(server->buckets[i]));
		// free(server->buckets[i]);
	}
	free(server->buckets);
	free(server);
}
