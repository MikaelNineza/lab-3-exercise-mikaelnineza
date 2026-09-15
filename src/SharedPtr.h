#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

class ControlBlockBase {
public:
    ControlBlockBase() : refCnt(1) {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase(); // TODO: implement the destructor.

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
    // TODO: add field(s) which both control block types need to have
    long refCnt;
};

#endif
