project(tactile-log-lib CXX)

add_library(tactile-log STATIC)
add_library(tactile::log ALIAS tactile-log)

target_sources(tactile-log
               PRIVATE
               "src/file_log_sink.cpp"
               "src/logger.cpp"
               "src/terminal_log_sink.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/log/file_log_sink.hpp"
               "inc/tactile/log/log_level.hpp"
               "inc/tactile/log/log_sink.hpp"
               "inc/tactile/log/logger.hpp"
               "inc/tactile/log/terminal_log_sink.hpp"
               )

tactile_prepare_target(tactile-log)

target_link_libraries(tactile-log
                      PUBLIC
                      tactile::base
                      )
