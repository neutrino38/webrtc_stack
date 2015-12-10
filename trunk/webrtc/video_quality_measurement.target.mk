# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := video_quality_measurement
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
	'-DGFLAGS_DLL_DECL=' \
	'-DGFLAGS_DLL_DECLARE_FLAG=' \
	'-DGFLAGS_DLL_DEFINE_FLAG=' \
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
	-Iwebrtc/test \
	-Iwebrtc/modules/video_coding/main/interface \
	-Iwebrtc/modules/video_coding/codecs/interface \
	-Ithird_party/google-gflags/gen/arch/linux/ia32/include \
	-Ithird_party/google-gflags/src \
	-Iwebrtc/modules/video_coding/codecs/vp8/include \
	-Iwebrtc/common_video/interface \
	-Iwebrtc/modules/video_coding/codecs/interface

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
	'-DGFLAGS_DLL_DECL=' \
	'-DGFLAGS_DLL_DECLARE_FLAG=' \
	'-DGFLAGS_DLL_DEFINE_FLAG=' \
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
	-Iwebrtc/test \
	-Iwebrtc/modules/video_coding/main/interface \
	-Iwebrtc/modules/video_coding/codecs/interface \
	-Ithird_party/google-gflags/gen/arch/linux/ia32/include \
	-Ithird_party/google-gflags/src \
	-Iwebrtc/modules/video_coding/codecs/vp8/include \
	-Iwebrtc/common_video/interface \
	-Iwebrtc/modules/video_coding/codecs/interface

OBJS := \
	$(obj).target/$(TARGET)/webrtc/modules/video_coding/codecs/tools/video_quality_measurement.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(obj).target/webrtc/modules/libvideo_codecs_test_framework.a $(obj).target/webrtc/modules/libwebrtc_video_coding.a $(obj).target/third_party/google-gflags/libgoogle-gflags.a $(obj).target/webrtc/test/libmetrics.a $(obj).target/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a $(obj).target/webrtc/test/libtest_support.a $(obj).target/testing/libgtest.a $(obj).target/testing/gtest_prod.stamp $(obj).target/testing/libgmock.a $(obj).target/webrtc/modules/libwebrtc_i420.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a $(obj).target/webrtc/common_video/libcommon_video.a $(obj).target/third_party/libjpeg_turbo/libjpeg_turbo.a $(obj).target/third_party/libyuv/libyuv.a $(obj).target/third_party/libvpx/libvpx.a $(obj).target/third_party/libvpx/gen_asm_offsets.stamp $(obj).target/third_party/libvpx/libvpx_asm_offsets.a $(obj).target/third_party/libvpx/libvpx_sse2.a

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

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

$(builddir)/video_quality_measurement: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/video_quality_measurement: LIBS := $(LIBS)
$(builddir)/video_quality_measurement: LD_INPUTS := $(OBJS) $(obj).target/webrtc/modules/libvideo_codecs_test_framework.a $(obj).target/webrtc/modules/libwebrtc_video_coding.a $(obj).target/third_party/google-gflags/libgoogle-gflags.a $(obj).target/webrtc/test/libmetrics.a $(obj).target/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a $(obj).target/webrtc/test/libtest_support.a $(obj).target/testing/libgtest.a $(obj).target/testing/libgmock.a $(obj).target/webrtc/modules/libwebrtc_i420.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a $(obj).target/webrtc/common_video/libcommon_video.a $(obj).target/third_party/libjpeg_turbo/libjpeg_turbo.a $(obj).target/third_party/libyuv/libyuv.a $(obj).target/third_party/libvpx/libvpx.a $(obj).target/third_party/libvpx/libvpx_asm_offsets.a $(obj).target/third_party/libvpx/libvpx_sse2.a
$(builddir)/video_quality_measurement: TOOLSET := $(TOOLSET)
$(builddir)/video_quality_measurement: $(OBJS) $(obj).target/webrtc/modules/libvideo_codecs_test_framework.a $(obj).target/webrtc/modules/libwebrtc_video_coding.a $(obj).target/third_party/google-gflags/libgoogle-gflags.a $(obj).target/webrtc/test/libmetrics.a $(obj).target/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a $(obj).target/webrtc/test/libtest_support.a $(obj).target/testing/libgtest.a $(obj).target/testing/libgmock.a $(obj).target/webrtc/modules/libwebrtc_i420.a $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a $(obj).target/webrtc/common_video/libcommon_video.a $(obj).target/third_party/libjpeg_turbo/libjpeg_turbo.a $(obj).target/third_party/libyuv/libyuv.a $(obj).target/third_party/libvpx/libvpx.a $(obj).target/third_party/libvpx/libvpx_asm_offsets.a $(obj).target/third_party/libvpx/libvpx_sse2.a FORCE_DO_CMD
	$(call do_cmd,link)

all_deps += $(builddir)/video_quality_measurement
# Add target alias
.PHONY: video_quality_measurement
video_quality_measurement: $(builddir)/video_quality_measurement

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/video_quality_measurement

