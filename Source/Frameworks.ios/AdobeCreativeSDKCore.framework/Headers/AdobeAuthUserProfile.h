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
 *
 * THIS FILE IS PART OF THE CREATIVE SDK PUBLIC API
 *
 ******************************************************************************/

#ifndef AdobeAuthUserProfileHeader
#define AdobeAuthUserProfileHeader

#import <Foundation/Foundation.h>
/**
 *  Used to indicate the current license status of a given user.
 */
typedef NS_ENUM(NSUInteger, AdobeAuthUserProfileLicenseStatus) {
    
    /**
     *  Indicates the license status is in an unknown state.
     */
    AdobeAuthUserProfileLicenseStatusUnknown,
    
    /**
     *  Indicates this is a free user.
     */
    AdobeAuthUserProfileLicenseStatusFree,
    
    /**
     *  Indicates the user's account is in the trial period.
     */
    AdobeAuthUserProfileLicenseStatusTrial,
    
    /**
     *  Indicates the user is a paying user.
     */
    AdobeAuthUserProfileLicenseStatusPaid
};

/**
 *
 * AdobeAuthUserProfile contains the properties of the currently logged in user as obtained by calling
 * [AdobeUXAuthManager sharedManager].userProfile.
 *
 * See AdobeUXAuthManager
 *
 */
@interface AdobeAuthUserProfile : NSObject

/**
 * The user ID.
 */
@property (nonatomic, readonly, strong) NSString *adobeID;

/**
 * The country code for the user.
 */
@property (nonatomic, readonly, strong) NSString *countryCode;

/**
 * Whether the application key is in developer mode.
 */
@property (nonatomic, readonly) BOOL developerMode;

/**
 * The display name of the user.
 */
@property (nonatomic, readonly, strong) NSString *displayName;

/**
 * The email address of the user.
 */
@property (nonatomic, readonly, strong) NSString *email;

/**
 * Whether or not the email address has been verified.
 */
@property (nonatomic, readonly) BOOL emailVerified;

/**
 * The first name of the user.
 */
@property (nonatomic, readonly, strong) NSString *firstName;

/**
 * The full name of the user.
 */
@property (nonatomic, readonly, strong) NSString *fullName;

/**
 * Whether the user is for an enterprise user.
 */
@property (nonatomic, readonly) BOOL isEnterpriseUser;

/**
 *  Indicates the license status of the user.
 */
@property (nonatomic, readonly) AdobeAuthUserProfileLicenseStatus licenseStatus;

/**
 * The last name of the user.
 */
@property (nonatomic, readonly, strong) NSString *lastName;

/**
 * The preferred languages of the user.
 */
@property (nonatomic, readonly, strong) NSArray *preferredLanguages;

/**
 * The object in a readable format. Useful for debugging but should not be used publicly.
 */
- (NSString *)description;

@end

#endif
