env = Environment(
    CPPPATH=["#/src", "#/includes"],
    tools = ["default", "doxygen"])

AddOption('--dbg', dest='debug', action='store_true', help='enable debug symbols')
if GetOption('debug'):
    env.Append(CXXFLAGS = ['-g3', '-O0'])
else:
    env.Append(CXXFLAGS = ['-O3'])

libcashaccount, tests, test_result = env.SConscript("#/src/SConscript", exports='env')
rustbindingstest = env.SConscript("#/rustbindingstest/SConscript",
    exports=['env', 'libcashaccount'])
Depends(rustbindingstest, libcashaccount)

doxygen = env.Doxygen("Doxyfile")

format = env.Command(
    source = ".clang-format",
    target = "dummyformat",
    action = "clang-format-8 -i ./src/*.cpp ./includes/*hpp ./src/test/*.cpp ./src/test/*.h")
AlwaysBuild(format)

build_lib = [libcashaccount, tests, test_result]
build_bindings = [rustbindingstest]
all = build_lib + build_bindings + [doxygen]

env.Alias('all', all)
env.Alias('lib', build_lib)
env.Alias('bindings', build_bindings)
env.Alias('docs', doxygen)
env.Alias('format', format)

Default([build_lib, build_bindings])
