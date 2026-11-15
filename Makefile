# Transformers Brawler - Raylib / C++17
# Preferred environment: MSYS2 UCRT64 with raylib installed through pacman.
# Fallback: standalone raylib distribution at C:/raylib.

CXX ?= g++
PKG_CONFIG ?= pkg-config

RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib 2>/dev/null)

ifeq ($(strip $(RAYLIB_LIBS)),)
    # MSYS2 UCRT64 package fallback when pkg-config is unavailable.
    ifneq ($(wildcard C:/msys64/ucrt64/include/raylib.h),)
        RAYLIB_CFLAGS = -I"C:/msys64/ucrt64/include"
        RAYLIB_LIBS = -L"C:/msys64/ucrt64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
    else
        # Standalone raylib distribution fallback.
        RAYLIB_PATH ?= C:/raylib/raylib/src
        RAYLIB_CFLAGS = -I"$(RAYLIB_PATH)"
        RAYLIB_LIBS = -L"$(RAYLIB_PATH)" -lraylib -lopengl32 -lgdi32 -lwinmm
    endif
else
    # raylib's pkg-config file supplies its required link dependencies.
    RAYLIB_LIBS += -lopengl32 -lgdi32 -lwinmm
endif

CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
CXXFLAGS += $(RAYLIB_CFLAGS) -Isrc
LDFLAGS += $(RAYLIB_LIBS)

TARGET = bin/transformers_brawler.exe

SRCS = src/main.cpp \
       src/core/Game.cpp \
       src/core/CameraManager.cpp \
       src/core/SoundSystem.cpp \
       src/core/SaveSystem.cpp \
       src/effects/ParticleSystem.cpp \
       src/world/ParallaxBackground.cpp \
       src/world/Arena.cpp \
       src/combat/CombatSystem.cpp \
       src/entities/Projectile.cpp \
       src/entities/Player.cpp \
       src/entities/Enemy.cpp \
       src/entities/Boss.cpp \
       src/ui/HUD.cpp \
       src/ui/MenuSystem.cpp \
       src/ui/LevelSelectUI.cpp \
       src/ui/SettingsUI.cpp \
       src/ui/MissionCompletionUI.cpp

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo Build complete: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
