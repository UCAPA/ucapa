#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T17:50:41
#
# This project build the UCAPA lib and also all 
# the examples apps.
#
#-------------------------------------------------


TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = ucapa \
          navigator \
          derivate

ucapa.file = ucapa.pro

navigator.file = examples/navigator/Navigator.pro
navigator.depends = ucapa

derivate.file = examples/derivate/Derivate.pro
derivate.depends = ucapa
