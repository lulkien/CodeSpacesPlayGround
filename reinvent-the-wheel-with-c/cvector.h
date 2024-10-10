#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t size;
    uint32_t capacity;
    uint32_t type_size;
    uint8_t* data;
} CVector;

/**
 * Construct vector
 */
#define DECLARE_CVECTOR_CONSTRUCTOR(TYPE)                                      \
    CVector cvector_new_##TYPE(uint32_t capacity) {                            \
        assert(sizeof(TYPE) > 0 && "Invalid type_size");                       \
        CVector vec;                                                           \
        vec.size = 0;                                                          \
        vec.capacity = capacity;                                               \
        vec.type_size = sizeof(TYPE);                                          \
        if (capacity != 0) {                                                   \
            vec.data = (uint8_t*)malloc(vec.capacity * vec.type_size);         \
        } else {                                                               \
            vec.data = NULL;                                                   \
        }                                                                      \
        return vec;                                                            \
    }

/**
 * Return vector size
 */
uint32_t cvector_size(CVector* vec) { return vec->size; }

/**
 * Change vector size
 */
void cvector_resize(CVector* vec, const uint32_t new_size, const void* data,
                    const uint32_t data_size) {
    assert(vec->type_size > 0 && "Invalid type_size");
    assert(data_size == vec->type_size && "Wrong data size");
    assert(vec->size <= vec->capacity && "Data overflow");
    if (new_size <= vec->size) {
        vec->size = new_size;
    } else {
        bool update_capacity = false;
        while (vec->capacity < new_size) {
            update_capacity = true;
            vec->capacity *= 2;
        }
        if (update_capacity) {
            uint8_t* old_data = vec->data;
            vec->data = (uint8_t*)malloc(vec->capacity * vec->type_size);
            memcpy(vec->data, old_data, vec->size * vec->type_size);
            int i;
            for (i = 0; i < new_size - vec->size; ++i) {
                memcpy(vec->data + (vec->size + i) * vec->type_size, data,
                       data_size);
            }
            vec->size = new_size;
            free(old_data);
        } else {
            int i;
            for (i = 0; i < new_size - vec->size; ++i) {
                memcpy(vec->data + (vec->size + i) * vec->type_size, data,
                       data_size);
            }
            vec->size = new_size;
        }
    }
}

#define DECLARE_CVECTOR_RESIZE(TYPE)                                           \
    void cvector_resize_##TYPE(CVector* vec, uint32_t new_size, TYPE value) {  \
        cvector_resize(vec, new_size, (void*)&value, sizeof(TYPE));            \
    }

/**
 * Return size of allocated storage capacity
 */
uint32_t cvector_capacity(CVector* vec) { return vec->capacity; }

/**
 * Test whether vector is empty
 */
bool cvector_empty(CVector* vec) { return vec->size == 0; }

/**
 * Request a change in capacity
 */
void cvector_reserve(CVector* vec, uint32_t new_capacity) {
    assert(vec->size <= vec->capacity && "Data overflow");
    if (new_capacity <= vec->capacity) {
        return;
    }

    uint8_t* old_data = vec->data;
    vec->capacity = new_capacity;
    vec->data = (uint8_t*)malloc(vec->capacity * vec->type_size);
    memcpy(vec->data, old_data, vec->size * vec->type_size);
    free(old_data);
}

/**
 * Shrink to fit
 */
void cvector_shrink_to_fit(CVector* vec) {
    assert(vec->size <= vec->capacity && "Data overflow");
    if (vec->size == vec->capacity) {
        return;
    }

    uint8_t* old_data = vec->data;
    vec->data = (uint8_t*)malloc(vec->size * vec->type_size);
    memcpy(vec->data, old_data, vec->size * vec->type_size);
    vec->capacity = vec->size;
    free(old_data);
}

/**
 * Access element
 */
uint8_t* cvector_at(CVector* vec, const uint32_t index) {
    assert((index >= 0 && index < vec->size) && "Index out of range.");
    return vec->data + index * vec->type_size;
}

#define DECLARE_CVECTOR_ACCESS(TYPE)                                           \
    TYPE cvector_at_##TYPE(CVector* vec, uint32_t index) {                     \
        assert(vec->type_size == sizeof(TYPE) && "Invalid type");              \
        return *(TYPE*)cvector_at(vec, index);                                 \
    }

/**
 * Access first element
 */
uint8_t* cvector_front(CVector* vec) { return vec->data; }

#define DECLARE_CVECTOR_FRONT(TYPE)                                            \
    TYPE cvector_front_##TYPE(CVector* vec) {                                  \
        return *(TYPE*)cvector_front(vec);                                     \
    }

/**
 * Access last element
 */
uint8_t* cvector_back(CVector* vec) {
    return vec->data + (vec->size - 1) * vec->type_size;
}

#define DECLARE_CVECTOR_BACK(TYPE)                                             \
    TYPE cvector_back_##TYPE(CVector* vec) { return *(TYPE*)cvector_back(vec); }

/**
 * Access data
 */
uint8_t* cvector_data(CVector* vec) { return vec->data; }

#define DECLARE_CVECTOR_DATA(TYPE)                                             \
    TYPE* cvector_data_##TYPE(CVector* vec) { return (TYPE*)cvector_data(vec); }

/**
 * Inserts element
 */
void cvector_insert(CVector* vec, const uint32_t index, const void* data,
                    const uint32_t data_size) {
    assert((index >= 0 && index <= vec->size) && "Invalid index");
    assert(vec->type_size > 0 && "Invalid type_size");
    assert(data_size == vec->type_size && "Wrong data size");
    assert(vec->size <= vec->capacity && "Data overflow");
    bool update_capacity = false;
    if (vec->capacity == 0) {
        vec->capacity = 1;
        update_capacity = true;
    } else if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        update_capacity = true;
    }

    if (update_capacity) {
        uint8_t* old_data = vec->data;
        vec->data = (uint8_t*)malloc(vec->capacity * vec->type_size);
        memcpy(vec->data, old_data, vec->size * vec->type_size);
        free(old_data);
    }

    if (index != vec->size) {
        memmove(vec->data + (index + 1) * vec->type_size,
                vec->data + index * vec->type_size,
                (vec->size - index) * vec->type_size);
    }
    memcpy(vec->data + index * vec->type_size, data, data_size);

    vec->size++;
}

#define DECLARE_CVECTOR_INSTERT(TYPE)                                          \
    void cvector_insert_##TYPE(CVector* vec, const uint32_t index,             \
                               const TYPE value) {                             \
        cvector_insert(vec, index, (void*)&value, sizeof(TYPE));               \
    }

/**
 * Add element at the end
 */
void cvector_push_back(CVector* vec, const void* data, uint32_t data_size) {
    cvector_insert(vec, vec->size, data, data_size);
}

#define DECLARE_CVECTOR_PUSH(TYPE)                                             \
    void cvector_push_back_##TYPE(CVector* vec, const TYPE value) {            \
        cvector_push_back(vec, (void*)&value, sizeof(TYPE));                   \
    }

/**
 * Erase elements
 */
void cvector_erase(CVector* vec, const uint32_t index) {
    assert((index >= 0 && index < vec->size) && "Invalid index");
    assert(vec->type_size > 0 && "Invalid type_size");
    assert(vec->size <= vec->capacity && "Data overflow");

    if (index != vec->size - 1) {
        memmove(vec->data + index * vec->type_size,
                vec->data + (index + 1) * vec->type_size,
                (vec->size - index - 1) * vec->type_size);
    }

    vec->size--;
}

/**
 * Delete last element
 */
void cvector_pop_back(CVector* vec) { cvector_erase(vec, vec->size - 1); }

/**
 * Swap content
 */
void cvector_swap(CVector* vec1, CVector* vec2) {
    uint32_t tmp_size = vec1->size;
    uint32_t tmp_cap = vec1->capacity;
    uint32_t tmp_type_size = vec1->type_size;
    uint8_t* tmp_data = vec1->data;

    vec1->size = vec2->size;
    vec1->capacity = vec2->capacity;
    vec1->type_size = vec2->type_size;
    vec1->data = vec2->data;

    vec2->size = tmp_size;
    vec2->capacity = tmp_cap;
    vec2->type_size = tmp_type_size;
    vec2->data = tmp_data;
}

/**
 * Clear content
 */
void cvector_clear(CVector* vec) { vec->size = 0; }

void cvector_free(CVector* vec) {
    if (vec->data) {
        free(vec->data);
    }
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->type_size = 0;
}

#define DECLARE_CVECTOR_PRINT(TYPE, fmt)                                       \
    void cvector_print_##TYPE(CVector* vec) {                                  \
        uint32_t i;                                                            \
        printf("[ ");                                                          \
        for (i = 0; i < vec->size; ++i) {                                      \
            printf(fmt, *(TYPE*)(vec->data + i * vec->type_size));             \
            printf(", ");                                                      \
        }                                                                      \
        printf("]\n");                                                         \
    }

#define IMPL_CVECTOR(TYPE, fmt)                                                \
    DECLARE_CVECTOR_PRINT(TYPE, fmt)                                           \
    DECLARE_CVECTOR_CONSTRUCTOR(TYPE)                                          \
    DECLARE_CVECTOR_RESIZE(TYPE)                                               \
    DECLARE_CVECTOR_ACCESS(TYPE)                                               \
    DECLARE_CVECTOR_DATA(TYPE)                                                 \
    DECLARE_CVECTOR_PUSH(TYPE)                                                 \
    DECLARE_CVECTOR_INSTERT(TYPE)
