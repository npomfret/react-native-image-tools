/*************************************************************************
 *
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
 * suppliers and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 **************************************************************************/

#ifndef AdobeUXInAppPurchaseHeader
#define AdobeUXInAppPurchaseHeader

#import <Foundation/Foundation.h>

/**
 * Provides an upgrade status information to the invoking application after the upgrade goes through a success or failure.
 *
 * @param title A short title that will be shown to the user.
 *
 * @param message A short message that will be shown to the user.
 *
 * @param error A short error message that will be shown to the user whenever there is a purchase error.
 */
typedef void (^AdobeUXInAppPurchaseBlock)(NSString *title, NSString *message, NSError *error);

/**
 * This class provides a way to prompt the user to upgrade their Creative Cloud subscription via
 * the current application.
 */
@interface AdobeUXInAppPurchase : NSObject

+ (instancetype)sharedHandler;

/**
 * Called in the current UIApplicationDelegate's -application:openURL:sourceApplication:annotation: method
 * This method provides storage upgrade success or failure information to your application.
 *
 * @param url The @c NSURL that was passed as the URL parameter of -application:openURL:sourceApplication:annotation:
 *
 * @param sourceApplication The @c NSString that was passed as the sourceApplication parameter of
 * -application:openURL:sourceApplication:annotation:
 *
 * @param completion A completion block of type AdobeUXInAppPurchaseBlock that is called upon success or failure.
 *
 * @return Returns YES if the URL can be handled by the receiver, otherwise returns NO.
 */
- (BOOL)handleOpenURL:(NSURL *)url
      fromApplication:(NSString *)sourceApplication
       withCompletion:(AdobeUXInAppPurchaseBlock)completion;

/**
 *  Checks whether the currently authenticated user is eligible for storage upgrade.
 *
 *  @param completion A completion block that indicates whether or not a storage upgrade is available. This block is
 *  called from the application's main thread.
 *
 *  @note upgradeAvailable will be NO in the event that the user is offline or the server is unreachable.
 */
- (void)checkUpgradeAvailability:(void (^)(BOOL upgradeAvailable))completion;

/**
 * Initiates a storage upgrade. If the user has the Creative Cloud Application installed, it will be opened; if not, the user will
 * be presented with the Creative Cloud's application page in a popover (iPad) or modal (iPhone/iPod) view controller.
 *
 * @return Returns YES in the event that either the Creative Cloud Application was opened or its App Store page was presented.
 * Returns NO if the App Store page cannot be loaded.
 *
 */
- (BOOL)performStorageUpgrade;

@end

#endif
