/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright 2014 Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual property 
* laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

#import "AdobeImageEditorCustomization.h"

NS_ASSUME_NONNULL_BEGIN

/*
 All constants and compatibility aliases definted in this file
 are meant to provide a bridge between implementations of the legacy
 Aviary SDK and the Adobe Image SDK. Officially, the Aviary APIs are
 considered deprecated and are here for your convenience. These may be removed
 in future releases of the SDK.
 */

#define ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT DEPRECATED_MSG_ATTRIBUTE("This constant has been deprecated use the " \
                                                                        "corresponding constant in AdobeImageEditorCustomization.h")

extern NSString *const kAVYEnhance ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                          	/* Enhance */
extern NSString *const kAVYEffects ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                          	/* Effects */
extern NSString *const kAVYStickers ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                         	/* Stickers */
extern NSString *const kAVYOrientation ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                      	/* Orientation */
extern NSString *const kAVYCrop ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                             	/* Crop */
extern NSString *const kAVYAdjustments ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT; 						/* Deprecated, use kAVYColorAdjust and kAVYLightingAdjust instead */
extern NSString *const kAVYColorAdjust ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                      	/* Color */
extern NSString *const kAVYLightingAdjust ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                   	/* Lighting */
extern NSString *const kAVYSharpness ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                        	/* Sharpness */
extern NSString *const kAVYDraw ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                             	/* Draw */
extern NSString *const kAVYText ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                             	/* Text */
extern NSString *const kAVYRedeye ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                           	/* Redeye */
extern NSString *const kAVYWhiten ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                           	/* Whiten */
extern NSString *const kAVYBlemish ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                          	/* Blemish */
extern NSString *const kAVYBlur ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                             	/* Blur */
extern NSString *const kAVYMeme ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                             	/* Meme */
extern NSString *const kAVYFrames ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                           	/* Frames */
extern NSString *const kAVYFocus ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                            	/* TiltShift */
extern NSString *const kAVYSplash ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                           	/* ColorSplash */

extern NSString *const kAVYLeftNavigationTitlePresetCancel ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;  	/* Cancel */
extern NSString *const kAVYLeftNavigationTitlePresetBack ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;    	/* Back */
extern NSString *const kAVYLeftNavigationTitlePresetExit ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;    	/* Exit */

extern NSString *const kAVYRightNavigationTitlePresetDone ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;   	/* Done */
extern NSString *const kAVYRightNavigationTitlePresetSave ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;   	/* Save */
extern NSString *const kAVYRightNavigationTitlePresetNext ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;   	/* Next */
extern NSString *const kAVYRightNavigationTitlePresetSend ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;   	/* Send */

extern NSString *const kAVYCropPresetName ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                   	/* Name */
extern NSString *const kAVYCropPresetWidth ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                  	/* Width */
extern NSString *const kAVYCropPresetHeight ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;                 	/* Height */

NSString *const _kAFEnhance(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFEnhance _kAFEnhance()

NSString *const _kAFEffects(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFEffects _kAFEffects()

NSString *const _kAFStickers(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFStickers _kAFStickers()

NSString *const _kAFOrientation(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFOrientation _kAFOrientation()

NSString *const _kAFCrop(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFCrop _kAFCrop()

NSString *const _kAFAdjustments(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFAdjustments _kAFAdjustments()

NSString *const _kAFColorAdjust(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFColorAdjust _kAFColorAdjust()

NSString *const _kAFLightingAdjust(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFLightingAdjust _kAFLightingAdjust()

NSString *const _kAFSharpness(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFSharpness _kAFSharpness()

NSString *const _kAFDraw(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFDraw _kAFDraw()

NSString *const _kAFText(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFText _kAFText()

NSString *const _kAFRedeye(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFRedeye _kAFRedeye()

NSString *const _kAFWhiten(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFWhiten _kAFWhiten()

NSString *const _kAFBlemish(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFBlemish _kAFBlemish()

NSString *const _kAFBlur(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFBlur _kAFBlur()

NSString *const _kAFMeme(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFMeme _kAFMeme()

NSString *const _kAFFrames(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFFrames _kAFFrames()

NSString *const _kAFFocus(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFFocus _kAFFocus()

NSString *const _kAFSplash(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFSplash _kAFSplash()

NSString *const _kAFLeftNavigationTitlePresetCancel(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFLeftNavigationTitlePresetCancel _kAFLeftNavigationTitlePresetCancel()

NSString *const _kAFLeftNavigationTitlePresetBack(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFLeftNavigationTitlePresetBack _kAFLeftNavigationTitlePresetBack()

NSString *const _kAFLeftNavigationTitlePresetExit(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFLeftNavigationTitlePresetExit _kAFLeftNavigationTitlePresetExit()

NSString *const _kAFRightNavigationTitlePresetDone(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFRightNavigationTitlePresetDone _kAFRightNavigationTitlePresetDone()

NSString *const _kAFRightNavigationTitlePresetSave(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFRightNavigationTitlePresetSave _kAFRightNavigationTitlePresetSave()

NSString *const _kAFRightNavigationTitlePresetNext(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFRightNavigationTitlePresetNext _kAFRightNavigationTitlePresetNext()

NSString *const _kAFRightNavigationTitlePresetSend(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFRightNavigationTitlePresetSend _kAFRightNavigationTitlePresetSend()

NSString *const _kAFCropPresetName(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFCropPresetName _kAFCropPresetName()

NSString *const _kAFCropPresetWidth(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFCropPresetWidth _kAFCropPresetWidth()

NSString *const _kAFCropPresetHeight(void) ADOBE_CUSTOMIZATON_DEPRECATED_CONSTANT;
#define kAFCropPresetHeight _kAFCropPresetHeight()

NS_ASSUME_NONNULL_END