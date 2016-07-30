//
// Created by dc on 7/8/16.
//
#include "suil.h"
#include "version.h"

namespace gap {
    const Version g_gapVersion = {
        SUIL_MAJOR_VERSION,
        SUIL_MINOR_VERSION,
        SUIL_PATCH_VERSION
    };

    const Version& gapVersion() {
        return g_gapVersion;
    }
}
