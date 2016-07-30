//
// Created by dc on 7/8/16.
//

#ifndef SUIL_VERSION_H
#define SUIL_VERSION_H

#include <inttypes.h>
#include <string>

namespace gap {

    struct Version {
        int16_t         major;
        int16_t         minor;
        int32_t         patch;
    };

    const Version& gapVersion();
}

#endif //SUIL_VERSION_H
