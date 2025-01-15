#will execute makefile with more than one jobs based on
# then number of processors you have. 
N_CORES:=1
ifeq ($(OS),Windows_NT) 
	tmp=  $(strip $(shell wmic cpu get NumberOfLogicalProcessors /value))
	N_CORES := $(strip $(subst NumberOfLogicalProcessors=,, $(tmp)))
else
	OS:=$(shell uname -s)
	ifeq ($(OS),Linux)
		N_CORES:=$(shell grep -c ^processor /proc/cpuinfo)
	endif
	ifeq ($(OS),Darwin) # Assume Mac OS X
		N_CORES:=$(shell system_profiler | awk '/Number Of CPUs/{print $4}{next;}')
	endif
endif

MAKEFLAGS := --jobs=$(N_CORES)

#buffer output to avoid interleaving of output
MAKEFLAGS += --output-sync=target