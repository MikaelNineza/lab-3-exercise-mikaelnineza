#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

class ControlBlockBase {
public:
    ControlBlockBase() : refCnt(1) {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() {
        if (refCnt < 1) {
            delete this;
        }
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
            deletePtr();
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
            other.reset();
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
                other.reset();
            }
            return *this;
        }

        bool operator==(const SharedPtr<T>& other) const {return storedPtr == other.storedPtr;}
        T& operator*() const {return *storedPtr;}
        T* operator->() const {return storedPtr;}
        operator bool() const {return storedPtr != nullptr;}

        T* get() const { return storedPtr;}

        void reset() {
            deletePtr();
        }

        void reset(T* other) {
            swap(new SharedPtr(other));
        }

        void swap(SharedPtr<T>& other) {
            if (this == other) {
                return;
            }
            T* tempT = storedPtr;
            ControlBlock tempBlock = cBlock;
            storedPtr = other.storedPtr;
            cBlock = other.cBlock;
            other.storedPtr = tempT;
            other.cBlock = tempBlock;
        }

        long useCount() {
            return cBlock->refCount();
        }

        bool deletePtr() {
            if (cBlock) {
                cBlock->decrement();
                if (cBlock->refCount() < 1) {
                    delete cBlock;
                    cBlock = nullptr;
                    storedPtr = nullptr;
                    return true;
                }
            }
            return false;
        }



    private:
        T* storedPtr;
        ControlBlockBase* cBlock;
};

#endif
