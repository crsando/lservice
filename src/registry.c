#include "registry.h"
#include "log.h"

registry_t * registry_put(registry_t ** reg, const char * key, void * ptr);
registry_t * registry_del(registry_t ** reg, const char * key);

registry_t * registry_get(registry_t ** reg, const char * key) {
	// log_debug("registry_get %d | %d | %s", reg, &reg, key);
	registry_t * s = NULL;
	HASH_FIND_STR(*reg, key, s);
	// log_debug("registry_get result %d", s);
	return s;
}
registry_t * registry_put(registry_t ** reg, const char * key, void * ptr) {
	// log_debug("ctp_reg_put | req_id: %d | size: %u", req_id, size);
	registry_t * p = NULL;
	p = registry_get(reg, key);
	if(p) {
		log_debug("registry_put | key duplicate | key : %s", key);
		return p;
	}

	p = (registry_t*)malloc(sizeof(registry_t));
    memset(p->key, 0, sizeof(char) * 32);
    strcpy(p->key, key);
	p->ptr = ptr;

	HASH_ADD_STR(*reg, key, p);

	return p;
}

registry_t * registry_del(registry_t ** reg, const char * key) {
	// log_debug("ctp_reg_del | req_id: %d", req_id);
	registry_t * p = NULL;
	p = registry_get(reg, key);
	if (p) {
		HASH_DEL(*reg, p);
	}
	free(p);
	return NULL;
}