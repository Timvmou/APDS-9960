#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES := APDS9960.cpp

# header files in this project
HEADERS := APDS9960.hpp MyServo.hpp

# other places to look for files for this project
SEARCH  := 

#TARGET := UART_PORT_UNO
SERIAL_PORT   ?= COM3

# set RELATIVE to the next higher directory 
# and defer to the appropriate Makefile.* there
RELATIVE := ..
include $(RELATIVE)/Makefile.uno
