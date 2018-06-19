#ifndef __HDMAYA_ADAPTER_H__
#define __HDMAYA_ADAPTER_H__

#include <pxr/pxr.h>

#include <pxr/usd/sdf/path.h>

#include <maya/MMessage.h>

#include <vector>

#include "delegateCtx.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdMayaAdapter {
public:
    HdMayaAdapter(const SdfPath& id, HdMayaDelegateCtx* delegate);
    virtual ~HdMayaAdapter();

    const SdfPath& GetID() { return _id; }
    HdMayaDelegateCtx* GetDelegate() { return _delegate; }
    void AddCallback(MCallbackId callbackId);
    virtual VtValue Get(const TfToken& key);

    virtual void CreateCallbacks() = 0;
    virtual void MarkDirty(HdDirtyBits dirtyBits) = 0;
    virtual void Populate() = 0;
protected:
    SdfPath _id;
    HdMayaDelegateCtx* _delegate;

    std::vector<MCallbackId> _callbacks;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // __HDMAYA_ADAPTER_H__
