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

/*
 All constants and compatibility aliases definted in this file
 are meant to provide a bridge between implementations of the legacy 
 Aviary SDK and the Adobe Image SDK. Officially, the Aviary APIs are 
 considered deprecated and are here for your convenience. These may be removed
 in future releases of the SDK.
*/

#import "AdobeUXImageEditorViewController.h"
#import "AdobeUXImageEditorViewController+InAppPurchase.h"
#import "AdobeUXImageEditorViewController+Compatibility.h"
#import "AdobeImageEditorContext.h"
#import "AdobeImageEditorCustomization.h"
#import "AdobeImageEditorOpenGLManager.h"

NS_ASSUME_NONNULL_BEGIN

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of "
                         "the AdobeUXImageEditorViewControllerDelegate protocol.")
@protocol AVYPhotoEditorControllerDelegate <AdobeUXImageEditorViewControllerDelegate> @end

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of "
                         "the AdobeUXImageEditorViewControllerDelegate protocol.")
@protocol AFPhotoEditorControllerDelegate <AdobeUXImageEditorViewControllerDelegate> @end

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of AdobeImageEditorInAppPurchaseManager.")
@protocol AVYInAppPurchaseManager <AdobeImageEditorInAppPurchaseManager> @end

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of AdobeImageEditorInAppPurchaseManager.")
@protocol AFInAppPurchaseManager <AdobeImageEditorInAppPurchaseManager> @end

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of AdobeImageEditorInAppPurchaseManagerDelegate.")
@protocol AVYInAppPurchaseManagerDelegate <AdobeImageEditorInAppPurchaseManagerDelegate> @end

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of AdobeImageEditorInAppPurchaseManagerDelegate.")
@protocol AFInAppPurchaseManagerDelegate <AdobeImageEditorInAppPurchaseManagerDelegate> @end

extern NSString *const AVYPhotoEditorSessionCancelledNotification DEPRECATED_ATTRIBUTE;
extern NSString *const kAFPhotoEditorEffectsIAPEnabledKey DEPRECATED_ATTRIBUTE;

#define ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE DEPRECATED_MSG_ATTRIBUTE("The Aviary premium add-on APIs have been " \
                                                                           "deprecated and no longer have any effect.")

// These aliases are provided for compatibility with the Aviary SDK premium add-ons API
typedef AdobeImageEditorPremiumAddOn AVYPhotoEditorPremiumAddOn ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorPremiumAddOn const AVYPhotoEditorPremiumAddOnNone;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorPremiumAddOn const AVYPhotoEditorPremiumAddOnHiRes;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorPremiumAddOn const AVYPhotoEditorPremiumAddOnWhiteLabel;

typedef AVYPhotoEditorPremiumAddOn AFPhotoEditorPremiumAddOn ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AFPhotoEditorPremiumAddOn const AFPhotoEditorPremiumAddOnNone;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AFPhotoEditorPremiumAddOn const AFPhotoEditorPremiumAddOnHiRes;
ADOBE_PREMIUM_ADD_ON_DEPRECATED_ATTRIBUTE extern AFPhotoEditorPremiumAddOn const AFPhotoEditorPremiumAddOnWhiteLabel;

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated in favor of AdobeImageEditorRender")
@protocol AVYPhotoEditorRender <AdobeImageEditorRender> @end

#define ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE DEPRECATED_MSG_ATTRIBUTE ("The Aviary high-resolution APIs and types have been " \
                                                                             "deprecated in favor of those defined in AdobeUXImageEditorViewController.h")

typedef AdobeImageEditorRenderCompletion AVYPhotoEditorRenderCompletion ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE;
typedef AdobeImageEditorHighResolutionErrorCode AVYPhotoEditorHighResolutionErrorCode ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE;
ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorHighResolutionErrorCode const AVYPhotoEditorHighResolutionErrorCodeUnknown;
ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorHighResolutionErrorCode const AVYPhotoEditorHighResolutionErrorCodeUserCancelled;
ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorHighResolutionErrorCode const AVYPhotoEditorHighResolutionErrorCodeNoModifications;
ADOBE_HIGH_RESOLUTION_DEPRECATED_ATTRIBUTE extern AVYPhotoEditorHighResolutionErrorCode const AVYPhotoEditorHighResolutionErrorCodeRenderCancelled;

NSString *const _AVYPhotoEditorErrorDomain(void) DEPRECATED_MSG_ATTRIBUTE("AVYPhotoEditorErrorDomain has been deprecated in favor of AdobeImageEditorErrorDomain");
#define AVYPhotoEditorErrorDomain (_AVYPhotoEditorErrorDomain())

extern NSString *const kAVYProductEffectsGrunge;
extern NSString *const kAVYProductEffectsNostalgia;
extern NSString *const kAVYProductEffectsViewfinder;

NSString *const _kAFProductEffectsGrunge(void);
#define kAFProductEffectsGrunge _kAFProductEffectsGrunge()

NSString *const _kAFProductEffectsNostalgia(void);
#define kAFProductEffectsNostalgia _kAFProductEffectsNostalgia()

NSString *const _kAFProductEffectsViewfinder(void);
#define kAFProductEffectsViewfinder _kAFProductEffectsViewfinder()

#pragma clang diagnostic pop

@compatibility_alias AVYPhotoEditorController AdobeUXImageEditorViewController;
@compatibility_alias AVYPhotoEditorContext AdobeImageEditorContext;
@compatibility_alias AVYPhotoEditorSession AdobeImageEditorSession;
@compatibility_alias AVYPhotoEditorProduct AdobeImageEditorProduct;
@compatibility_alias AVYPhotoEditorCustomization AdobeImageEditorCustomization;
@compatibility_alias AVYOpenGLManager AdobeImageEditorOpenGLManager;

@compatibility_alias AFPhotoEditorController AdobeUXImageEditorViewController;
@compatibility_alias AFPhotoEditorContext AdobeImageEditorContext;
@compatibility_alias AFPhotoEditorSession AdobeImageEditorSession;
@compatibility_alias AFPhotoEditorProduct AdobeImageEditorProduct;
@compatibility_alias AFPhotoEditorCustomization AdobeImageEditorCustomization;
@compatibility_alias AFOpenGLManager AdobeImageEditorOpenGLManager;

NS_ASSUME_NONNULL_END