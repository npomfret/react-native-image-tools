/******************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 * Copyright 2013 Adobe Systems Incorporated
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains the property of
 * Adobe Systems Incorporated and its suppliers, if any. The intellectual and
 * technical concepts contained herein are proprietary to Adobe Systems
 * Incorporated and its suppliers and are protected by trade secret or
 * copyright law. Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 ******************************************************************************/

#ifndef AdobeCreativeSDKVersionHeader
#define AdobeCreativeSDKVersionHeader

#import <Foundation/Foundation.h>

/**
 Check for the USE_CSDK_COMPONENTS macro definition in the project.
 This has been deprecated and is no longer necessary.
 In the next Public SDK release, this should be removed.
 */
#ifdef USE_CSDK_COMPONENTS
#error "Use of the USE_CSDK_COMPONENTS Preprocessor Macro has been deprecated. Remove it from your project."
#endif

/**
 * AdobeCreativeSDKVersion allows the client to obtain version information for each
 * CreativeSDK framework linked into their app.
 */
@interface AdobeCreativeSDKVersion : NSObject

/**
 * Get the version information for all of the Creative SDK frameworks linked into your app.
 *
 * @return An NSArray of NSDictionaries that contain framwork version information.
 * The returned array contains one dictionary for each framework linked into the client app.
 *
 * @note Each dictionary in the array has three keys: "Name", "Version", and "Branch", each of which
 * have string values.  "Name" is the name of the framework, "Version" is the version of the framework,
 * and "Branch" refers to the code branch from which the framework was built.
 */
+ (NSArray *)frameworkVersions;

@end

#endif
