#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

#include <iostream>
#include <stdio.h>
#include <utility>
#include <cassert>
class ControlBlockBase {
public:
    ControlBlockBase() : refCnt(1) {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() {
    }

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        refCnt += 1;
        return refCnt;
    }

    long decrement()
    {
        refCnt -= 1;
        return refCnt;
    }

    long refCount() const
    {
        return refCnt;
    }

private:
    long refCnt;
};

template <class T>
class ControlBlock: public ControlBlockBase {
    public:
        ControlBlock(T* p = nullptr) : storedPtr(p) {}

        ~ControlBlock() override {
            delete storedPtr;
            storedPtr = nullptr;
        }

        void* managedAddress() override {
            return storedPtr;
        }

    private:
        T* storedPtr;
};

template <class T>
class SharedPtr {
    public:
        SharedPtr() : storedPtr(nullptr), cBlock(nullptr) {}

        SharedPtr(T* p = nullptr) : storedPtr(p) {
            cBlock = new ControlBlock<T>(p);
        }

        template <typename U> SharedPtr(const SharedPtr<U>& other, T* storedPtr) {
            this->storedPtr = storedPtr;

        }

        ~SharedPtr() {
            if (cBlock) {
                if (cBlock->decrement() < 1) {
                    delete cBlock;
                    storedPtr = nullptr;
                }
            }
        }

        // copy
        SharedPtr(const SharedPtr<T>& other) {
            cBlock = other.cBlock;
            storedPtr = other.storedPtr;
            cBlock->increment();
        }

        // move
        SharedPtr(SharedPtr<T>&& other) {
            cBlock = other.cBlock;
            storedPtr = other.storedPtr;
            other.cBlock = nullptr;
                other.storedPtr = nullptr;
        }
        // copy operator
        SharedPtr<T>& operator=(const SharedPtr<T>& other) {
            if (this != &other) {
                reset();
                cBlock = other.cBlock;
                storedPtr = other.storedPtr;
                cBlock->increment();
            }
            return *this;
        };
        // move operator
        SharedPtr<T>& operator=(SharedPtr<T>&& other) {
            if (this != &other) {
                reset();
                cBlock = other.cBlock;
                storedPtr = other.storedPtr;
                other.cBlock = nullptr;
                other.storedPtr = nullptr;
            }
            return *this;
        }

        bool operator==(const SharedPtr<T>& other) const {return storedPtr == other.storedPtr;}
        T& operator*() const {return *storedPtr;}
        T* operator->() const {return storedPtr;}
        operator bool() const {return storedPtr != nullptr;}

        T* get() const { return storedPtr;}

        void reset() {
            if (cBlock) {
                if (cBlock->decrement() < 1) {
                    delete cBlock;
                    storedPtr = nullptr;
                }
            }
        }

        void reset(T* other) {
            swap(new SharedPtr(other));
        }

        void swap(SharedPtr<T>& other) {
            if (this == other) {
                return;
            }
            T* tempT = storedPtr;
            ControlBlock<T> tempBlock = cBlock;
            storedPtr = other.storedPtr;
            cBlock = other.cBlock;
            other.storedPtr = tempT;
            other.cBlock = tempBlock;
        }

        long useCount() {
            return cBlock->refCount();
        }

    private:
        T* storedPtr;
        ControlBlockBase* cBlock;
};

#endif
