cmake_minimum_required(VERSION 3.0)

include(GapSetup)

set(echo_ADDITIONAL_SOURCES "")
# Initialize target echo
Gap_InitializeTarget(echo)

set(echo_ADDITIONAL_LIBRARIES "")
# Load target dependencies
Gap_LoadDependecies(echo)

# Add the target
set(echo_ADDITIONAL_DEFINES "")
Gap_AddTarget(echo)

# Finilize the target
Gap_FinalizeTarget(echo)