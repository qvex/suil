//
// Created by dc on 6/10/16.
//

#ifndef GAR_ECHO_H_H
#define GAR_ECHO_H_H

#include "handlers.h"
extern "C" {
    // Use classes from gap
    using namespace gap;

    AppHandler *Echo_main() {
        GAP_Dbg("%s application create\n", __FUNCTION__);
        return new EchoApp();
    }

    void Echo_exit() {
        GAP_Info("%s application exit\n", __FUNCTION__);
    }

}


#endif //GAR_ECHO_H_H
