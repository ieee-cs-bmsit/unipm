# Tests CMakeLists.txt

# Test executable
add_executable(test_os_detector
    test_os_detector.cpp
)

target_link_libraries(test_os_detector PRIVATE
    unipm_lib
)

add_test(NAME OSDetectorTest COMMAND test_os_detector)

# Add more test executables as needed
add_executable(test_resolver
    test_resolver.cpp
)

target_link_libraries(test_resolver PRIVATE
    unipm_lib
)

add_test(NAME ResolverTest COMMAND test_resolver)
