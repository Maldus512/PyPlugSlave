import os

SRC_DIR = 'src'
BUILD_DIR = 'build'
TOOLCHAIN = "arm-none-eabi-"
ELF = "pyplugslave.elf"
BIN = "pyplugslave.bin"
LIBOPENCM3 = "components/libopencm3"
LIBOPENCM3_ARCHIVE = "{}/lib/libopencm3_stm32l0.a".format(LIBOPENCM3)
CPUFLAGS = ["-mcpu=cortex-m0", "-mthumb"]
CFLAGS = ["-Wall", "-Wextra", "-g3", "-O0",  # "-MD",
          "-DSTM32L0", "-I{}include".format(LIBOPENCM3)] + CPUFLAGS
LDSCRIPT = "{}/lib/stm32/l0/stm32l0xx8.ld".format(LIBOPENCM3)
LDFLAGS = ["-nostartfiles", "-L{}/lib".format(
    LIBOPENCM3), "-Wl,-T,{}".format(LDSCRIPT)] + CPUFLAGS
LDLIBS = ["-lopencm3_stm32l0", "-lc", "-lm", "-lnosys"]

# Creates a Phony target


def PhonyTargets(
        target,
        action,
        depends,
        env=None,
):
    if not env:
        env = DefaultEnvironment()
    t = env.Alias(target, depends, action)
    env.AlwaysBuild(t)


externalEnvironment = {}
if 'PATH' in os.environ.keys():
    externalEnvironment['PATH'] = os.environ['PATH']
if 'LD_LIBRARY_PATH' in os.environ.keys():
    externalEnvironment['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH']


env_options = {
    "ENV": externalEnvironment,
    "CC": "{}gcc".format(TOOLCHAIN),
    "CPPPATH": [SRC_DIR, "{}/include".format(LIBOPENCM3), "./components/generic_embedded_libs", "./components/I2C"],
    "CCFLAGS": CFLAGS,
    "LINKFLAGS": LDFLAGS,
    "LIBS": LDLIBS
}

#VariantDir(BUILD_DIR, SRC_DIR)

sources = Glob("{}/*.c".format(SRC_DIR))
sources += Glob("{}/**/*.c".format(SRC_DIR))
sources += Glob("components/generic_embedded_libs/generic/circularbuffer/*.c")
sources += Glob("components/I2C/common/*.c")
sources += Glob("components/I2C/MCP9800/*.c")
sources += Glob("components/I2C/EEPROM/*.c")

env = Environment(**env_options)

lib = env.Command(LIBOPENCM3_ARCHIVE, None, "cd {} && make".format(LIBOPENCM3))
env.NoClean(lib)
p = env.Program(ELF, [sources, lib])
env.Command(BIN, p, "{}objcopy -O binary {} {}".format(TOOLCHAIN, ELF, BIN))
env.Default(BIN)

PhonyTargets("flash", "st-flash write {} 0x8000000".format(BIN), BIN, env)
PhonyTargets(
    "openocd", "openocd -f interface/stlink-v2.cfg -f target/stm32l0.cfg", BIN, env)
PhonyTargets("libclean", "cd {} && make clean".format(LIBOPENCM3), None)
