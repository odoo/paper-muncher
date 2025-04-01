#include "bundle.h"

#include "_embed.h"

namespace marK::Pkg {

Res<BundleInfo> currentBundle() {
    auto id = try$(_Embed::currentBundle());
    return Ok(BundleInfo{id});
}

Res<Vec<BundleInfo>> installedBundles() {
    Vec<BundleInfo> bundles;
    auto ids = try$(_Embed::installedBundles());
    for (auto& id : ids) {
        bundles.pushBack({id});
    }
    return Ok(bundles);
}

} // namespace marK::Pkg
