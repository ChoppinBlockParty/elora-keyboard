USER_NAME := halcyon_modules
MOUSEKEY_ENABLE = yes
SRC += tap_hold.c keylog.c
OPT_DEFS += -DELORA_KEYLOG
ifdef HLC_TFT_DISPLAY
SRC += display.c smiley.c
endif
