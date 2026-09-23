#include "core/conversation.h"
#include <stdexcept>
#include <utility>

Conversation::Conversation() : data_(new Message[2]), size_(0), capacity_(2) {}

Conversation::~Conversation() {
    delete[] data_;
}

Conversation::Conversation(const Conversation& other) 
    : data_(new Message[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}

Conversation& Conversation::operator=(const Conversation& other) {
    if (this != &other) {
        delete[] data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        data_ = new Message[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    return *this;
}

Conversation::Conversation(Conversation&& other) noexcept 
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

void Conversation::append(Message m) {
    if (size_ == capacity_) {
        size_t new_capacity = capacity_ * 2;
        Message* new_data = new Message[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    data_[size_++] = std::move(m);
}

const Message& Conversation::at(size_t index) const {
    if (index >= size_) throw std::out_of_range("Index out of bounds");
    return data_[index];
}

size_t Conversation::size() const noexcept {
    return size_;
}

const Message* Conversation::begin() const noexcept {
    return data_;
}

const Message* Conversation::end() const noexcept {
    return data_ + size_;
}