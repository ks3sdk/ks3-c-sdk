/*
 * @landyliu
*/

#ifndef PANDORA_SHARED_H 
#define PANDORA_SHARED_H 

#include "mutex.h"
#include <assert.h>

namespace pandora
{
/* 
 * SimpleShared class provide simple reference count
 */
class SimpleShared
{
public:

    SimpleShared();
    SimpleShared(const SimpleShared&);

    virtual ~SimpleShared(){}

    SimpleShared& operator=(const SimpleShared&);

    void IncRef();

    void DecRef();

    int GetRef() const;

    void SetNoDelete(bool b);

private:

    int ref_;
    bool no_delete_;
};

/*
 * Shared provide simple reference cout, mainly used for smart pointer.
 * User should extends this class if you want to use sp.
 */
class Shared
{
public:

    Shared();
    Shared(const Shared&);

    virtual ~Shared()
    {
    }

    Shared& operator=(const Shared&)
    {
        return *this;
    }
    
    virtual void IncRef();
    virtual void DecRef();
    virtual int GetRef() const;
    /* 
     * Set no_delete_ flag
     * true: if ref_ > 0, do not delete the obj managed
     * false: if ref_ == 0, delete the obj managed
     * @param bool
     */
    virtual void SetNoDelete(bool);

protected:
    int ref_;
    bool no_delete_;
    Mutex mutex_;
};
}//end namespace
#endif
