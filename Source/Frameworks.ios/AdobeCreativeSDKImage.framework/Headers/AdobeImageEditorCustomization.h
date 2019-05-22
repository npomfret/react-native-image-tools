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

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const kAdobeImageEditorEnhance;        /* Enhance */
extern NSString *const kAdobeImageEditorEffects;        /* Effects */
extern NSString *const kAdobeImageEditorStickers;       /* Stickers */
extern NSString *const kAdobeImageEditorOrientation;    /* Orientation */
extern NSString *const kAdobeImageEditorCrop;           /* Crop */
extern NSString *const kAdobeImageEditorColorAdjust;    /* Color */
extern NSString *const kAdobeImageEditorLightingAdjust; /* Lighting */
extern NSString *const kAdobeImageEditorSharpness;      /* Sharpness */
extern NSString *const kAdobeImageEditorDraw;           /* Draw */
extern NSString *const kAdobeImageEditorText;           /* Text */
extern NSString *const kAdobeImageEditorRedeye;         /* Redeye */
extern NSString *const kAdobeImageEditorWhiten;         /* Whiten */
extern NSString *const kAdobeImageEditorBlemish;        /* Blemish */
extern NSString *const kAdobeImageEditorBlur;           /* Blur */
extern NSString *const kAdobeImageEditorMeme;           /* Meme */
extern NSString *const kAdobeImageEditorFrames;         /* Frames */
extern NSString *const kAdobeImageEditorFocus;          /* TiltShift */
extern NSString *const kAdobeImageEditorSplash;         /* ColorSplash */
extern NSString *const kAdobeImageEditorOverlay;        /* Overlay */
extern NSString *const kAdobeImageEditorVignette;       /* Vignette */


extern NSString *const kAdobeImageEditorLeftNavigationTitlePresetCancel; /* Cancel */
extern NSString *const kAdobeImageEditorLeftNavigationTitlePresetBack;   /* Back */
extern NSString *const kAdobeImageEditorLeftNavigationTitlePresetExit;   /* Exit */

extern NSString *const kAdobeImageEditorRightNavigationTitlePresetDone;  /* Done */
extern NSString *const kAdobeImageEditorRightNavigationTitlePresetSave;  /* Save */
extern NSString *const kAdobeImageEditorRightNavigationTitlePresetNext;  /* Next */
extern NSString *const kAdobeImageEditorRightNavigationTitlePresetSend;  /* Send */

extern NSString *const kAdobeImageEditorCropPresetName;   /* Name */
extern NSString *const kAdobeImageEditorCropPresetWidth;  /* Width */
extern NSString *const kAdobeImageEditorCropPresetHeight; /* Height */

/**
 This class provides a powerful interface for configuring 
 AdobeUXImageEditorViewController's appearance and behavior. While changing 
 values after presenting an AdobeUXImageEditorViewController's instance is 
 possible, it is strongly recommended that you make all necessary calls to 
 AdobeImageEditorCustomization *before* editor presentation. Example usage 
 can be found in the iOS SDK Customization Guide.
 */
@interface AdobeImageEditorCustomization : NSObject

@end

@interface AdobeImageEditorCustomization (PCNSupport)

/**
 Configures the editor to point at the Premium Content Network's staging 
 environment.
 
 By default, the editor points at the production environment. Call this 
 method with YES before editor to launch to view the content in the 
 Premium Content Network staging environment.
 @param usePCNStagingEnvironment YES points the editor to staging, 
 NO points it to production.
 */

+ (void)usePCNStagingEnvironment:(BOOL)usePCNStagingEnvironment;

@end

@interface AdobeImageEditorCustomization (iPadOrientation)

/**
 Configures the orientations the editor can have on the iPad form factor.
 
 On the iPhone form factor, orientation is always portrait.
 
 @param supportedOrientations An NSArray containing NSNumbers each 
 representing a valid UIInterfaceOrientation.
 */

+ (void)setSupportedIpadOrientations:(NSArray *)supportedOrientations;
+ (NSArray *)supportedIpadOrientations;

@end

@interface AdobeImageEditorCustomization (Appearance)

/**
 Sets the tool's icon image in the editor's home bottom bar.
 
 The tool key must be one of the options listed at the top of this header.
 
 @param image The image to use.
 @param tool The tool to set for the image. See the discussion 
 for possible values.
 */
+ (void)setIconImage:(UIImage *__nullable)image forTool:(NSString*)tool;

/**
 Sets the editor's navigation bar's background image.
 
 @param navBarImage The image to use.
 @warning This method no longer has any effect.
 */
+ (void)setNavBarImage:(UIImage *__nullable)navBarImage DEPRECATED_MSG_ATTRIBUTE("This property is now deprecated and has no effect. ");

/**
 Sets the editor's preferred status bar style when running 
 on a device running iOS 7 and later.

 @param statusBarStyle The status bar style to use.
 @warning This method no longer has any effect.
 */
+ (void)setStatusBarStyle:(UIStatusBarStyle)statusBarStyle DEPRECATED_MSG_ATTRIBUTE("This property is now deprecated and has no effect. Please use the Editor customizer application for your customization needs.");

@end

@interface AdobeImageEditorCustomization (Imaging)

/**
 Configures the editor to free GPU memory when possible.
 
 By default, the SDK keeps a small number of OpenGL objects loaded to 
 optimize launches of the editor. Set this key to YES purge GPU memory 
 when possible.
 
 @param purgeGPUMemory YES purges GPU memory when possible, NO retains it.
 */
+ (void)purgeGPUMemoryWhenPossible:(BOOL)purgeGPUMemory;
+ (BOOL)purgeGPUMemoryWhenPossible;

@end

@interface AdobeImageEditorCustomization (NavigationBarButtons)

/**
 Sets the text of the editor's left navigation bar button.
 
 Attempting to set any string besides one of the kAdobeImageEditorLeftNavigationTitlePresets 
 will have no effect.
 
 @param leftNavigationBarButtonTitle An NSString value represented by 
 one of the three kAdobeImageEditorLeftNavigationTitlePreset keys.
 */

+ (void)setLeftNavigationBarButtonTitle:(NSString *)leftNavigationBarButtonTitle;
+ (NSString *)leftNavigationBarButtonTitle;

/**
 Sets the text of the editor's right navigation bar button.
 
 Attempting to set any string besides one of the kAdobeImageEditorRightNavigationTitlePresets 
 will have no effect.
 
 @param rightNavigationBarButtonTitle An NSString value represented by one 
 of the three kAdobeImageEditorRightNavigationTitlePreset keys.
 */

+ (void)setRightNavigationBarButtonTitle:(NSString *)rightNavigationBarButtonTitle;
+ (NSString *)rightNavigationBarButtonTitle;

@end

@interface AdobeImageEditorCustomization (ToolSettings)

/**
 Sets the type and order of tools to be presented by the editor.
 
 The tool key must be one of the options listed at the top of this header.
 
 @param toolOrder An NSArray containing NSString values represented by 
 one of the tool keys.
 */

+ (void)setToolOrder:(NSArray *)toolOrder;
+ (NSArray *)toolOrder;

/**
 Enables or disables the custom crop size.
 
 The Custom crop preset does not constrain the crop area to any 
 specific aspect ratio. By default, custom crop size is enabled.
 
 @param cropToolEnableCustom YES enables the custom crop size, NO disables it.
 */

+ (void)setCropToolCustomEnabled:(BOOL)cropToolEnableCustom;
+ (BOOL)cropToolCustomEnabled;

/**
 Enables or disables the custom crop size.
 
 The Original crop preset constrains the crop area to 
 photo's original aspect ratio. By default, original crop 
 size is enabled.
 
 @param cropToolEnableOriginal YES enables the original crop size, NO disables it.
 */

+ (void)setCropToolOriginalEnabled:(BOOL)cropToolEnableOriginal;
+ (BOOL)cropToolOriginalEnabled;

/**
 Enables or disables the invertability of crop sizes.
 
 By default, inversion is enabled. Presets with names, i.e. Square, 
 are not invertible, regardless of whether inversion is enabled.
 
 @param cropToolEnableInvert YES enables the crop size inversion, 
 NO disables it.
 */

+ (void)setCropToolInvertEnabled:(BOOL)cropToolEnableInvert;
+ (BOOL)cropToolInvertEnabled;

/** Sets the availability and order of crop preset options.
 
 The dictionaries should be of the form: 
 
  @{kAdobeImageEditorCropPresetName: <NSString representing the display name>, 
    kAdobeImageEditorCropPresetWidth: <NSNumber representing width>,
    kAdobeImageEditorCropPresetHeight: <NSNumber representing height>}
 
 When the corresponding option is selected, the crop box will be constrained 
 to a kAdobeImageEditorCropPresetWidth:kAdobeImageEditorCropPresetHeight aspect ratio.
 
 If Original and/or Custom options are enabled, then they will precede the 
 presets defined here. If no crop tool presets are set, the default options 
 are Square, 3x2, 5x3, 4x3, 6x4, and 7x5.
 
 @param cropToolPresets An array of crop option dictionaries.
 */

+ (void)setCropToolPresets:(NSArray *)cropToolPresets;
+ (NSArray *)cropToolPresets;

@end

@interface AdobeImageEditorCustomization (UserMessaging)

/**
 Configures whether to show tutorials explaining the editor's features 
 to users. By default, this is set to YES.
 
 @param tutorialsEnabled whether to enable the the tutorials or not
 */

+ (void)setTutorialsEnabled:(BOOL)tutorialsEnabled;
+ (BOOL)tutorialsEnabled;

/**
 Configures whether to ask the user for a confirmation when 
 cancelling out of the editor with unsaved edits.
 
 By default, this returns YES.
 
 @param tutorialsEnabled whether to show the confirmation or not
 */

+ (void)setConfirmOnCancelEnabled:(BOOL)confirmOnCancelEnabled;
+ (BOOL)confirmOnCancelEnabled;

@end

@interface AdobeImageEditorCustomization (Localization)

/**
 Configures the editor to use localization or not.
 
 By default, Aviary enables localization.
 
 @param disableLocalization YES disables localization, 
 NO leaves it enabled.
 */

+ (void)disableLocalization:(BOOL)disableLocalization;

@end

NS_ASSUME_NONNULL_END