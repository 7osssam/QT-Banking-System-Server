
# CMakeLists.txt file for test root directory


#============================================================================
# Alternatively, We can use FetchContent module to download GoogleTest
#============================================================================
## Include FetchContent module
# include(FetchContent)
#
## Add GoogleTest
#FetchContent_Declare(
#	googletest
#	GIT_REPOSITORY https://github.com/google/googletest
#	GIT_TAG v1.14.0
#	GIT_SHALLOW TRUE
#)
#
## Fetch GoogleTest content
#FetchContent_Populate(googletest)
##============================================================================

# this will include a git submodule in a directory (relative to the root CMakeLists.txt file)
add_git_submodule(tests/lib/googletest https://github.com/google/googletest)

# =============================================================================
# Testing Modules (unit tests)
# =============================================================================


#add_subdirectory(test_suite)  # Test suite Template

add_subdirectory(handleLogin)
add_subdirectory(handleGetAccountNumber)
add_subdirectory(handleGetTransactionsHistory)
add_subdirectory(handleMakeTransaction)
add_subdirectory(handleCreateNewUser)
add_subdirectory(handleGetBalance)
add_subdirectory(handleGetDatabase)
#add_subdirectory(RequestManager)


############# etc....

