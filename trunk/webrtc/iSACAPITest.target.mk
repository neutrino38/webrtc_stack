# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := iSACAPITest
DEFS_Debug := \
	'-DWEBRTC_SVNREVISION="Unavailable(issue687)"' \
	'-D_FORTIFY_SOURCE=2' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_NSS=1' \
	'-DENABLE_ONE_CLICK_SIGNIN' \
	'-DGTK_DISABLE_SINGLE_INCLUDES=1' \
	'-DENABLE_REMOTING=1' \
	'-DENABLE_WEBRTC=1' \
	'-DENABLE_PEPPER_THREADING' \
	'-DENABLE_CONFIGURATION_POLICY' \
	'-DENABLE_INPUT_SPEECH' \
	'-DENABLE_NOTIFICATIONS' \
	'-DENABLE_GPU=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DUSE_SKIA=1' \
	'-DENABLE_TASK_MANAGER=1' \
	'-DENABLE_WEB_INTENTS=1' \
	'-DENABLE_EXTENSIONS=1' \
	'-DENABLE_PLUGIN_INSTALLATION=1' \
	'-DENABLE_PROTECTOR_SERVICE=1' \
	'-DENABLE_SESSION_SERVICE=1' \
	'-DENABLE_THEMES=1' \
	'-DENABLE_BACKGROUND=1' \
	'-DENABLE_AUTOMATION=1' \
	'-DENABLE_LANGUAGE_DETECTION=1' \
	'-DENABLE_PRINTING=1' \
	'-DENABLE_CAPTIVE_PORTAL_DETECTION=1' \
	'-DWEBRTC_LOGGING' \
	'-DWEBRTC_LINUX' \
	'-DWEBRTC_THREAD_RR' \
	'-D__STDC_CONSTANT_MACROS' \
	'-D__STDC_FORMAT_MACROS' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \
	'-D_DEBUG'

# Flags passed to all source files.
CFLAGS_Debug := \
	-fstack-protector \
	--param=ssp-buffer-size=4 \
	-Werror \
	-pthread \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-fvisibility=hidden \
	-pipe \
	-fPIC \
	-Wextra \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-m32 \
	-mmmx \
	-march=pentium4 \
	-msse2 \
	-mfpmath=sse \
	-O0 \
	-g

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-fno-rtti \
	-fno-threadsafe-statics \
	-fvisibility-inlines-hidden \
	-Wsign-compare \
	-Woverloaded-virtual

INCS_Debug := \
	-Iwebrtc \
	-I. \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/test \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/interface \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/util

DEFS_Release := \
	'-DWEBRTC_SVNREVISION="Unavailable(issue687)"' \
	'-D_FORTIFY_SOURCE=2' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_NSS=1' \
	'-DENABLE_ONE_CLICK_SIGNIN' \
	'-DGTK_DISABLE_SINGLE_INCLUDES=1' \
	'-DENABLE_REMOTING=1' \
	'-DENABLE_WEBRTC=1' \
	'-DENABLE_PEPPER_THREADING' \
	'-DENABLE_CONFIGURATION_POLICY' \
	'-DENABLE_INPUT_SPEECH' \
	'-DENABLE_NOTIFICATIONS' \
	'-DENABLE_GPU=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DUSE_SKIA=1' \
	'-DENABLE_TASK_MANAGER=1' \
	'-DENABLE_WEB_INTENTS=1' \
	'-DENABLE_EXTENSIONS=1' \
	'-DENABLE_PLUGIN_INSTALLATION=1' \
	'-DENABLE_PROTECTOR_SERVICE=1' \
	'-DENABLE_SESSION_SERVICE=1' \
	'-DENABLE_THEMES=1' \
	'-DENABLE_BACKGROUND=1' \
	'-DENABLE_AUTOMATION=1' \
	'-DENABLE_LANGUAGE_DETECTION=1' \
	'-DENABLE_PRINTING=1' \
	'-DENABLE_CAPTIVE_PORTAL_DETECTION=1' \
	'-DWEBRTC_LOGGING' \
	'-DWEBRTC_LINUX' \
	'-DWEBRTC_THREAD_RR' \
	'-D__STDC_CONSTANT_MACROS' \
	'-D__STDC_FORMAT_MACROS' \
	'-DNDEBUG' \
	'-DNVALGRIND' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=0'

# Flags passed to all source files.
CFLAGS_Release := \
	-fstack-protector \
	--param=ssp-buffer-size=4 \
	-Werror \
	-pthread \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-fvisibility=hidden \
	-pipe \
	-fPIC \
	-Wextra \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-m32 \
	-mmmx \
	-march=pentium4 \
	-msse2 \
	-mfpmath=sse \
	-O2 \
	-fno-ident \
	-fdata-sections \
	-ffunction-sections

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-fno-rtti \
	-fno-threadsafe-statics \
	-fvisibility-inlines-hidden \
	-Wsign-compare \
	-Woverloaded-virtual

INCS_Release := \
	-Iwebrtc \
	-I. \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/test \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/interface \
	-Iwebrtc/modules/audio_coding/codecs/isac/main/util

OBJS := \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/isac/main/test/ReleaseTest-API/ReleaseTest-API.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/isac/main/util/utility.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(obj).target/webrtc/modules/libiSAC.a $(obj).target/webrtc/common_audio/libsignal_processing.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-Wl,-z,now \
	-Wl,-z,relro \
	-pthread \
	-Wl,-z,noexecstack \
	-fPIC \
	-Wl,--threads \
	-Wl,--thread-count=4 \
	-B$(builddir)/../../third_party/gold \
	-m32 \
	-Wl,--icf=none

LDFLAGS_Release := \
	-Wl,-z,now \
	-Wl,-z,relro \
	-pthread \
	-Wl,-z,noexecstack \
	-fPIC \
	-Wl,--threads \
	-Wl,--thread-count=4 \
	-B$(builddir)/../../third_party/gold \
	-m32 \
	-Wl,--icf=none \
	-Wl,-O1 \
	-Wl,--as-needed \
	-Wl,--gc-sections

LIBS := \
	 \
	-lrt

$(builddir)/iSACAPITest: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/iSACAPITest: LIBS := $(LIBS)
$(builddir)/iSACAPITest: LD_INPUTS := $(OBJS) $(obj).target/webrtc/modules/libiSAC.a $(obj).target/webrtc/common_audio/libsignal_processing.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a
$(builddir)/iSACAPITest: TOOLSET := $(TOOLSET)
$(builddir)/iSACAPITest: $(OBJS) $(obj).target/webrtc/modules/libiSAC.a $(obj).target/webrtc/common_audio/libsignal_processing.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a FORCE_DO_CMD
	$(call do_cmd,link)

all_deps += $(builddir)/iSACAPITest
# Add target alias
.PHONY: iSACAPITest
iSACAPITest: $(builddir)/iSACAPITest

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/iSACAPITest

