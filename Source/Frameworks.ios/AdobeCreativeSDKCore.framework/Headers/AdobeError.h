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

#ifndef AdobeErrorHeader
#define AdobeErrorHeader

#import <Foundation/Foundation.h>

/** These keys may appear in the NSError userInfo dictionary for errors
 * in the AdobeErrorDomains.
 */

extern NSString *const AdobeErrorPathKey;             /**< userInfo key returning an NSString* containing
                                                       * the path of a local file that caused the error. */
extern NSString *const AdobeErrorOtherErrorsKey;      /**< userInfo key returning an NSArray containing
                                                       * other errors that happened in parallel. */
extern NSString *const AdobeErrorComponentErrorsKey;  /**< userInfo key returning an NSDictionary containing
                                                       * the component IDs associated with the individual errors. */
extern NSString *const AdobeErrorDetailsStringKey;    /**< userInfo key returning an NSString. containing
                                                       * a clear text description of the problem. */

#endif
