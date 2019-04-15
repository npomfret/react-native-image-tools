/******************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 * Copyright 2014 Adobe Systems Incorporated
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains the property of
 * Adobe Systems Incorporated and its suppliers, if any. The intellectual and
 * technical concepts contained herein are proprietary to Adobe Systems
 * Incorporated and its suppliers and are protected by trade secret or
 * copyright law. Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 *
 * THIS FILE IS PART OF THE CREATIVE SDK PUBLIC API
 *
 ******************************************************************************/

/**
 *
 * AdobeAuthErrorCode is an enumerated type that specifies the type of error that
 * may occur during the authentication process.
 *
 * See AdobeUXAuthManager
 *
 */

#ifndef AdobeAuthErrorHeader
#define AdobeAuthErrorHeader

#import <Foundation/Foundation.h>

extern NSString *const AdobeAuthErrorDomain;

typedef NS_ENUM (NSInteger, AdobeAuthErrorCode)
{
    /** Indicates that the authentication process failed because the application cannot connect to the Internet. */
    AdobeAuthErrorCodeOffline = kCFURLErrorNotConnectedToInternet,
    /** no error */
    AdobeAuthErrorCodeNoError = 0,
    /** Indicates that the authentication process has been cancelled by the user. */
    AdobeAuthErrorCodeUserCancelled = 1,
    /** Indicates that the login process cannot proceed without user interactions. */
    AdobeAuthErrorCodeUserInteractionRequired = 2,
    /** Indicates that a valid username and password is not provided. */
    AdobeAuthErrorCodeUsernameAndPasswordRequired = 3,
    /** Indicates that a Device ID is required to continue with the authentication process. */
    AdobeAuthErrorCodeDeviceIDRequired = 4,
    /** Indicates that a Client ID is required to continue with the authentication process. */
    AdobeAuthErrorCodeClientIDRequired = 5,
    /** Invalid argument */
    AdobeAuthErrorCodeInvalidArgument = 6,
    /** Indicates that the user is not entitled to send assets to a desktop application. */
    AdobeAuthErrorCodeUserNotEntitled = 7,
    /** Indicates that valid user credentials are required. */
    AdobeAuthErrorCodeUserCredentialsRequired = 8,
    /** Indicates that the user is already authenticated. */
    AdobeAuthErrorCodeUserAlreadyAuthenticated = 9,
    /** Indicates that the API is not available. */
    AdobeAuthErrorCodeAPINotAvailable = 10,
    /** Indicates that an aunknow error has occured during the authentication process. */
    AdobeAuthErrorCodeUnknownError = 99
};

@interface AdobeAuthError : NSObject

@end

#endif
