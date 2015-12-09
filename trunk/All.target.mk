# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := All
### Rules for final target.
$(obj).target/All.stamp: TOOLSET := $(TOOLSET)
$(obj).target/All.stamp: $(obj).target/webrtc/common_audio/libsignal_processing.a $(obj).target/webrtc/common_audio/libresampler.a $(obj).target/webrtc/common_audio/libvad.a $(builddir)/signal_processing_unittests $(builddir)/resampler_unittests $(builddir)/vad_unittests $(obj).target/webrtc/common_video/libcommon_video.a $(builddir)/common_video_unittests $(obj).target/webrtc/modules/libCNG.a $(obj).target/webrtc/modules/libG711.a $(obj).target/webrtc/modules/libG722.a $(obj).target/webrtc/modules/libiLBC.a $(obj).target/webrtc/modules/libiSAC.a $(obj).target/webrtc/modules/libiSACFix.a $(obj).target/webrtc/modules/libPCM16B.a $(obj).target/webrtc/modules/libaudio_coding_module.a $(obj).target/webrtc/modules/libNetEq.a $(obj).target/webrtc/modules/libaudio_conference_mixer.a $(obj).target/webrtc/modules/libaudio_device.a $(obj).target/webrtc/modules/libaudio_processing.a $(obj).target/webrtc/modules/libbitrate_controller.a $(obj).target/webrtc/modules/libmedia_file.a $(obj).target/webrtc/modules/libpaced_sender.a $(obj).target/webrtc/modules/libremote_bitrate_estimator.a $(obj).target/webrtc/modules/librtp_rtcp.a $(obj).target/webrtc/modules/libudp_transport.a $(obj).target/webrtc/modules/libwebrtc_utility.a $(obj).target/webrtc/modules/libwebrtc_i420.a $(obj).target/webrtc/modules/libwebrtc_video_coding.a $(obj).target/webrtc/modules/libvideo_capture_module.a $(obj).target/webrtc/modules/libvideo_processing.a $(obj).target/webrtc/modules/libvideo_render_module.a $(obj).target/webrtc/modules/libwebrtc_opus.a $(builddir)/opus_demo $(builddir)/iSACtest $(builddir)/iSACAPITest $(builddir)/iSACSwitchSampRateTest $(builddir)/iSACFixtest $(builddir)/isacfix_unittests $(builddir)/audioproc_unittest $(obj).target/webrtc/modules/libaudioproc_unittest_proto.a $(builddir)/rtp_rtcp_unittests $(builddir)/test_fec $(builddir)/test_rtp_rtcp_api $(builddir)/video_coding_test $(builddir)/video_coding_unittests $(builddir)/video_processing_unittests $(builddir)/audioproc $(builddir)/unpack_aecdump $(obj).target/webrtc/modules/libvideo_codecs_test_framework.a $(builddir)/video_codecs_test_framework_unittests $(builddir)/video_codecs_test_framework_integrationtests $(builddir)/video_quality_measurement $(builddir)/cng_unittests $(builddir)/g711_unittests $(builddir)/g711_test $(builddir)/g722_unittests $(builddir)/G722Test $(builddir)/iLBCtest $(builddir)/pcm16b_unittests $(builddir)/audio_coding_module_test $(builddir)/audio_coding_unittests $(builddir)/neteq_unittests $(builddir)/NetEqRTPplay $(builddir)/RTPencode $(builddir)/RTPjitter $(builddir)/RTPanalyze $(builddir)/RTPchange $(builddir)/RTPtimeshift $(builddir)/RTPcat $(builddir)/rtp_to_text $(obj).target/webrtc/modules/libNetEqTestTools.a $(builddir)/audio_conference_mixer_unittests $(builddir)/audio_device_test_api $(builddir)/audio_device_test_func $(obj).target/webrtc/modules/libaudioproc_debug_proto.a $(obj).target/webrtc/modules/libaudio_processing_sse2.a $(builddir)/bitrate_controller_unittests $(builddir)/media_file_unittests $(builddir)/paced_sender_unittests $(builddir)/remote_bitrate_estimator_unittests $(builddir)/udp_transport_unittests $(builddir)/webrtc_utility_unittests $(builddir)/video_capture_module_test $(obj).target/webrtc/modules/libvideo_processing_sse2.a $(builddir)/video_render_module_test $(obj).target/webrtc/system_wrappers/source/libsystem_wrappers.a $(builddir)/system_wrappers_unittests $(obj).target/webrtc/video_engine/libvideo_engine_core.a $(obj).target/webrtc/video_engine/libvietest.a $(builddir)/vie_auto_test $(builddir)/video_engine_core_unittests $(obj).target/webrtc/voice_engine/libvoice_engine_core.a $(builddir)/voe_auto_test $(builddir)/voe_cmd_test $(builddir)/voice_engine_unittests $(obj).target/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a $(builddir)/vp8_integrationtests $(builddir)/vp8_unittests $(obj).target/webrtc/modules/video_coding/codecs/vp8/libtest_framework.a $(obj).target/webrtc/test/libmetrics.a $(builddir)/metrics_unittests $(obj).target/webrtc/test/libtest_support.a $(obj).target/webrtc/test/libtest_support_main.a $(obj).target/webrtc/test/libtest_support_main_threaded_mac.a $(builddir)/test_support_unittests $(obj).target/webrtc/tools/libcommand_line_parser.a $(obj).target/webrtc/tools/libvideo_quality_analysis.a $(builddir)/frame_analyzer $(builddir)/psnr_ssim_analyzer $(builddir)/rgba_to_i420_converter $(obj).target/webrtc/tools/libframe_cutter_lib.a $(builddir)/frame_cutter $(builddir)/tools_unittests $(builddir)/audio_e2e_harness FORCE_DO_CMD
	$(call do_cmd,touch)

all_deps += $(obj).target/All.stamp
# Add target alias
.PHONY: All
All: $(obj).target/All.stamp

# Add target alias to "all" target.
.PHONY: all
all: All
