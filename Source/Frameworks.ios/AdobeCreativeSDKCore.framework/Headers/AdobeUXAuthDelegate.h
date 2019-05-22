/*************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2016 Adobe Systems Incorporated
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
 *
 **************************************************************************/

#ifndef AdobeAuthDelegateHeader
#define AdobeAuthDelegateHeader

@protocol AdobeUXAuthDelegate <NSObject>

@optional

/**
 * Called when the Creative SDK detects an authentication event that requires user interaction.
 * The Creative SDK will not complete any additional server actions until the user interaction is
 * completed.  "Terms of Use acceptance" and "Email verification" are examples of such events.
 * The client must dispatch the delegate code on the main thread as needed.
 *
 * The client application is expected to save the event dictionary and use it to open the user
 * interaction URL when it's convenient to display UI.  The client can also choose to logout.
 * The CSDK will open the user interaction URL on a subsequent login.  The application will logout
 * if the delegate method is not implemented.  AdobeUXAuthManager openContinuableAuthenticationURLForError:
 * must be used to open the URL.
 *
 * @param event  Dictionary that contains the information needed to open the user interaction web page.
 */
- (void)detectedContinuableAuthenticationEvent:(NSDictionary *)event;

@end

#endif