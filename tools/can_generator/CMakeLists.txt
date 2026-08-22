add_executable(can_generator
    main.cpp
)

target_link_libraries(can_generator
    PRIVATE
        Qt6::Core
        Qt6::SerialBus
)