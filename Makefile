# === PROJECT NAME ===
PROJECT_NAME = retools

# === PATHS ===

BUILD_DIR = build
BIN_DIR   = bin

UNITY_DIR   = tests/Unity
UNITY_BUILD = $(BUILD_DIR)/unity

SRC_DIR = src

# === TARGETS ===

.PHONY: rtw rcat rhead rtail unity clbuild

rtw:
	$(MAKE) -C src/rtw

rcat:
	$(MAKE) -C src/rcat

rhead:
	$(MAKE) -C src/rhead

rtail:
	$(MAKE) -C src/rtail

unity: $(UNITY_BUILD)
	cd $(UNITY_BUILD) && cmake ../../$(UNITY_DIR)
	cd $(UNITY_BUILD) && make
	find build/unity -mindepth 1 ! -name 'libunity.a' -exec rm -rf {} +

clbuild:
	rm -rf $(BIN_DIR)/*
	find $(BUILD_DIR) -mindepth 1 -maxdepth 1 -not -name 'unity' -exec rm -rf {} +

clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(BIN_DIR)/*

# ===RULES===

$(UNITY_BUILD):
	mkdir -p $@
