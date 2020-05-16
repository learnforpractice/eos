add_executable( pyeos main.cpp )

# set(COMPILE_OPTIONS "-I/Users/newworld/opt/anaconda3/include/python3.7m -I/Users/newworld/opt/anaconda3/include/python3.7m -Wno-unused-result -Wsign-compare -Wunreachable-code -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I/Users/newworld/opt/anaconda3/include -arch x86_64 -I/Users/newworld/opt/anaconda3/include -arch x86_64")
#set(LINK_OPTIONS -L/Users/newworld/opt/anaconda3/lib/python3.7/config-3.7m-darwin -lpython3.7m -ldl -framework CoreFoundation)

# target_include_directories(pyeos PRIVATE 
#     /Users/newworld/opt/anaconda3/include/python3.7m
# )

# target_compile_options(pyeos PRIVATE ${COMPILE_OPTIONS})
# target_link_options(pyeos PRIVATE -L/Users/newworld/opt/anaconda3/lib/python3.7/config-3.7m-darwin -lpython3.7m -ldl -framework CoreFoundation)
