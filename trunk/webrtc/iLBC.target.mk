# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := iLBC
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
	-Iwebrtc/modules/audio_coding/codecs/ilbc/interface \
	-Iwebrtc/common_audio/signal_processing/include

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
	-Iwebrtc/modules/audio_coding/codecs/ilbc/interface \
	-Iwebrtc/common_audio/signal_processing/include

OBJS := \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/abs_quant.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/abs_quant_loop.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/augmented_cb_corr.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/bw_expand.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_construct.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_mem_energy.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_mem_energy_augmentation.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_mem_energy_calc.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_search.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_search_core.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/cb_update_best_index.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/chebyshev.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/comp_corr.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/constants.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/create_augmented_vec.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/decode.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/decode_residual.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/decoder_interpolate_lsf.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/do_plc.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/encode.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/energy_inverse.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/enh_upsample.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/enhancer.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/enhancer_interface.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/filtered_cb_vecs.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/frame_classify.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/gain_dequant.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/gain_quant.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/get_cd_vec.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/get_lsp_poly.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/get_sync_seq.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/hp_input.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/hp_output.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/ilbc.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/index_conv_dec.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/index_conv_enc.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/init_decode.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/init_encode.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/interpolate.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/interpolate_samples.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lpc_encode.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsf_check.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsf_interpolate_to_poly_dec.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsf_interpolate_to_poly_enc.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsf_to_lsp.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsf_to_poly.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/lsp_to_lsf.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/my_corr.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/nearest_neighbor.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/pack_bits.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/poly_to_lsf.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/poly_to_lsp.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/refiner.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/simple_interpolate_lsf.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/simple_lpc_analysis.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/simple_lsf_dequant.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/simple_lsf_quant.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/smooth.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/smooth_out_data.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/sort_sq.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/split_vq.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/state_construct.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/state_search.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/swap_bytes.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/unpack_bits.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/vq3.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/vq4.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/window32_w32.o \
	$(obj).target/$(TARGET)/webrtc/modules/audio_coding/codecs/ilbc/xcorr_coef.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

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
	

$(obj).target/webrtc/modules/libiLBC.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/webrtc/modules/libiLBC.a: LIBS := $(LIBS)
$(obj).target/webrtc/modules/libiLBC.a: TOOLSET := $(TOOLSET)
$(obj).target/webrtc/modules/libiLBC.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink_thin)

all_deps += $(obj).target/webrtc/modules/libiLBC.a
# Add target alias
.PHONY: iLBC
iLBC: $(obj).target/webrtc/modules/libiLBC.a

# Add target alias to "all" target.
.PHONY: all
all: iLBC

