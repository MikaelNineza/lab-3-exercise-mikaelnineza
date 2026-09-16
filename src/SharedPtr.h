#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

#include <cassert>
#include <utility>
class ControlBlockBase {
public:
    ControlBlockBase() : refCnt(1) {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() {
        assert(refCnt == 0);
    }

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        assert(refCnt > 0);
        refCnt += 1;
        return refCnt;
    }

    long decrement()
    {
        assert(refCnt > 0);
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
        template <class U> friend class SharedPtr;
        SharedPtr() : storedPtr(nullptr), cBlock(nullptr) {}

        SharedPtr(T* p) : storedPtr(p) {
            cBlock = new ControlBlock<T>(p);
        }

        template <typename U> SharedPtr(const SharedPtr<U>& other, T* storedPtr) : storedPtr(storedPtr), cBlock(other.cBlock) {
            if (cBlock) {
                cBlock->increment();
            }
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
            // if (cBlock) {
            //     if (cBlock->decrement() < 1) {
            //         delete cBlock;
            //     }
            //     cBlock = nullptr;
            //     storedPtr = nullptr;
            // }
            SharedPtr<T>().swap(*this);
        }

        void reset(T* other) {
            SharedPtr<T>(other).swap(*this);
        }

        void swap(SharedPtr<T>& other) {
            if (storedPtr == other.storedPtr) {
                return;
            }
            T* tempT = storedPtr;
            ControlBlockBase* tempBlock = cBlock;
            storedPtr = other.storedPtr;
            cBlock = other.cBlock;
            other.storedPtr = tempT;
            other.cBlock = tempBlock;
        }

        long useCount() const {
            return cBlock->refCount();
        }

    private:
        template <class U, class... Args> friend SharedPtr<U> makeShared(Args&&... args);
        T* storedPtr;
        ControlBlockBase* cBlock;

        // Bonus
        SharedPtr(T* p, ControlBlockBase* conBlock) : storedPtr(p), cBlock(conBlock) {}
};

template <class T, class... Args> SharedPtr<T> makeSharedBasic(Args&&... args) {
    SharedPtr<T> ptr(new T(std::forward<Args>(args)...));
    return ptr;
}


template <class T>
class ControlBlockEmbedded : public ControlBlockBase {
    public:
    template <class... Args>
    ControlBlockEmbedded(Args&&... args) : obj(std::forward<Args>(args)...) {}
    
    ~ControlBlockEmbedded() override {}
    
    void* managedAddress() override {
        return &obj;
    }
    
    private:
    T obj;
};

template <class T, class... Args> SharedPtr<T> makeShared(Args&&... args) {
    ControlBlockBase* cBlock = new ControlBlockEmbedded<T>(std::forward<Args>(args)...);
    return  SharedPtr<T>((T*)cBlock->managedAddress(), cBlock);
}

#endif
