#include "cvector.h"

CVector cvector_new(uint32_t capacity, uint8_t type_size) {
    CVector vec = {
        .m_type_size = type_size,
        .m_capacity = capacity,
        .m_size = 0,
        .m_data = NULL,
    };

    if (0 != capacity) {
        vec.m_data = malloc(capacity * type_size);
    }

    return vec;
}

uint32_t cvector_size(CVector* self) { return self->m_size; }

void cvector_resize(CVector* self, const uint32_t new_size, const void* data,
                    const uint8_t data_size) {
    assert(self->m_type_size > 0 && "Invalid type_size");
    assert(data_size == self->m_type_size && "Wrong data size");
    assert(self->m_size <= self->m_capacity && "Data overflow");

    if (new_size <= self->m_size) {
        self->m_size = new_size;
        return;
    }

    bool update_capacity = false;
    while (self->m_capacity < new_size) {
        update_capacity = true;
        self->m_capacity *= 2;
    }

    if (update_capacity) {
        self->m_data =
            realloc(self->m_data, self->m_capacity * self->m_type_size);
    }

    int i;
    for (i = 0; i < new_size - self->m_size; ++i) {
        memcpy(self->m_data + (self->m_size + i) * self->m_type_size, data,
               data_size);
    }
    self->m_size = new_size;
}

uint32_t cvector_capacity(CVector* self) { return self->m_capacity; }

bool cvector_empty(CVector* self) { return self->m_size == 0; }

void cvector_reserve(CVector* self, const uint32_t new_capacity) {
    if (new_capacity > self->m_capacity) {
        self->m_data = realloc(self->m_data, new_capacity * self->m_type_size);
        self->m_capacity = new_capacity;
    }
}

void cvector_shrink_to_fit(CVector* self) {
    assert(self->m_size <= self->m_capacity && "Data overflow");

    if (self->m_capacity == self->m_size) {
        return;
    }

    self->m_data = realloc(self->m_data, self->m_size * self->m_type_size);
    self->m_capacity = self->m_size;
}

void* cvector_at(CVector* self, const uint32_t index) {
    assert(index < self->m_size && "Index out of range");
    return self->m_data + index * self->m_type_size;
}

void* cvector_front(CVector* self) { return cvector_at(self, 0); }

void* cvector_back(CVector* self) { return cvector_at(self, self->m_size - 1); }

void* cvector_data(CVector* self) { return self->m_data; }

void cvector_insert(CVector* self, void* data, uint8_t data_size,
                    uint32_t index) {
    assert(index <= self->m_size && "Invalid index");
    assert(data_size == self->m_type_size && "Wrong data size");
    assert(self->m_size <= self->m_capacity && "Data overflow");

    bool update_capacity = false;
    if (self->m_capacity == 0) {
        self->m_capacity = 1;
        update_capacity = true;
    } else if (self->m_size == self->m_capacity) {
        self->m_capacity *= 2;
        update_capacity = true;
    }

    if (update_capacity) {
        self->m_data =
            realloc(self->m_data, self->m_capacity * self->m_type_size);
    }

    if (index != self->m_size) {
        memmove(self->m_data + (index + 1) * self->m_type_size,
                self->m_data + index * self->m_type_size,
                (self->m_size - index) * self->m_type_size);
    }

    memcpy(self->m_data + index * self->m_type_size, data, data_size);

    self->m_size += 1;
}

void cvector_push_back(CVector* self, void* data, uint8_t data_size) {
    cvector_insert(self, data, data_size, self->m_size);
}

void cvector_erase(CVector* self, const uint32_t index) {
    assert(index < self->m_size && "Invalid index");
    assert(self->m_size <= self->m_capacity && "Data overflow");

    if (index != self->m_size - 1) {
        memmove(self->m_data + index * self->m_type_size,
                self->m_data + (index + 1) * self->m_type_size,
                (self->m_size - index - 1) * self->m_type_size);
    }

    self->m_size -= 1;
}

void cvector_pop_back(CVector* self) { cvector_erase(self, self->m_size - 1); }

void cvector_clear(CVector* self) { self->m_size = 0; }