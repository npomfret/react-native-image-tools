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

NS_ASSUME_NONNULL_BEGIN

@class AdobeImageEditorSession;

/**
 Options for defining a set of premium add ons to enable. Deprecated.
 */
typedef NS_OPTIONS(NSUInteger, AdobeImageEditorPremiumAddOn) {
    /** The option indicating no premium add ons.*/
    AdobeImageEditorPremiumAddOnNone = 0,
    /** The option indicating the high resolution premium add on.*/
    AdobeImageEditorPremiumAddOnHiRes = 1 << 0,
    /** The option indicating the white labeling premium add on.*/
    AdobeImageEditorPremiumAddOnWhiteLabel = 1 << 1,
};

@interface AdobeUXImageEditorViewController (Deprecated)

+ (void)setAPIKey:(NSString *)apiKey secret:(NSString *)secret
DEPRECATED_MSG_ATTRIBUTE("Please use the -setAuthenticationParametersWithClientID:withClientSecret: "
                         "method on the AdobeUXAuthManager singleton.");

/**
 This method is provided purely for compatibility with the legacy Aviary SDK. 
 Calls to this API have no effect. High resolution output is now free for all 
 SDK integrations. White labelling is no longer offered in the Creative SDK.
 
 @param premiumAddOns bitmask of the add-ons to enable
 */
+ (void)setPremiumAddOns:(AdobeImageEditorPremiumAddOn)premiumAddOns
DEPRECATED_MSG_ATTRIBUTE("The Aviary premium add-on APIs have been "
                         "deprecated and have no effect. This method "
                         "is provided for source compatibility.");

/**
 An AdobeImageEditorSession instance that tracks user actions within the photo editor. This can
 be used for high-resolution processing. Usage of this property for generating high resolution output
 has been deprecated in favor of the `-enqueueHighResolutionRenderWithImage:maximumSize:completion:`
 and `-enqueueHighResolutionRenderWithImage:completion:` methods.
 */
@property (nonatomic, strong, readonly) AdobeImageEditorSession *session
DEPRECATED_MSG_ATTRIBUTE("This property has been deprecated for high resolution output "
                         "in favor of -enqueueHighResolutionRenderWithImage:completion.");
@end

NS_ASSUME_NONNULL_END