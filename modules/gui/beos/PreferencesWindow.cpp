/*****************************************************************************
 * PreferencesWindow.cpp: beos interface
 *****************************************************************************
 * Copyright (C) 1999, 2000, 2001 VideoLAN
 * $Id: PreferencesWindow.cpp,v 1.7 2003/01/16 15:26:23 titer Exp $
 *
 * Authors: Eric Petit <titer@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/* BeOS headers */
#include <InterfaceKit.h>

/* VLC headers */
#include <vlc/vlc.h>
#include <vlc/intf.h>

/* BeOS module headers */
#include "VlcWrapper.h"
#include "MsgVals.h"
#include "PreferencesWindow.h"


/*****************************************************************************
 * Preferences::PreferencesWindow
 *****************************************************************************/
PreferencesWindow::PreferencesWindow( BRect frame, const char* name,
								intf_thread_t *p_interface )
	:	BWindow( frame, name, B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
				 B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_CLOSABLE )
{
	p_intf = p_interface;
    BRect rect;

    /* "background" view */
    fPrefsView = new BView( Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW );
    fPrefsView->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
    AddChild( fPrefsView );

    /* add the tabs */
    rect = Bounds();
    rect.top += 10;
    rect.bottom -= 65;
    fTabView = new BTabView( rect, "preferences view" );
    fTabView->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
    
    fGeneralView = new BView( fTabView->Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW );
    fGeneralView->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
    fAdjustView = new BView( fTabView->Bounds(), NULL, B_FOLLOW_ALL, B_WILL_DRAW );
    fAdjustView->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
    
    fGeneralTab = new BTab();
    fTabView->AddTab( fGeneralView, fGeneralTab );
    fGeneralTab->SetLabel( "General" );
    
    fAdjustTab = new BTab();
    fTabView->AddTab( fAdjustView, fAdjustTab );
    fAdjustTab->SetLabel( "Adjust" );
    
    /* fills the tabs */
    /* ffmpeg tab */
    rect = fGeneralView->Bounds();
    rect.InsetBy( 10, 10 );
    rect.bottom = rect.top + 10;
    fOldDvdCheck = new BCheckBox( rect, "olddvd", "Do not use DVD menus",
                                  new BMessage( OLDDVD_CHECK ) );
    fGeneralView->AddChild( fOldDvdCheck );
    fprintf( stderr, "%s\n", config_GetPsz( p_intf, "access" ) );
    if( config_GetPsz( p_intf, "access" ) &&
        !strncmp( config_GetPsz( p_intf, "access" ), "dvdold,any", 10 ) )
        fOldDvdCheck->SetValue( 1 );
    
    rect.top = rect.bottom + 20;
    rect.bottom = rect.top + 30;
    fPpSlider = new BSlider( rect, "post-processing", "MPEG4 post-processing level",
                               new BMessage( SLIDER_UPDATE ),
                               0, 6, B_TRIANGLE_THUMB,
                               B_FOLLOW_LEFT, B_WILL_DRAW ); 
    fPpSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
    fPpSlider->SetHashMarkCount( 7 );
    fPpSlider->SetLimitLabels( "None", "Maximum" );
    fPpSlider->SetValue( config_GetInt( p_intf, "ffmpeg-pp-q" ) );
    fGeneralView->AddChild( fPpSlider );
    
    
    /* adjust tab */
    rect = fAdjustView->Bounds();
    rect.InsetBy( 10, 10 );
    rect.bottom = rect.top + 30;
    fBrightnessSlider = new BSlider( rect, "brightness", "Brightness",
                                       new BMessage( SLIDER_UPDATE ),
                                       0, 200, B_TRIANGLE_THUMB,
                                       B_FOLLOW_LEFT, B_WILL_DRAW );
    fBrightnessSlider->SetValue( 100 * config_GetFloat( p_intf, "brightness" ) );
    rect.OffsetBy( 0, 40 );
    fContrastSlider = new BSlider( rect, "contrast", "Contrast",
                                     new BMessage( SLIDER_UPDATE ),
                                     0, 200, B_TRIANGLE_THUMB,
                                     B_FOLLOW_LEFT, B_WILL_DRAW );
    fContrastSlider->SetValue( 100 * config_GetFloat( p_intf, "contrast" ) );
    rect.OffsetBy( 0, 40 );
    fHueSlider = new BSlider( rect, "hue", "Hue",
                                new BMessage( SLIDER_UPDATE ),
                                0, 360, B_TRIANGLE_THUMB,
                                B_FOLLOW_LEFT, B_WILL_DRAW );
    fHueSlider->SetValue( config_GetInt( p_intf, "hue" ) );
    rect.OffsetBy( 0, 40 );
    fSaturationSlider = new BSlider( rect, "saturation", "Saturation",
                                       new BMessage( SLIDER_UPDATE ),
                                       0, 200, B_TRIANGLE_THUMB,
                                       B_FOLLOW_LEFT, B_WILL_DRAW );
    fSaturationSlider->SetValue( 100 * config_GetFloat( p_intf, "saturation" ) );
    fAdjustView->AddChild( fBrightnessSlider );
    fAdjustView->AddChild( fContrastSlider );
    fAdjustView->AddChild( fHueSlider );
    fAdjustView->AddChild( fSaturationSlider );
    
    fPrefsView->AddChild( fTabView );

    /* restart message */
    rect = fPrefsView->Bounds();
    rect.bottom -= 43;
    rect.top = rect.bottom - 10;
    fRestartString = new BStringView( rect, NULL,
        "Warning: changing settings after starting playback may have no effect." );
    /*rgb_color redColor = {255, 0, 0, 255};
    fRestartString->SetHighColor(redColor);*/
    fRestartString->SetAlignment( B_ALIGN_CENTER );
    fPrefsView->AddChild( fRestartString );

    /* buttons */
    BButton *button;
    rect = Bounds();
    rect.InsetBy( 10, 10 );
    rect.top = rect.bottom - 25;
    rect.left = rect.right - 80;
    button = new BButton( rect, NULL, "OK", new BMessage( PREFS_OK ) );
    fPrefsView->AddChild( button );

    rect.OffsetBy( -90, 0 );
    button = new BButton( rect, NULL, "Save", new BMessage( PREFS_SAVE ) );
    fPrefsView->AddChild( button );
    
    rect.OffsetBy( -90, 0 );
    button = new BButton( rect, NULL, "Defaults", new BMessage( PREFS_DEFAULTS ) );
    fPrefsView->AddChild( button );
    
	// start window thread in hidden state
	Hide();
	Show();
}

/*****************************************************************************
 * PreferencesWindow::~PreferencesWindow
 *****************************************************************************/
PreferencesWindow::~PreferencesWindow()
{
}

/*****************************************************************************
 * PreferencesWindow::MessageReceived
 *****************************************************************************/
void PreferencesWindow::MessageReceived( BMessage * p_message )
{
	switch ( p_message->what )
	{
	    case OLDDVD_CHECK:
	    case SLIDER_UPDATE:
	    {
	        ApplyChanges();
	        break;
	    }
	    case PREFS_DEFAULTS:
	    {
	        SetDefaults();
            ApplyChanges();
	        break;
	    }
	    case PREFS_SAVE:
	    {
	        config_SaveConfigFile( p_intf, "main" );
	        config_SaveConfigFile( p_intf, "adjust" );
	        config_SaveConfigFile( p_intf, "ffmpeg" );
	        break;
	    }
	    case PREFS_OK:
	    {
            Hide();
            break;
	    }
		default:
			BWindow::MessageReceived( p_message );
			break;
	}
}

/*****************************************************************************
 * PreferencesWindow::ReallyQuit
 *****************************************************************************/
void PreferencesWindow::ReallyQuit()
{
    Hide();
    Quit();
}

/*****************************************************************************
 * PreferencesWindow::SetDefaults
 *****************************************************************************/
void PreferencesWindow::SetDefaults()
{
    fOldDvdCheck->SetValue( 0 );
    fPpSlider->SetValue( 0 );
    fBrightnessSlider->SetValue( 100 );
    fContrastSlider->SetValue( 100 );
    fHueSlider->SetValue( 0 );
    fSaturationSlider->SetValue( 100 );
}

/*****************************************************************************
 * PreferencesWindow::ApplyChanges
 *****************************************************************************/
void PreferencesWindow::ApplyChanges()
{
    if( fOldDvdCheck->Value() )
        config_PutPsz( p_intf, "access", "dvdold,any" );
    else
        config_PutPsz( p_intf, "access", NULL );
    
    config_PutInt( p_intf, "ffmpeg-pp-q", fPpSlider->Value() );
    config_PutFloat( p_intf, "brightness",
                     (float)fBrightnessSlider->Value() / 100 );
    config_PutFloat( p_intf, "contrast",
                     (float)fContrastSlider->Value() / 100 );
    config_PutInt( p_intf, "hue", fHueSlider->Value() );
    config_PutFloat( p_intf, "saturation",
                     (float)fSaturationSlider->Value() / 100 );

    if( config_GetFloat( p_intf, "brightness" ) != 1 ||
        config_GetFloat( p_intf, "contrast" ) != 1 ||
        config_GetInt( p_intf, "hue" ) != 0 ||
        config_GetFloat( p_intf, "saturation" ) != 1 )
    {
        config_PutPsz( p_intf, "filter", "adjust" );
    }
    else
    {
        config_PutPsz( p_intf, "filter", NULL );
    }
}
