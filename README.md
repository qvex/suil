# suil
[suil](https://github.com/suilteam/suil/wiki/suil) is a lightweight C++ RESTful Web API framwork and server developed for `*nix` operating system.

```C++
#include <gap/gap.h>

#include "echo_handler.h"

void EchoHandler::onStart(gap::Gap::Ptr app) {
    Router& r = gap::UseSystemRouter(app);
    
    r.route("/api/echo/{name:string}")
      .methods(gap::HttpMethod::GET)
      ([](const gap::HttpRequest& req, gap::HttpResponse& res, const gap::RouteParams& p) {
          std::string name = "Hello " + p["name"].s() + ", ";
          res.write(name);
          res.end(200, req.bodyCopy());
      });
      
    GAP_route(r, "/", gap::HttpMethod::POST | gap::HttpMethod::PUT)
    ([](const gap::HttpRequest& req, gap::HttpResponse& res, const gap::RouteParams& /*p*/) {
        res.end(200, "Hello World!");
    });
}
```

###### Requirements
There is a [section](https://github.com/suilteam/suil/wiki/suil) on the wiki on how to install all the required build dependencies on raspberry pi, the same procedure applies and has been tested on Ubuntu. The following are the minimal requirements for a sucessful build.
* [`Cmake`](https://cmake.org/)`, cmake >= 3.0`
* [`Boost Libaries`](http://www.boost.org/doc/libs/)`, >= 1.60`
* `gnu make, gcc toolchain with support for c++11`
* `libssl (Only required when building with SSL support)`
* `pthreads`

## `gatls` - gap tools
Gliese application tool is a cli component of suil used to manipulate gliese applications (gaps). The tool can be used to create  a project template which implements a simple hello gap.

#### `gatls create app`

This command will create a new project called `app` which is ready to be built and servered. The created project has an application configuration (runtime config), C++ source files and a `CMakeLists.txt` file for building the application. The following options are available
```
  --no-ssl : application does not use ssl
  --name=[name] : the name to be given to the server
```

#### `gatls build`
```bash
[dc@suil : / ] cd app
[dc@suil : / ] gatls build
```
This command will build the project (assuming app is a gap) and create a bin folder which has the gap binary and the runtime configuration file. Gatls build uses cmake under the hood to find dependencies, the functionality of this feature is based on the user not breaking the generated CMakeLists.txt. Gatls build supports passing arguments to cmake in the form of defines, for example
```bash
[dc@suil : / ] gatls build -DCMAKE_BUILD_TYPE=Debug
```
By default gatls and the generated CMakeLists.txt recognize the following arguments
```
  --flavor=[flavor] : The build flavor, e.g Debug or Release
  --gap-dir=[path]  : This is the path to include in search for gap library, e.g ./lib (if gap binaries are installed in ./lib)
```

#### `gatls run`
`gatls run` is used to run a gap using a debug gap runner (gar) embedded onto gatls. The directory on which this is being called should be a gap directory. Running a gap this way assumes the application has already been built using `gatls build`. The instance can be stopped by `Ctrl-C`.
