#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

# Backend data structure
PEARL_DS = LIST
#PEARL_DS = CCCG


# Additional include paths needed for backend data structure
ifeq ($(PEARL_DS),CCCG)
  PEARL_DS_INC = -DHAVE_CONFIG_H -I$(HOME)/local/cccg/include
endif
