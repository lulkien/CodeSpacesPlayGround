#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t m_type_size;
    uint32_t m_capacity;
    uint32_t m_size;
    void* m_data;
} CVector;

CVector cvector_new(uint32_t capacity, uint8_t type_size);
uint32_t cvector_size(CVector* self);
void cvector_resize(CVector* self, const uint32_t new_size, const void* data,
                    const uint8_t data_size);
uint32_t cvector_capacity(CVector* self);
bool cvector_empty(CVector* self);
void cvector_reserve(CVector* self, const uint32_t new_capacity);
void cvector_shrink_to_fit(CVector* self);
void* cvector_at(CVector* self, const uint32_t index);
void* cvector_front(CVector* self);
void* cvector_back(CVector* self);
void* cvector_data(CVector* self);
void cvector_insert(CVector* self, void* data, uint8_t data_size,
                    uint32_t index);
void cvector_push_back(CVector* self, void* data, uint8_t data_size);
void cvector_erase(CVector* self, const uint32_t index);
void cvector_pop_back(CVector* self);
void cvector_clear(CVector* self);

#define IMPL_CVECTOR_TYPE(TYPE, fmt)                                           \
    struct IMPL_CVECTOR_##TYPE {                                               \
        CVector (*new)(const uint32_t capacity);                               \
        uint32_t (*size)(CVector * self);                                      \
        void (*resize)(CVector * self, const uint32_t new_size, TYPE data);    \
        uint32_t (*capacity)(CVector * self);                                  \
        bool (*empty)(CVector * self);                                         \
        void (*reserve)(CVector * self, const uint32_t new_capacity);          \
        void (*shrink_to_fit)(CVector * self);                                 \
        TYPE (*at)(CVector * self, const uint32_t index);                      \
        TYPE (*front)(CVector * self);                                         \
        TYPE (*back)(CVector * self);                                          \
        void* (*data)(CVector * self);                                         \
        void (*push_back)(CVector * self, TYPE data);                          \
        void (*pop_back)(CVector * self);                                      \
        void (*insert)(CVector * self, TYPE data, uint32_t index);             \
        void (*erase)(CVector * self, uint32_t index);                         \
        void (*clear)(CVector * self);                                         \
        void (*dump)(CVector * self);                                          \
    };                                                                         \
    CVector cvector_new_##TYPE(uint32_t capacity) {                            \
        return cvector_new(capacity, sizeof(TYPE));                            \
    }                                                                          \
    void cvector_resize_##TYPE(CVector* self, const uint32_t new_size,         \
                               TYPE data) {                                    \
        cvector_resize(self, new_size, &data, sizeof(data));                   \
    }                                                                          \
    TYPE cvector_at_##TYPE(CVector* self, uint32_t index) {                    \
        return *(TYPE*)cvector_at(self, index);                                \
    }                                                                          \
    TYPE cvector_front_##TYPE(CVector* self) {                                 \
        return *(TYPE*)cvector_front(self);                                    \
    }                                                                          \
    TYPE cvector_back_##TYPE(CVector* self) {                                  \
        return *(TYPE*)cvector_back(self);                                     \
    }                                                                          \
    void cvector_push_back_##TYPE(CVector* self, TYPE data) {                  \
        cvector_push_back(self, &data, sizeof(data));                          \
    }                                                                          \
    void cvector_insert_##TYPE(CVector* self, TYPE data, uint32_t index) {     \
        cvector_insert(self, &data, sizeof(data), index);                      \
    }                                                                          \
    void cvector_dump_##TYPE(CVector* self) {                                  \
        printf("{\n    ");                                                     \
        printf("\"Capacity\": %d,\n    ", self->m_capacity);                   \
        printf("\"TypeSize\": %d,\n    ", self->m_type_size);                  \
        printf("\"Size\": %d,\n    ", self->m_size);                           \
        printf("\"Front\": ");                                                 \
        if (cvector_empty(self)) {                                             \
            printf("null");                                                    \
        } else {                                                               \
            printf(fmt, cvector_front_##TYPE(self));                           \
        }                                                                      \
        printf(",\n    \"Back\": ");                                           \
        if (cvector_empty(self)) {                                             \
            printf("null");                                                    \
        } else {                                                               \
            printf(fmt, cvector_back_##TYPE(self));                            \
        }                                                                      \
        printf(",\n    \"Data\": [");                                          \
        uint32_t idx;                                                          \
        for (idx = 0; idx < self->m_size; ++idx) {                             \
            printf("\n        ");                                              \
            printf(fmt, *(TYPE*)(self->m_data + idx * self->m_type_size));     \
            printf("%s", idx != self->m_size - 1 ? "," : "\n    ");            \
        }                                                                      \
        printf("]\n");                                                         \
        printf("}\n");                                                         \
    }                                                                          \
    struct IMPL_CVECTOR_##TYPE IVector_##TYPE = {                              \
        .new = cvector_new_##TYPE,                                             \
        .size = cvector_size,                                                  \
        .resize = cvector_resize_##TYPE,                                       \
        .capacity = cvector_capacity,                                          \
        .empty = cvector_empty,                                                \
        .reserve = cvector_reserve,                                            \
        .shrink_to_fit = cvector_shrink_to_fit,                                \
        .at = cvector_at_##TYPE,                                               \
        .front = cvector_front_##TYPE,                                         \
        .back = cvector_back_##TYPE,                                           \
        .data = cvector_data,                                                  \
        .push_back = cvector_push_back_##TYPE,                                 \
        .pop_back = cvector_pop_back,                                          \
        .insert = cvector_insert_##TYPE,                                       \
        .erase = cvector_erase,                                                \
        .clear = cvector_clear,                                                \
        .dump = cvector_dump_##TYPE,                                           \
    };
