# Includes, LD overrides
# INC_DIRS ?= -I/mmc/usr/include -I/mmc/include
# LD_DIRS  ?= -L/mmc/usr/lib -L/mmc/lib

# Debug
EN_DEVMODE ?= 1

# Collect
LOCAL_DEFINES += -DEN_DEVMODE=$(EN_DEVMODE)
