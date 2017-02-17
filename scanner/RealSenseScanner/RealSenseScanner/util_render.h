/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include <Windows.h>
#include "pxcsession.h"
#include "pxcimage.h"
#include "pxcaudio.h"
#include "pxccapture.h"
#include "service/pxcsessionservice.h"

#define AUDIO_SAMPLE_COUNT 10
#define AUDIO_SAMPLE_MAX_CHANNEL 10

typedef void(FocusCallback)(void*);

/// This class represents a window that renders an image.
class UtilRender {
public:
	UtilRender(pxcCHAR *title = 0, PXCSession *session = 0);
	virtual ~UtilRender(void);

	void SetSize(pxcI32 width, pxcI32 height);
	bool RenderFrame(const unsigned char* pBuffer, const int bitCount, const int width, const int height);
	bool RenderFrame(PXCImage *image);
	bool RenderFrame(PXCAudio *audio);
	void DoMessageLoop(void);

	static void RenderProgressBar(
		PXCImage*             in_out_image,
		size_t                frame_number,
		size_t                total_frames,
		size_t                color_channel_offset)
	{
		if (frame_number > total_frames) frame_number = total_frames; // Clamp ratio to no more than 1.0

		PXCImage::ImageData image_data;
		in_out_image->AcquireAccess(PXCImage::ACCESS_READ_WRITE, PXCImage::PIXEL_FORMAT_RGB24, &image_data);
		const size_t PROGRESS = (size_t)in_out_image->QueryInfo().width * frame_number / total_frames;
		const unsigned PROGRESS_BAR_THICKNESS_PIXELS = 5;
		for (int y = 0; y < PROGRESS_BAR_THICKNESS_PIXELS; y++)
		{
			// Get a pointer to the specified color channel of the first pixel in image row y
			pxcBYTE* p = color_channel_offset + image_data.planes[0] + y * image_data.pitches[0];
			const unsigned BYTES_PER_PIXEL = 3;
			for (size_t x = 0; x < PROGRESS; x++, p += BYTES_PER_PIXEL) *p = 0xff;
		}
		in_out_image->ReleaseAccess(&image_data);
	}

	const wchar_t *GetWC(const char *c)
	{
		const size_t cSize = strlen(c) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, c, cSize);

		return wc;
	}

	// Callback which is triggered when the window receives focus
	void SetOnFocusCallback(FocusCallback cb, void* data);

	int GetCurrentFPS();

	HWND            m_hWnd;
	PXCPointI32     m_mouse;
	int             m_frame;
	int             m_pause;

protected:

	PXCImage        *m_image;
	HBITMAP         m_bitmap;
	BITMAPINFO      m_info;
	BITMAPINFO*      m_pExtendedInfo;
	pxcI32          *m_buffer;
	float           m_scale;
	pxcI32          m_rendering_ratio;

	/// What depth information the window is rendering.  In most cases this
	/// is the magnitude of the value (normalized by a predefined max constant).
	/// The X and Y coordinates are only rendeFor vertex planes images.
	/// This mode has no effect for color or audio data.
	enum {
		RENDER_DEFAULT,    ///      Convert image to RGB32
		RENDER_X,          /// (F1) The x-coordinate (in color for vertex data)
		RENDER_Y,          /// (F2) The y-coordinate (in color for vertex data)
		RENDER_Z,          /// (F3) The z-coordinate in gray (for vertex data)
		///      or values (for depth data)
		RENDER_EDGES,      /// (F5) The the distance data in color with 
		///      edge detection (the sensitivity of
		///      edge detection is a function of m_scale)
		RENDER_CONFIDENCE, /// (F6) The IR in gray scale
	} m_depth_mode;

	/// Inidcates that m_depth_mode renders from one of the distance plane modes,
	/// either distance values or confidence values (on those samples).
	bool IsRenderingNonVertex() const {
		return m_depth_mode == RENDER_EDGES || m_depth_mode == RENDER_CONFIDENCE;
	}

	/* for fps measure */
	pxcCHAR         m_title[1024];
	pxcI64          m_time_first;
	pxcI64          m_rdtsc_first;
	int             m_fps_nframes;
	int				m_lastRenderedFPS;
	pxcF64          m_freq;


	pxcI64          m_bufferTime;
	int             m_sampleStride;
	int             m_numSamples;
	float           m_sampleBuffer[AUDIO_SAMPLE_COUNT*AUDIO_SAMPLE_MAX_CHANNEL];

	PXCSessionService *sservice;

	FocusCallback *callback;
	void		  *callbackData;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool   ResampleAudio(PXCAudio *audio);
	bool   DrawAudioWave(PXCAudio *audio);
	int    QueryFormatSize(PXCAudio::AudioFormat format);

	/// Subclasses may override to do additional drawing.
	virtual void DrawMore(HDC /*hdc*/, double /*scale_x*/, double /*scale_y*/) {}
};
