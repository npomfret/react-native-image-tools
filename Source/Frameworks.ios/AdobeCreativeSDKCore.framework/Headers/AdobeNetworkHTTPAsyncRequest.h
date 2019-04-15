/******************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 * Copyright 2015 Adobe Systems Incorporated
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

#ifndef AdobeNetworkHTTPAsyncRequestHeader
#define AdobeNetworkHTTPAsyncRequestHeader

#import <Foundation/Foundation.h>

/**
 * AdobeNetworkHTTPAsyncRequest is the class by which asynchronous network operations are managed.
 *
 * It allows clients to:
 *
 * - Change the priority of asynchronous network requests that have not yet been started.
 * - Cancel asynchronous network requests.
 */
@interface AdobeNetworkHTTPAsyncRequest : NSObject

/**
 * The request ID.
 */
@property (nonatomic, readonly, strong) NSString *GUID;

/**
 * Whether the request is active.
 */
@property (nonatomic, readonly, assign, getter = isActive) BOOL active;

/**
 * @abstract get and set the request's priority
 *
 * @discussion Allows setting the request's priority relative to other queued requests. Setting this
 * property has no effect if the request is already executing.
 */
@property NSOperationQueuePriority priority;

/**
 * @abstract Cancels the asynchronous request.
 *
 * @discussion Call cancel to cancel an asynchronous request.  If the asynchronous request has not completed,
 * i.e. neither the error or completion blocks have been called, then cancel will cancel
 * the request and the cancellation block will be called upon successful cancellation. Note that it is possible
 * to receive a completion or error callback after cancel has been called in the event when the asynchronous
 * request completes before the cancellation can be processed.
 */
- (void)cancel;

@end

#endif /* ifndef AdobeNetworkHTTPAsyncRequestHeader */