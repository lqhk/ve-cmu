// =============================================================================
//	main.cpp
//
//	Revision History:
//	Rev.01 2008/11/6	The First edition
//	Rev.02 2010/05/07	Change license to the modified BSD license
//
//	This file is licensed under the modified BSD license.
//
//	Copyright (C) 2008-2010 ViewPLUS Inc.
//	All Rights Reserved.
//
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions
//	are met:
//
//	*	Redistributions of source code must retain the above copyright
//		notice, this list of conditions and the following disclaimer.
//
//	*	Redistributions in binary form must reproduce the above
//		copyright notice, this list of conditions and the following
//		disclaimer in the documentation and/or other materials provided
//		with the distribution.
//
//	*	Neither the name of ViewPLUS Inc. nor the names of
//		its contributors may be used to endorse or promote products
//		derived from this software without specific prior written
//		permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// =============================================================================
/*!
	\file		VP1066Registers.h
	\author		Dairoku Sekiguchi
	\version	1.2
	\date		2008/11/06
	\license	modified BSD license
	\brief		VP1066A Register Definitions

	This file defines all the VP1066 Registers.
*/


#include <gtkmm.h>
#include <libglademm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "libviewplus/PF_EZInterface.h"

#define	DISP_IMAGE_WIDTH		800
#define	DISP_IMAGE_HEIGHT		600
#define	CAMERA_NUM				25

class EZViewerApp
{
public:
	Gtk::DrawingArea	*mDrawingArea;
	Gtk::Label		*mInfoLabel;

	Gtk::HScale	*mBrightnessScale;
	Gtk::CheckButton	*mBrightnessCheck;
	Gtk::HScale	*mAutoExposureScale;
	Gtk::CheckButton	*mAutoExposureCheck;
	Gtk::HScale	*mShutterScale;
	Gtk::CheckButton	*mShutterCheck;
	Gtk::HScale	*mGainScale;
	Gtk::CheckButton	*mGainCheck;

	Gtk::CheckButton	*mColorCheck;
	Gtk::CheckButton	*mCamOpCheck;

	Glib::RefPtr< Gdk::Pixmap >	mPixmap;
	Glib::RefPtr< Gdk::GC >	mGC;

	unsigned char	*mDispImageBuf;

	PF_EZDeviceHandle	mDeviceHandle;
	PF_EZImage			*mDeviceImage;
	PF_EZImage			*mColorImage;

	bool	mIsQVGAMode;
        int     mDeviceID;

	int		mCameraIndex;
	int		mFrameNum;
	bool	mIsColorEnabled;
	bool	mIsCamOpEnabled;
	bool	mIsCamOpUpdate;

	EZViewerApp()
	{
                mDeviceID = 0;
		mCameraIndex = 0;
		mFrameNum = 0;
		mIsColorEnabled = false;
		mIsCamOpEnabled = false;
		mIsCamOpUpdate = true;
	}

	void	UpdateInfoLabel()
	{
		char	buf[80];

		if (mCameraIndex == 0)
			sprintf(buf, "All");
		else
			sprintf(buf, "%d", mCameraIndex);
		
		mInfoLabel->set_text(buf);
		
		if (mIsCamOpEnabled)
			mIsCamOpUpdate = true;
	}

	void	On_cam_button_Clicked(int inCamIndex)
	{
		mCameraIndex = inCamIndex;
		UpdateInfoLabel();
	}

	void	On_prev_cam_button_Clicked()
	{
		mCameraIndex--;
		if (mCameraIndex < 0)
			mCameraIndex = CAMERA_NUM;

		UpdateInfoLabel();
	}

	void	On_next_cam_button_Clicked()
	{
		mCameraIndex++;
		if (mCameraIndex > CAMERA_NUM)
			mCameraIndex = 0;

		UpdateInfoLabel();
	}

	void	On_CameraNumButton_Clicked()
	{
		mCameraIndex = 0;
		UpdateInfoLabel();
	}

	void	On_reset_seq_button_Clicked()
	{
		mFrameNum = 0;
	}

	void	On_color_check_Toggled()
	{
		mIsColorEnabled = mColorCheck->get_active();
	}

	void	On_enable_camera_operation_chk_Toggled()
	{
		mIsCamOpEnabled = mCamOpCheck->get_active();
		mIsCamOpUpdate = true;
	}

	void	On_brightness_scale_Changed()
	{
		 // Note:
		 // The following kind of event synchronization mechanism strongly depends on a library implementation.
		 // It depends on how each gui event is processed within the library. Especially, it's order is important.
		 // So, please use with special care.
		 // At least, libgtkmm-2.4 seems to work fine.
		if (mIsCamOpUpdate == false)
			PF_EZPropertySetValue(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_BRIGHTNESS, mBrightnessScale->get_value());
	}

	void	On_brightness_chk_chk_Toggled()
	{
		if (mIsCamOpUpdate == false)
		{
			PF_EZPropertySetAutoState(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_BRIGHTNESS, mBrightnessCheck->get_active());
			mIsCamOpUpdate = true;
		}
	}

	void	On_exposure_scale_Changed()
	{
		if (mIsCamOpUpdate == false)
			PF_EZPropertySetValue(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_AUTO_EXPOSURE, mAutoExposureScale->get_value());
	}

	void	On_exposure_chk_Toggled()
	{
		if (mIsCamOpUpdate == false)
		{
			PF_EZPropertySetAutoState(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_AUTO_EXPOSURE, mAutoExposureCheck->get_active());
			if (mAutoExposureCheck->get_active() == false)
				PF_EZPropertySetValue(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_AUTO_EXPOSURE, 45.0);
			mIsCamOpUpdate = true;
		}
	}

	void	On_shutter_scale_Changed()
	{
		if (mIsCamOpUpdate == false)
			PF_EZPropertySetValue(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_SHUTTER, mShutterScale->get_value());
	}

	void	On_shutter_chk_Toggled()
	{
		if (mIsCamOpUpdate == false)
		{
			PF_EZPropertySetAutoState(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_SHUTTER, mShutterCheck->get_active());
			mIsCamOpUpdate = true;
		}
	}

	void	On_gain_scale_Changed()
	{
		if (mIsCamOpUpdate == false)
			PF_EZPropertySetValue(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_GAIN, mGainScale->get_value());
	}

	void	On_gain_chk_Toggled()
	{
		if (mIsCamOpUpdate == false)
		{
			PF_EZPropertySetAutoState(mDeviceHandle, mCameraIndex - 1, PF_EZ_CAMERA_GAIN, mGainCheck->get_active());
			mIsCamOpUpdate = true;
		}
	}

	void	On_update_button_Clicked()
	{
		bool	isAuto;
		double	max, min, val;
		const char	*unitStr;
		//wchar_t	buf[256];
		int	cameraIndex = mCameraIndex - 1;

		//	Brightness property
		PF_EZPropertyGetValueRange(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_BRIGHTNESS, &min, &max);
		PF_EZPropertyGetUnits(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_BRIGHTNESS, &unitStr);
		PF_EZPropertyGetValue(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_BRIGHTNESS, &val);
		PF_EZPropertyGetAutoState(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_BRIGHTNESS, &isAuto);

		mBrightnessScale->set_increments((max - min) / 256.0, (max - min) / 25.6);
		mBrightnessScale->set_range(min, max);
		mBrightnessScale->set_value(val);
		mBrightnessScale->set_sensitive(mIsCamOpEnabled & (~isAuto));
		mBrightnessCheck->set_active(isAuto);
		mBrightnessCheck->set_sensitive(mIsCamOpEnabled);

		//	Auto Exposure property
		PF_EZPropertyGetValueRange(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_AUTO_EXPOSURE, &min, &max);
		PF_EZPropertyGetUnits(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_AUTO_EXPOSURE, &unitStr);
		PF_EZPropertyGetValue(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_AUTO_EXPOSURE, &val);
		PF_EZPropertyGetAutoState(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_AUTO_EXPOSURE, &isAuto);

		mAutoExposureScale->set_increments((max - min) / 256.0, (max - min) / 25.6);
		mAutoExposureScale->set_range(min, max);
		mAutoExposureScale->set_value(val);
		mAutoExposureScale->set_sensitive(mIsCamOpEnabled & (~isAuto));
		mAutoExposureCheck->set_active(isAuto);
		mAutoExposureCheck->set_sensitive(mIsCamOpEnabled);

		//	Shutter property
		PF_EZPropertyGetValueRange(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_SHUTTER, &min, &max);
		PF_EZPropertyGetUnits(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_SHUTTER, &unitStr);
		PF_EZPropertyGetValue(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_SHUTTER, &val);
		PF_EZPropertyGetAutoState(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_SHUTTER, &isAuto);

		mShutterScale->set_increments((max - min) / 256.0, (max - min) / 25.6);
		mShutterScale->set_range(min, max);
		mShutterScale->set_value(val);
		mShutterScale->set_sensitive(mIsCamOpEnabled & (~isAuto));
		mShutterCheck->set_active(isAuto);
		mShutterCheck->set_sensitive(mIsCamOpEnabled);

		//	Gain property
		PF_EZPropertyGetValueRange(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_GAIN, &min, &max);
		PF_EZPropertyGetUnits(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_GAIN, &unitStr);
		PF_EZPropertyGetValue(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_GAIN, &val);
		PF_EZPropertyGetAutoState(mDeviceHandle, cameraIndex, PF_EZ_CAMERA_GAIN, &isAuto);

		mGainScale->set_increments((max - min) / 256.0, (max - min) / 25.6);
		mGainScale->set_range(min, max);
		mGainScale->set_value(val);
		mGainScale->set_sensitive(mIsCamOpEnabled & (~isAuto));
		mGainCheck->set_active(isAuto);
		mGainCheck->set_sensitive(mIsCamOpEnabled);

	}

	void	On_save_image_button_Clicked()
	{
		unsigned char	*tempBuf = new unsigned char[640 * 480 * 3];
		FILE	*fp;
		char	fileName[256];
	
		sprintf(fileName, "./ImageFiles");
		mkdir(fileName, 0777);
		sprintf(fileName, "./ImageFiles/%d", mFrameNum);
		mkdir(fileName, 0777);

		for(int cam = 0; cam < CAMERA_NUM; cam++)
		{
			sprintf(fileName, "./ImageFiles/%d/image%d.ppm", mFrameNum, cam);
			fp = fopen(fileName, "wb");		// Opening in binary is important

			fprintf(fp, "P6\n");
			fprintf(fp, "# ** comment **\n");
			fprintf(fp, "%d %d\n255\n", mDeviceImage->width, mDeviceImage->height);

			if (mIsColorEnabled == false)
			{
				unsigned char	*srcPtr = mDeviceImage->imageArray[cam];	
				unsigned char	*destPtr = tempBuf;	
				for(int y = 0; y < mDeviceImage->height; y++)
					for(int x = 0; x < mDeviceImage->width; x++)
					{
						*destPtr = *srcPtr;
						destPtr++;
						*destPtr = *srcPtr;
						destPtr++;
						*destPtr = *srcPtr;
						destPtr++;
						srcPtr++;
					}
	
				fwrite(tempBuf, sizeof(unsigned char),
				       mDeviceImage->width * mDeviceImage->height * 3, fp);
				fclose(fp);
			}
			else
			{
				unsigned char	*srcPtr = mColorImage->imageArray[cam];
				unsigned char	*destPtr = tempBuf;	
				for(int y = 0; y < mDeviceImage->height; y++)
					for(int x = 0; x < mDeviceImage->width; x++)
					{
						*destPtr = srcPtr[2];
						destPtr++;
						*destPtr = srcPtr[1];
						destPtr++;
						*destPtr = srcPtr[0];
						destPtr++;
						srcPtr+=3;
					}
	
				fwrite(tempBuf, sizeof(unsigned char),
				       mDeviceImage->width * mDeviceImage->height * 3, fp);
				fclose(fp);
			}
		}

		printf("All File saved into ./ImageFiles/%d \n", mFrameNum);
		mFrameNum++;

		delete tempBuf;
	}
	
	void	On_ImageDrawingArea_Realize()
	{
		mPixmap = Gdk::Pixmap::create(mDrawingArea->get_window(), 800, 600);
		mGC = Gdk::GC::create(mPixmap);
		
		mDispImageBuf = new unsigned char[800 * 600 * 3];
		unsigned char	pixData = 0;
		for (int y = 0; y < 600; y++)
			for (int x = 0; x < 800; x++)
			{
				mDispImageBuf[y * 800 * 3 + x * 3]  = pixData;
				mDispImageBuf[y * 800 * 3 + x * 3 + 1]  = pixData;
				mDispImageBuf[y * 800 * 3 + x * 3 + 2]  = pixData;
				pixData++;
			}
		
		mGC->set_rgb_fg_color(Gdk::Color("red"));
		mPixmap->draw_rectangle(mGC, true, 0, 0, 800, 600);

		if (mIsQVGAMode)
		{
			if (PF_EZOpenDevice(PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU, mDeviceID, &mDeviceHandle) != PF_EZ_OK)
			{
				printf("Can't open ProFUSION");
			}
		}
		else
		{
			if (PF_EZOpenDevice(PF_EZ_DEVICE_ProFUSION_25_CMU, mDeviceID, &mDeviceHandle) != PF_EZ_OK)
			{
				printf("Can't open ProFUSION");
			}
		}

		PF_EZCreateDeviceImage(mDeviceHandle, &mDeviceImage);
		PF_EZCreateBGR8Image(mDeviceHandle, &mColorImage);
		PF_EZCaptureStart(mDeviceHandle);
		
		mCameraIndex = 0;
	}

	bool	On_ImageDrawingArea_ExposeEvent(GdkEventExpose *event)
	{
		mPixmap->draw_rgb_image(
//			mDrawingArea->get_style()->get_fg_gc(mDrawingArea->get_state()),
			mGC,
			0, 0, 800, 600, Gdk::RGB_DITHER_NONE,
			(const guchar *)mDispImageBuf,
			800 * 3);

		mDrawingArea->get_window()->draw_drawable(
			mDrawingArea->get_style()->get_fg_gc(mDrawingArea->get_state()),
			mPixmap,
			event->area.x, event->area.y, event->area.x, event->area.y,
			event->area.width, event->area.height);

		return true;
	}

	bool	On_Idle()
	{
		if (PF_EZGetImage(mDeviceHandle, mDeviceImage) == PF_EZ_OK)
		{
			printf("Frame %lld\n", mDeviceImage->timestamp);
			if (mIsColorEnabled == false)
			{
				if (mCameraIndex != 0)
				{
					unsigned char *srcPtr = mDeviceImage->imageArray[mCameraIndex - 1];
					for (int y = 0; y < mDeviceImage->height; y++)
					{
						for (int x = 0; x < mDeviceImage->width; x++)
						{
							mDispImageBuf[x * 3 + DISP_IMAGE_WIDTH * 3 * y] =  *srcPtr;
							mDispImageBuf[x * 3 + 1+ DISP_IMAGE_WIDTH * 3 * y] =  *srcPtr;
							mDispImageBuf[x * 3 + 2 + DISP_IMAGE_WIDTH * 3 * y] =  *srcPtr;
							srcPtr++;
						}
					}
				}
				else
				{
					for(int cam = 0; cam < CAMERA_NUM; cam++)
					{
						int sx = (cam % 5) * 160;
						int sy = (int)(cam / 5) * 120;
		
						unsigned char *srcPtr = mDeviceImage->imageArray[cam];
						for(int y = 0; y < 120; y++)
						{
							for(int x = 0;x < 160; x++)
							{
								mDispImageBuf[(sx + x) * 3 + DISP_IMAGE_WIDTH * 3 * (sy + y)] = *srcPtr;
								mDispImageBuf[(sx + x) * 3 + 1 + DISP_IMAGE_WIDTH * 3 * (sy + y)] = *srcPtr;
								mDispImageBuf[(sx + x) * 3 + 2 + DISP_IMAGE_WIDTH * 3 * (sy + y)] = *srcPtr;
								srcPtr+=(mDeviceImage->width/160);
							}
							srcPtr+=mDeviceImage->widthStep*(mDeviceImage->height/120-1);
						}
					}
				}
			}
			else
			{
				PF_EZColorProcessing(mDeviceHandle, mDeviceImage, mColorImage);

				if (mCameraIndex != 0)
				{
					unsigned char *srcPtr = mColorImage->imageArray[mCameraIndex - 1];
					for (int y = 0; y < mDeviceImage->height; y++)
					{
						for (int x = 0; x < mDeviceImage->width; x++)
						{
							mDispImageBuf[x * 3 + 2 + DISP_IMAGE_WIDTH * 3 * y]	= *srcPtr;	// B
							srcPtr++;
							mDispImageBuf[x * 3 + 1+ DISP_IMAGE_WIDTH * 3 * y]	= *srcPtr;	// G
							srcPtr++;
							mDispImageBuf[x * 3 + DISP_IMAGE_WIDTH * 3 * y]		=  *srcPtr;	// R
							srcPtr++;
						}
					}
				}
				else
				{
					for(int cam = 0; cam < CAMERA_NUM; cam++)
					{
						int sx = (cam % 5) * 160;
						int sy = (int)(cam / 5) * 120;
		
						unsigned char *srcPtr = mColorImage->imageArray[cam];
						for(int y = 0; y < 120; y++)
						{
							for(int x = 0;x < 160; x++)
							{
								mDispImageBuf[(sx + x) * 3 + 2 + DISP_IMAGE_WIDTH * 3 * (sy + y)]	= *srcPtr;	// B
								srcPtr++;
								mDispImageBuf[(sx + x) * 3 + 1 + DISP_IMAGE_WIDTH * 3 * (sy + y)]	= *srcPtr;	// G
								srcPtr++;
								mDispImageBuf[(sx + x) * 3 + DISP_IMAGE_WIDTH * 3 * (sy + y)]		= *srcPtr;	// R
								srcPtr++;
								srcPtr+=3*(mDeviceImage->width/160-1);
							}
							srcPtr+=mDeviceImage->widthStep*(mDeviceImage->height/120-1)*3;
						}
					}
				}
			}
			mDrawingArea->queue_draw();
		}
		
		if (mIsCamOpUpdate)
		{
			On_update_button_Clicked();
			mIsCamOpUpdate = false;
		}
		
		return true;
	}
};

void help_and_exit(char * argv[]) {
  fprintf(stderr, "\n%s VGA|QVGA [CAMID]\n\n  example: %s VGA 1\n           => VGA/25fps mode, 2nd camera on the PCI bus (/dev/vpcpro1).\n\n", argv[0], argv[0]);
  exit(1);
}

int main(int argc, char *argv[])
{
	EZViewerApp	viewer_app;
	char	buf[256];

        if(argc != 2 && argc != 3) {
          help_and_exit(argv);
        } else {
          if(! strcmp(argv[1], "VGA") ) {
            viewer_app.mIsQVGAMode = false;
          } else if(! strcmp(argv[1], "QVGA") ) {
            viewer_app.mIsQVGAMode = true;
          } else {
            help_and_exit(argv);
          }

          if(argc == 3) {
            viewer_app.mDeviceID = atoi(argv[2]);
          }
        }

	Gtk::Main	kit(argc, argv);
	Glib::RefPtr<Gnome::Glade::Xml>	refMainWin = Gnome::Glade::Xml::create("main.glade");
	Glib::RefPtr<Gnome::Glade::Xml>	refControlWin = Gnome::Glade::Xml::create("control.glade");

	Gtk::Button	*pButton = 0;
	refControlWin->get_widget("prev_cam_button", pButton);
	pButton->signal_clicked().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_prev_cam_button_Clicked));
	refControlWin->get_widget("next_cam_button", pButton);
	pButton->signal_clicked().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_next_cam_button_Clicked));
	refControlWin->get_widget("save_image_button", pButton);
	pButton->signal_clicked().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_save_image_button_Clicked));
	refControlWin->get_widget("reset_seq_button", pButton);
	pButton->signal_clicked().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_reset_seq_button_Clicked));

	refControlWin->get_widget("update_button", pButton);
	pButton->signal_clicked().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_update_button_Clicked));

	refControlWin->get_widget("brightness_scale", viewer_app.mBrightnessScale);
	viewer_app.mBrightnessScale->signal_value_changed().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_brightness_scale_Changed));
	refControlWin->get_widget("exposure_scale", viewer_app.mAutoExposureScale);
	viewer_app.mAutoExposureScale->signal_value_changed().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_exposure_scale_Changed));
	refControlWin->get_widget("shutter_scale", viewer_app.mShutterScale);
	viewer_app.mShutterScale->signal_value_changed().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_shutter_scale_Changed));
	refControlWin->get_widget("gain_scale", viewer_app.mGainScale);
	viewer_app.mGainScale->signal_value_changed().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_gain_scale_Changed));

	refControlWin->get_widget("brightness_chk", viewer_app.mBrightnessCheck);
	viewer_app.mBrightnessCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_brightness_chk_chk_Toggled));
	refControlWin->get_widget("exposure_chk", viewer_app.mAutoExposureCheck);
	viewer_app.mAutoExposureCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_exposure_chk_Toggled));
	refControlWin->get_widget("shutter_chk", viewer_app.mShutterCheck);
	viewer_app.mShutterCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_shutter_chk_Toggled));
	refControlWin->get_widget("gain_chk", viewer_app.mGainCheck);
	viewer_app.mGainCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_gain_chk_Toggled));

	refControlWin->get_widget("enable_camera_operation_chk", viewer_app.mCamOpCheck);
	viewer_app.mCamOpCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_enable_camera_operation_chk_Toggled));

	refControlWin->get_widget("color_pcocessing_chk", viewer_app.mColorCheck);
	viewer_app.mColorCheck->signal_toggled().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_color_check_Toggled));

	for (int i = 0; i <=  CAMERA_NUM; i++)
	{
		sprintf(buf, "cam%d_button", i);
		refControlWin->get_widget(buf, pButton);
		pButton->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(viewer_app, &EZViewerApp::On_cam_button_Clicked), i));
	}

	refControlWin->get_widget("cam_index_label", viewer_app.mInfoLabel);

	refMainWin->get_widget("ImageDrawingArea", viewer_app.mDrawingArea);
	viewer_app.mDrawingArea->signal_realize().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_ImageDrawingArea_Realize));
	viewer_app.mDrawingArea->signal_expose_event().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_ImageDrawingArea_ExposeEvent));

	Glib::signal_idle().connect(sigc::mem_fun(viewer_app, &EZViewerApp::On_Idle));

	Gtk::Window	*pMainWindow = 0;
	refMainWin->get_widget("MainWindow", pMainWindow);

	Gtk::Window	*pControlWindow = 0;
	refControlWin->get_widget("Control", pControlWindow);
	pControlWindow->show();

	Gtk::Main::run(*pMainWindow);

	return 0;
}
